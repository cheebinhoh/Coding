/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * The Dmn_DMesgNet stands for Distributed Messaging network Network,
 * it acts an extension to the Dmn_DMesg with support of serializing
 * the DMesgPb message and send over the output Dmn_Io and deserialize
 * the data read from input Dmn_Io and then publish to the local
 * subscripted handlers.
 */

#ifndef DMN_DMESGNET_HPP_HAVE_SEEN

#define DMN_DMESGNET_HPP_HAVE_SEEN

#include "dmn-debug.hpp"
#include "dmn-dmesg-pb-util.hpp"
#include "dmn-dmesg.hpp"
#include "dmn-io.hpp"
#include "dmn-timer.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <sys/time.h>

namespace Dmn {

#define DMN_DMESGNET_HEARTBEAT_IN_NS (1000000000)
#define DMN_DMESGNET_MASTERPENDING_MAX_COUNTER (3)
#define DMN_DMESGNET_MASTERSYNC_MAX_COUNTER (5)

class Dmn_DMesgNet : public Dmn_DMesg {
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

  Dmn_DMesgNet(std::string_view name,
               std::shared_ptr<Dmn_Io<std::string>> outputHandler = nullptr,
               std::shared_ptr<Dmn_Io<std::string>> inputHandler = nullptr)
      : Dmn_DMesg{name}, m_name{name}, m_outputHandler{outputHandler},
        m_inputHandler{inputHandler} {

    struct timeval tv;
    gettimeofday(&tv, NULL);

    DMESG_PB_SET_TOPIC(this->m_sys, DMesgSysIdentifier);
    DMESG_PB_SET_TYPE(this->m_sys, Dmn::DMesgTypePb::sys);

    DMESG_PB_SYS_SET_TIMESTAMP(this->m_sys, tv);

    auto *self = this->m_sys.mutable_body()->mutable_sys()->mutable_self();
    DMESG_PB_SYS_NODE_SET_INITIALIZEDTIMESTAMP(self, tv);
    DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP(self, tv);
    DMESG_PB_SYS_NODE_SET_IDENTIFIER(self, this->m_name);
    DMESG_PB_SYS_NODE_SET_STATE(self, Dmn::DMesgStatePb::MasterPending);
    DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, "");

