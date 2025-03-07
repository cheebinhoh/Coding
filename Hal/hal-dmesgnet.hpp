/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * The Hal_DMesgNet stands for HAL Distributed Messaging over Network,
 * it acts an extension to the Hal_DMesg with support of serializing
 * the DMesgPb message and send over the output Hal_Io and deserialize
 * the data read from input Hal_Io and then publish to the local
 * subscripted handlers.
 */

#ifndef HAL_DMESGNET_HPP_HAVE_SEEN

#define HAL_DMESGNET_HPP_HAVE_SEEN

#include "hal-debug.hpp"
#include "hal-dmesg-pb-util.hpp"
#include "hal-dmesg.hpp"
#include "hal-io.hpp"
#include "hal-timer.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <sys/time.h>

namespace Hal {

#define HAL_DMESGNET_HEARTBEAT_IN_NS (1000000000)
#define HAL_DMESGNET_MASTERPENDING_MAX_COUNTER (3)

class Hal_DMesgNet : public Hal_DMesg {
public:
  /**
   * 1.    Initialized - [send heartbeat] -> MasterElectionPending
   * 2[1]. MasterPending - [receive master heartbeat] -> Ready
   * 2[2]. MasterPending - [timeout w/o master heartbeat, act as master]
   *       -> Ready
   * 3[1]. Ready - [receive master last heartbeat] -> MasterPending
   * 3[2]. Ready - [send last heartbeat, optionally relinquish master role]
   *       -> Destroyed
   * 4.    Destroyed [cache last state in file?]
   *
   * When a master is about to be destroyed, its last heartbeat message will
   * relinquish its master role, that will trigger 3[1] for all other nodes,
   * and instead of moving all other nodes into passive master election 2[1]
   * or 2[2], all other nodes can do co-election of master by:
   * - each node selects the earliest created timestamp node from the list of
   *   neighbor nodes (including itself).
   * - if all remaining nodes have same list of neighbor nodes, they will all
   *   elect the same master, and all are in sync.
   * - otherwise, different master will be elected by different nodes, but as
   *   those nodes are syncing their heartbeat, they will all agree on node
   *   with earliest created timestamp.
   *
   * for each heartbeat broadcast message from the node, it includes the
   * following information:
   * - the node identifier [timestamp_at_initialized.process_id.ip]
   * - the node's master identifier which
   * - the node's known list of networked node identifiers (include itself)
   *
   * If there are 3 nodes, A, B and C. And it boots up in the following orders:
   *
   * T1: A boot up, and send heartbeat [A, , []]
   * T2: B boot up, and send heartbeat [B, , []]
   * T3: A receives [B, , []], and add B to its neighbor list, but B is
   *     not master, so A remains in initialized (as not yet timeout),
   *     and its next pending heartbeat is [A, , [A, B]]
   * T4: B receives [A, , []], and add A to its neighbor list, but A is
   *     not master, so B remains in initialized (as not yet timeout),
   *     and its next pending heartbeat is [B, , [A, B]]
   * T5: A timeout at initialized state, and self-proclaim itself as
   *     master based on its list of neighbor nodes [A, B] as A
   *     has earliest timestamp at created.
   * T6: B timeout at initialized state, and select A as the master based
   *     on its list of neighbor nodes [A, B] and A has earliest timestamp
   *     at created.
   * T7: both A and B receives each other heartbeat, and then
   *     both agree that A is master as A has early timestamp on created,
   *     and so both will remain in ready state.
   *     heartbeat A = [A, A, [A, B]] B = [B, A, [A, B]].
   * T8: when C boots up, it is in initialized state, and send out
   *     heartbeat C = [C, , ,]
   * T9: A and B receives C heartbeat and both send out heartbeat
   *     A = [A, A, [A, B, C]] and B = [B, A, [A, B, C]], and C receives
   *     either of the message will declare A as master and into ready
   *     state, also keep a list of neighbor nodes [A, B, C].
   *
   * Tn: A is shutting down, and manage to send its last heartbeat,
   *     [A, ,[B, C]] to B and C, and both will notice that the message
   *     is from A but it relinquishes master position, and remove itself
   *     from the neighbor list, and B and C will elect new master
   *     which is B as it has earliest timestamp.
   *
   * The node will maintain its list of neighbor nodes, but if there is no
   * heartbeat from one of the neighbor for N seconds, it will remove it from
   * the list, if the master is not sending heartbeat, then all nodes will
   * participate in master selection by using it last known list of networed
   * nodes (include itself) and elect one with early timestamp as master, and
   * send out the next heartbeat, in good case, all active nodes will agree on
   * the same new master right away.
   *
   * In very race case, two nodes elect different master as one node does not
   * see one or few other nodes that are better candidate for master (early
   * timestamp), then it will surrender its master election result and agree on
   * one selected by its neighbor heartbeat message.
   */

  Hal_DMesgNet(std::string_view name,
               std::shared_ptr<Hal_Io<std::string>> outputHandler = nullptr,
               std::shared_ptr<Hal_Io<std::string>> inputHandler = nullptr)
      : Hal_DMesg{name}, m_name{name}, m_outputHandler{outputHandler},
        m_inputHandler{inputHandler} {

    struct timeval tv;
    gettimeofday(&tv, NULL);

    this->m_runningCounter++;

    DMESG_PB_SET_TIMESTAMP(this->m_sys, tv);
    DMESG_PB_SET_IDENTIFIER(this->m_sys, DMesgSysIdentifier);
    DMESG_PB_SET_SOURCEIDENTIFIER(this->m_sys, DMesgSysIdentifier);
    DMESG_PB_SET_RUNNINGCOUNTER(this->m_sys, this->m_runningCounter);
    DMESG_PB_SET_TYPE(this->m_sys, Hal::DMesgTypePb::sys);
    DMESG_PB_SYS_SET_TIMESTAMP(this->m_sys, tv);

    auto *self = this->m_sys.mutable_body()->mutable_sys()->mutable_self();
    DMESG_PB_SYS_NODE_SET_INITIALIZEDTIMESTAMP(self, tv);
    DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP(self, tv);
    DMESG_PB_SYS_NODE_SET_IDENTIFIER(self, this->m_name);
    DMESG_PB_SYS_NODE_SET_STATE(self, Hal::DMesgStatePb::MasterPending);
    DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, "");

    // subscriptHandler to read and write with local DMesg
    m_subscriptHandler = Hal_DMesg::openHandler(
        m_name,
        true, // include DMesgSys!
        [this](const Hal::DMesgPb &dmesgPb) {
          return dmesgPb.sourceidentifier() != this->m_name;
        },
        [this](Hal::DMesgPb dmesgPb) mutable {
          if (m_outputHandler) {
            std::string serialized_string{};

            // set the source, so that we can skip it if the
            // data is read back over the input Hal_Io.
            //
            // FIXME: shall we generate UUID internally to guarantee
            // uniqueness across networked nodes or global Internet?
            //
            // This is point we check if outgoing is in conflict
            // for the message stream with the identifier.
            dmesgPb.set_sourceidentifier(this->m_name);
            dmesgPb.SerializeToString(&serialized_string);

            m_outputHandler->write(serialized_string);
          }
        });

    if (m_inputHandler) {
      m_inputProc = std::make_unique<Hal_Proc>(m_name + "_inputProc", [this]() {
        while (true) {
          Hal::DMesgPb dmesgPbRead{};

          auto data = this->m_inputHandler->read();
          if (data) {
            dmesgPbRead.ParseFromString(*data);

            if (dmesgPbRead.sourceidentifier() != this->m_name) {
              // this is important to prevent that the
              // m_subscriptHandler of this DMesgNet from
              // reading this message again and send out.
              //
              // the Hal_DMesgHandler->write will add the name
              // of the Hal_DMesgHandler from read it again,
              // but it is good to be explicit.
              //
              // FIXME: the source is now used by both application
              // and the DMesg protocol level, maybe we should
              // separate the namespace of source.

              if (dmesgPbRead.type() == Hal::DMesgTypePb::sys) {
                this->write([this, dmesgPbRead]() mutable {
                  auto other = dmesgPbRead.body().sys().self();
                  auto self =
                      this->m_sys.mutable_body()->mutable_sys()->mutable_self();
                  std::string masterIdentifier{};

                  if (other.masteridentifier() != "") {
                    assert(other.state() == Hal::DMesgStatePb::Ready);
                    masterIdentifier = other.masteridentifier();
                  }

                  if (self->state() == Hal::DMesgStatePb::MasterPending &&
                      self->masteridentifier() != masterIdentifier) {
                    DMESG_PB_SYS_NODE_SET_STATE(self, Hal::DMesgStatePb::Ready);
                    DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self,
                                                           masterIdentifier);

                    struct timeval tv;
                    gettimeofday(&tv, NULL);

                    DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP(self, tv);
                    this->m_sysHandler->write(this->m_sys);
                  }
                });
              } else {
                dmesgPbRead.set_sourceidentifier(this->m_name);

                this->m_subscriptHandler->write(dmesgPbRead);
              }
            }
          } else {
            // no data, quit the thread.
            // FIXME: in what case will it has no data? inputHandler being
            // release and the chain of objects in DMesg are destroyed?
            break;
          }
        }
      });