    // subscriptHandler to read and write with local DMesg
    m_subscriptHandler = Dmn_DMesg::openHandler(
        m_name,
        true, // include DMesgSys!
        [this](const Dmn::DMesgPb &dmesgPb) {
          return dmesgPb.sourceidentifier() != this->m_name;
        },
        [this](Dmn::DMesgPb dmesgPb) mutable {
          if (m_outputHandler) {
            std::string serialized_string{};

            // set the source, so that we can skip it if the
            // data is read back over the input Dmn_Io.
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
      m_inputProc = std::make_unique<Dmn_Proc>(m_name + "_inputProc", [this]() {
        while (true) {
          Dmn::DMesgPb dmesgPbRead{};

          auto data = this->m_inputHandler->read();
          if (data) {
            dmesgPbRead.ParseFromString(*data);

            if (dmesgPbRead.sourceidentifier() != this->m_name) {
              // this is important to prevent that the
              // m_subscriptHandler of this DMesgNet from
              // reading this message again and send out.
              //
              // the Dmn_DMesgHandler->write will add the name
              // of the Dmn_DMesgHandler from read it again,
              // but it is good to be explicit.
              //
              // FIXME: the source is now used by both application
              // and the DMesg protocol level, maybe we should
              // separate the namespace of source.

              if (dmesgPbRead.type() == Dmn::DMesgTypePb::sys) {
                this->write([this, dmesgPbRead]() mutable {
                  this->reconciliateDmesgPbSys(dmesgPbRead);
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

      m_sysHandler = Dmn_DMesg::openHandler(
          m_name + "_sys",
          [this](const Dmn::DMesgPb &dmesgPb) { return false; }, nullptr);
    }

    if (m_inputHandler && m_outputHandler) {
      // into MasterPending
      m_timerProc = std::make_unique<Dmn::Dmn_Timer<std::chrono::nanoseconds>>(
          std::chrono::nanoseconds(DMN_DMESGNET_HEARTBEAT_IN_NS), [this]() {
            this->write([this]() mutable {
              if (this->m_sys.body().sys().self().state() ==
                  Dmn::DMesgStatePb::MasterPending) {
                this->m_masterPendingCounter++;

                if (this->m_masterPendingCounter >=
                    DMN_DMESGNET_MASTERPENDING_MAX_COUNTER) {
                  this->m_masterPendingCounter = 0;

                  auto *self =
                      this->m_sys.mutable_body()->mutable_sys()->mutable_self();
                  DMESG_PB_SYS_NODE_SET_STATE(self, Dmn::DMesgStatePb::Ready);
                  DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, this->m_name);

                  struct timeval tv;
                  gettimeofday(&tv, NULL);

                  DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP(self, tv);
                }
              } else if (this->m_sys.body().sys().self().state() ==
                         Dmn::DMesgStatePb::Ready) {
                if (this->m_sys.body().sys().self().masteridentifier() !=
                    this->m_sys.body().sys().self().identifier()) {
                  this->m_masterSyncPendingCounter++;

                  if (this->m_masterSyncPendingCounter >=
                      DMN_DMESGNET_MASTERSYNC_MAX_COUNTER) {
                    this->m_masterSyncPendingCounter = 0;
                    this->m_lastRemoteMasterTimestamp = {};

                    auto *self = this->m_sys.mutable_body()
                                     ->mutable_sys()
                                     ->mutable_self();

                    DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, "");
                    DMESG_PB_SYS_NODE_SET_STATE(
                        self, Dmn::DMesgStatePb::MasterPending);
                  }
                }
              }

              this->m_sysHandler->write(this->m_sys);
            });
          });
    } else {
      auto *self = this->m_sys.mutable_body()->mutable_sys()->mutable_self();
      DMESG_PB_SYS_NODE_SET_STATE(self, Dmn::DMesgStatePb::Ready);
      DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, this->m_name);
    }

    if (m_sysHandler) {
      m_sysHandler->write(this->m_sys);
    }
  }

  virtual ~Dmn_DMesgNet() noexcept try {
    if (m_sysHandler && m_outputHandler) {
      // it is about to destroy the Dmn_DMesgNet and free everything
      // it will send last heartbeat and reliquinsh itself as master (if
      // itself is master).
      //
      // we avoid use of m_sysHandler as we are to destroy it, so we
      // do not want to hold the object life up and have to wait for
      // asynchrononous action to send last heartbeat messge.
      struct timeval tv;
      gettimeofday(&tv, NULL);

      DMESG_PB_SET_SOURCEIDENTIFIER(this->m_sys, this->m_name);

      auto *self = this->m_sys.mutable_body()->mutable_sys()->mutable_self();

      DMESG_PB_SYS_NODE_SET_STATE(self, Dmn::DMesgStatePb::Destroyed);
      DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, "");
      DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP(self, tv);

      std::string serialized_string{};
      this->m_sys.SerializeToString(&serialized_string);

      m_outputHandler->write(serialized_string);
    }
  } catch (...) {
    // explicit return to resolve exception as destructor must be noexcept
    return;
  }

  Dmn_DMesgNet(const Dmn_DMesgNet &dmnDMesgNet) = delete;
  const Dmn_DMesgNet &operator=(const Dmn_DMesgNet &dmnDMesgNet) = delete;
  Dmn_DMesgNet(Dmn_DMesgNet &&dmnDMesgNet) = delete;
  Dmn_DMesgNet &operator=(Dmn_DMesgNet &&dmnDMesgNet) = delete;

protected:
  void reconciliateDmesgPbSys(Dmn::DMesgPb dmesgPbOther) {
    auto other = dmesgPbOther.body().sys().self();
    auto self = this->m_sys.mutable_body()->mutable_sys()->mutable_self();

    struct timeval tv;
    gettimeofday(&tv, NULL);

    if (self->state() == Dmn::DMesgStatePb::MasterPending &&
        other.state() == Dmn::DMesgStatePb::Ready) {
      assert(self->masteridentifier() == "");
      assert(other.masteridentifier() != "");

      DMESG_PB_SYS_NODE_SET_STATE(self, Dmn::DMesgStatePb::Ready);
      DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, other.masteridentifier());

      DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP(self, tv);

      this->m_lastRemoteMasterTimestamp = tv;
      this->m_masterPendingCounter = 0;
      this->m_masterSyncPendingCounter = 0;
      this->m_sysHandler->write(this->m_sys);
    } else if (self->state() == Dmn::DMesgStatePb::Ready) {
      if (other.identifier() == self->masteridentifier()) {
        if (other.state() == Dmn::DMesgStatePb::Ready) {
          this->m_masterSyncPendingCounter = 0;
          this->m_lastRemoteMasterTimestamp = tv;
        } else {
          assert(self->masteridentifier() != "");
          assert(other.masteridentifier() == "");

          DMESG_PB_SYS_NODE_SET_STATE(self, Dmn::DMesgStatePb::MasterPending);
          DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, "");

          DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP(self, tv);

          this->m_lastRemoteMasterTimestamp = tv;
          this->m_masterPendingCounter = 0;
          this->m_masterSyncPendingCounter = 0;
          this->m_sysHandler->write(this->m_sys);
        }
      }
    }

    int i = 0;
    while (i < this->m_sys.mutable_body()->mutable_sys()->nodelist().size()) {
      if (other.identifier() == this->m_sys.mutable_body()
                                    ->mutable_sys()
                                    ->nodelist()
                                    .Get(i)
                                    .identifier()) {
        break;
      }

      i++;
    }

    if (other.state() == Dmn::DMesgStatePb::Destroyed) {
      if (i >= this->m_sys.mutable_body()->mutable_sys()->nodelist().size()) {
        // do nothing
      } else {
        this->m_sys.mutable_body()
            ->mutable_sys()
            ->mutable_nodelist()
            ->DeleteSubrange(i, 1);
      }
    } else {
      if (i >= this->m_sys.mutable_body()->mutable_sys()->nodelist().size()) {
        this->m_sys.mutable_body()->mutable_sys()->add_nodelist();
      }

      this->m_sys.mutable_body()
          ->mutable_sys()
          ->mutable_nodelist(i)
          ->set_identifier(other.identifier());
      this->m_sys.mutable_body()
          ->mutable_sys()
          ->mutable_nodelist(i)
          ->set_masteridentifier(other.masteridentifier());
      this->m_sys.mutable_body()->mutable_sys()->mutable_nodelist(i)->set_state(
          other.state());
      this->m_sys.mutable_body()
          ->mutable_sys()
          ->mutable_nodelist(i)
          ->mutable_initializedtimestamp()
          ->set_seconds(other.initializedtimestamp().seconds());
      this->m_sys.mutable_body()
          ->mutable_sys()
          ->mutable_nodelist(i)
          ->mutable_initializedtimestamp()
          ->set_nanos(other.initializedtimestamp().nanos());
      this->m_sys.mutable_body()
          ->mutable_sys()
          ->mutable_nodelist(i)
          ->mutable_updatedtimestamp()
          ->set_seconds(other.updatedtimestamp().seconds());
      this->m_sys.mutable_body()
          ->mutable_sys()
          ->mutable_nodelist(i)
          ->mutable_initializedtimestamp()
          ->set_nanos(other.updatedtimestamp().nanos());
    }
  }

private:
  std::string m_name{};
  std::shared_ptr<Dmn_Io<std::string>> m_outputHandler{};
  std::shared_ptr<Dmn_Io<std::string>> m_inputHandler{};

  std::unique_ptr<Dmn::Dmn_Proc> m_inputProc{};
  std::shared_ptr<Dmn_DMesgHandler> m_subscriptHandler{};
  std::shared_ptr<Dmn_DMesgHandler> m_sysHandler{};
  std::unique_ptr<Dmn::Dmn_Timer<std::chrono::nanoseconds>> m_timerProc{};

  Dmn::DMesgPb m_sys{};
  long long m_masterPendingCounter{};
  long long m_masterSyncPendingCounter{};
  struct timeval m_lastRemoteMasterTimestamp {};
};

} /* namespace Dmn */

#endif /* DMN_DMESGNET_HPP_HAVE_SEEN */