      m_inputProc->exec();

      m_sysHandler = Hal_DMesg::openHandler(
          m_name + "_sys",
          [this](const Hal::DMesgPb &dmesgPb) { return false; }, nullptr);
    }

    if (m_inputHandler && m_outputHandler) {
      // into MasterPending
      m_timerProc = std::make_unique<Hal::Hal_Timer<std::chrono::nanoseconds>>(
          std::chrono::nanoseconds(HAL_DMESGNET_HEARTBEAT_IN_NS), [this]() {
            this->write([this]() mutable {
              if (this->m_sys.body().sys().self().state() ==
                  Hal::DMesgStatePb::MasterPending) {
                this->m_masterPendingCounter++;

                if (this->m_masterPendingCounter >=
                    HAL_DMESGNET_MASTERPENDING_MAX_COUNTER) {
                  this->m_masterPendingCounter++;

                  auto *self =
                      this->m_sys.mutable_body()->mutable_sys()->mutable_self();
                  DMESG_PB_SYS_NODE_SET_STATE(self, Hal::DMesgStatePb::Ready);
                  DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, this->m_name);

                  struct timeval tv;
                  gettimeofday(&tv, NULL);

                  DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP(self, tv);
                }
              }

              HAL_DEBUG_PRINT(
                  std::cout
                  << "self: " << this->m_sys.body().sys().self().identifier()
                  << ", master: "
                  << this->m_sys.body().sys().self().masteridentifier()
                  << "\n");
              this->m_sysHandler->write(this->m_sys);
            });
          });
    } else {
      auto *self = this->m_sys.mutable_body()->mutable_sys()->mutable_self();
      DMESG_PB_SYS_NODE_SET_STATE(self, Hal::DMesgStatePb::Ready);
      DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, this->m_name);
    }

    if (m_sysHandler) {
      m_sysHandler->write(this->m_sys);
    }
  }

  virtual ~Hal_DMesgNet() noexcept try {
  } catch (...) {
    // explicit return to resolve exception as destructor must be noexcept
    return;
  }

  Hal_DMesgNet(const Hal_DMesgNet &halDMesgNet) = delete;
  const Hal_DMesgNet &operator=(const Hal_DMesgNet &halDMesgNet) = delete;
  Hal_DMesgNet(Hal_DMesgNet &&halDMesgNet) = delete;
  Hal_DMesgNet &operator=(Hal_DMesgNet &&halDMesgNet) = delete;

private:
  std::string m_name{};
  std::shared_ptr<Hal_Io<std::string>> m_outputHandler{};
  std::shared_ptr<Hal_Io<std::string>> m_inputHandler{};

  std::unique_ptr<Hal::Hal_Proc> m_inputProc{};
  std::shared_ptr<Hal_DMesgHandler> m_subscriptHandler{};
  std::shared_ptr<Hal_DMesgHandler> m_sysHandler{};
  std::unique_ptr<Hal::Hal_Timer<std::chrono::nanoseconds>> m_timerProc{};

  Hal::DMesgPb m_sys{};
  long long m_runningCounter{};
  long long m_masterPendingCounter{};
};

} /* namespace Hal */

#endif /* HAL_DMESGNET_HPP_HAVE_SEEN */
