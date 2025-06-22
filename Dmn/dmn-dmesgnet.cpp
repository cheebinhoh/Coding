/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * @file dmn-dmesgnet.cpp
 * @brief The source implementation file for dmn-dmesgnet.
 */

#include "dmn-dmesgnet.hpp"

#include <sys/time.h>

#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include "dmn-debug.hpp"
#include "dmn-dmesg-pb-util.hpp"
#include "dmn-dmesg.hpp"
#include "dmn-io.hpp"
#include "dmn-timer.hpp"

namespace dmn {

Dmn_DMesgNet::Dmn_DMesgNet(std::string_view name,
                           std::unique_ptr<Dmn_Io<std::string>> input_handler,
                           std::unique_ptr<Dmn_Io<std::string>> output_handler)
    : Dmn_DMesg{name}, m_name{name}, m_input_handler{std::move(input_handler)},
      m_output_handler{std::move(output_handler)} {

  // Initialize the DMesgNet state
  struct timeval tv;
  gettimeofday(&tv, NULL);

  DMESG_PB_SET_MSG_TOPIC(this->m_sys, kDMesgSysIdentifier);
  DMESG_PB_SET_MSG_TYPE(this->m_sys, dmn::DMesgTypePb::sys);

  DMESG_PB_SYS_SET_TIMESTAMP_FROM_TV(this->m_sys, tv);

  auto *self = this->m_sys.mutable_body()->mutable_sys()->mutable_self();
  DMESG_PB_SYS_NODE_SET_INITIALIZEDTIMESTAMP_FROM_TV(self, tv);
  DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP_FROM_TV(self, tv);
  DMESG_PB_SYS_NODE_SET_IDENTIFIER(self, this->m_name);
  DMESG_PB_SYS_NODE_SET_STATE(self, dmn::DMesgStatePb::MasterPending);
  DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, "");

  auto handlerConfig = Dmn_DMesg::kHandlerConfig_Default;
  handlerConfig[Dmn_DMesg::kHandlerConfig_IncludeSys] = "yes";
  handlerConfig[Dmn_DMesg::kHandlerConfig_NoTopicFilter] = "yes";

  // subscriptHandler to read and write with local DMesg
  m_subscript_handler = Dmn_DMesg::openHandler(
      m_name,
      [this](const dmn::DMesgPb &dmesgPb) {
        return dmesgPb.sourcewritehandleridentifier() != this->m_name;
      },
      [this](dmn::DMesgPb dmesgPbWrite) mutable {
        if (m_output_handler) {
          DMN_ASYNC_CALL_WITH_CAPTURE(
              {
                std::string serialized_string{};

                // set the source, so that we can skip it if the
                // data is read back over the input Dmn_Io.
                //
                // FIXME: shall we generate UUID internally to guarantee
                // uniqueness across networked nodes or global Internet?
                //
                // This is point we check if outgoing is in conflict
                // for the message stream with the identifier.
                DMESG_PB_SET_MSG_SOURCEWRITEHANDLERIDENTIFIER(dmesgPbWrite,
                                                              this->m_name);
                dmesgPbWrite.SerializeToString(&serialized_string);
                m_output_handler->write(serialized_string);

                if (dmesgPbWrite.type() != dmn::DMesgTypePb::sys) {
                  m_topic_last_dmesgpb[dmesgPbWrite.topic()] = dmesgPbWrite;
                }
              },
              this, dmesgPbWrite);
        }
      },
      handlerConfig);

  if (m_input_handler) {
    m_input_proc = std::make_unique<Dmn_Proc>(m_name + "_inputProc", [this]() {
      bool stop{};

      while ((!stop) && this->m_input_handler) {
        dmn::DMesgPb dmesgpb_read{};

        auto data = this->m_input_handler->read();
        Dmn_Proc::yield();

        if (data) {
          dmesgpb_read.ParseFromString(*data);
          if (dmesgpb_read.sourcewritehandleridentifier() == this->m_name) {
            continue;
          }

          // this is important to prevent that the
          // m_subscript_handler of this DMesgNet from
          // reading this message again and send out.
          //
          // the Dmn_DMesgHandler->write will add the name
          // of the Dmn_DMesgHandler from read it again,
          // but it is good to be explicit.

          if (dmesgpb_read.type() == dmn::DMesgTypePb::sys) {
            DMN_ASYNC_CALL_WITH_CAPTURE(
                { this->reconciliateDMesgPbSys(dmesgpb_read); }, this,
                dmesgpb_read);
          } else {
            DMN_ASYNC_CALL_WITH_CAPTURE(
                {
                  try {
                    this->m_subscript_handler->write(dmesgpb_read);

                    if (dmesgpb_read.type() != dmn::DMesgTypePb::sys) {
                      m_topic_last_dmesgpb[dmesgpb_read.topic()] = dmesgpb_read;
                    }
                  } catch (...) {
                    // The data from network is out of sync with data
                    // in the Dmn_DMesg, and a few should happen:
                    // - mark the topic as in conflict for local Dmn_DMesg
                    // - the local Dmn_DMesg will mark all openHandler in
                    //   conflict but waiting for resolution with
                    //   Dmn_DMesgNet master, so they will not allow any
                    //   message on the same topic band.
                    // - the local Dmn_DMesgNet will broadcast a sys
                    // conflict
                    //   message.
                    // - all networked DMesgNet(s) receives the sys conflict
                    //   message will then put its local Dmn_DMesg in
                    //   conflict state for the same topic.
                    // - master node will then send its last message for the
                    //   to all nodes, and all nodes receives the message
                    //   will use new message as its last valid message for
                    //   the topic and clear it conflict state.
                  }
                },
                this, dmesgpb_read);
          } /* else (dmesgpb_read.type() == dmn::DMesgTypePb::sys) */
        }
      }
    });

    m_input_proc->exec();

    m_sys_handler = Dmn_DMesg::openHandler(
        m_name + "_sys",
        [this]([[maybe_unused]] const dmn::DMesgPb &dmesgpb) { return false; },
        nullptr, Dmn_DMesg::kHandlerConfig_Default);
  }

  if (m_input_handler && m_output_handler) {
    // into MasterPending
    m_timer_proc = std::make_unique<dmn::Dmn_Timer<std::chrono::nanoseconds>>(
        std::chrono::nanoseconds(DMN_DMESGNET_HEARTBEAT_IN_NS), [this]() {
          this->write([this]() mutable {
            if (this->m_sys.body().sys().self().state() ==
                dmn::DMesgStatePb::MasterPending) {
              this->m_master_pending_counter++;

              if (this->m_master_pending_counter >=
                  DMN_DMESGNET_MASTERPENDING_MAX_COUNTER) {
                this->m_master_pending_counter = 0;

                auto *self =
                    this->m_sys.mutable_body()->mutable_sys()->mutable_self();
                DMESG_PB_SYS_NODE_SET_STATE(self, dmn::DMesgStatePb::Ready);
                DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, this->m_name);

                struct timeval tv;
                gettimeofday(&tv, NULL);

                DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP_FROM_TV(self, tv);
              }
            } else if (this->m_sys.body().sys().self().state() ==
                       dmn::DMesgStatePb::Ready) {
              if (this->m_sys.body().sys().self().masteridentifier() !=
                  this->m_sys.body().sys().self().identifier()) {
                this->m_master_sync_pending_counter++;

                if (this->m_master_sync_pending_counter >=
                    DMN_DMESGNET_MASTERSYNC_MAX_COUNTER) {
                  this->m_master_sync_pending_counter = 0;
                  this->m_last_remote_master_timestamp = {};

                  auto *self =
                      this->m_sys.mutable_body()->mutable_sys()->mutable_self();

                  DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, "");
                  DMESG_PB_SYS_NODE_SET_STATE(self,
                                              dmn::DMesgStatePb::MasterPending);
                }
              }
            }

            this->m_sys_handler->write(this->m_sys);

            bool master = this->m_sys.body().sys().self().masteridentifier() ==
                          this->m_sys.body().sys().self().identifier();

            // if self is a master, and it is becoming master or # of neighbor
            // increase, let resend prior last message per topic.
            // FIXME: maybe it is good that master resend them prioritically?
            if (m_output_handler && master &&
                ((master != m_is_master) ||
                 (m_number_of_neighbor !=
                  this->m_sys.body().sys().nodelist().size()))) {
              for (auto &topic_dmesgpb : m_topic_last_dmesgpb) {
                dmn::DMesgPb pb = topic_dmesgpb.second;

                DMESG_PB_SET_MSG_PLAYBACK(pb, true);
                DMESG_PB_SET_MSG_SOURCEWRITEHANDLERIDENTIFIER(pb, this->m_name);

                std::string serialized_string{};
                pb.SerializeToString(&serialized_string);
                m_output_handler->write(serialized_string);
              }
            }

            m_number_of_neighbor = this->m_sys.body().sys().nodelist().size();
            m_is_master = master;
          });
        });
  } else {
    auto *self = this->m_sys.mutable_body()->mutable_sys()->mutable_self();
    DMESG_PB_SYS_NODE_SET_STATE(self, dmn::DMesgStatePb::Ready);
    DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, this->m_name);
  }

  if (m_sys_handler) {
    m_sys_handler->write(this->m_sys);
  }
}

Dmn_DMesgNet::~Dmn_DMesgNet() noexcept try {
  // it is important that we free up m_input_handler as if it is a
  // kafka handler, it will be continuing to be feed of incoming
  // message and including one that is generated by this dmesgnet,
  // and that will prolong it closing.
  m_input_handler.reset();
  m_input_proc.reset();
  m_timer_proc.reset();

  if (m_output_handler) {
    // it is about to destroy the Dmn_DMesgNet and free everything
    // it will send last heartbeat and reliquinsh itself as master (if
    // itself is master).
    //
    // we avoid use of m_sys_handler as we are to destroy it, so we
    // do not want to hold the object life up and have to wait for
    // asynchrononous action to send last heartbeat messge.
    struct timeval tv;
    gettimeofday(&tv, NULL);

    DMESG_PB_SET_MSG_SOURCEIDENTIFIER(this->m_sys, this->m_name);
    DMESG_PB_SET_MSG_SOURCEWRITEHANDLERIDENTIFIER(this->m_sys, this->m_name);

    auto *self = this->m_sys.mutable_body()->mutable_sys()->mutable_self();

    DMESG_PB_SYS_NODE_SET_STATE(self, dmn::DMesgStatePb::Destroyed);
    DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, "");
    DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP_FROM_TV(self, tv);

    std::string serialized_string{};
    this->m_sys.SerializeToString(&serialized_string);

    m_output_handler->write(serialized_string);
  }

  if (m_sys_handler) {
    Dmn_DMesg::closeHandler(m_sys_handler);
  }

  if (m_subscript_handler) {
    Dmn_DMesg::closeHandler(m_subscript_handler);
  }

  this->waitForEmpty();
} catch (...) {
  // explicit return to resolve exception as destructor must be noexcept
  return;
}

void Dmn_DMesgNet::reconciliateDMesgPbSys(dmn::DMesgPb dmesgpb_other) {
  auto other = dmesgpb_other.body().sys().self();
  auto self = this->m_sys.mutable_body()->mutable_sys()->mutable_self();

  struct timeval tv;
  gettimeofday(&tv, NULL);

  if (self->state() == dmn::DMesgStatePb::MasterPending &&
      other.state() == dmn::DMesgStatePb::Ready) {
    assert(self->masteridentifier() == "");
    assert(other.masteridentifier() != "");

    DMESG_PB_SYS_NODE_SET_STATE(self, dmn::DMesgStatePb::Ready);
    DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, other.masteridentifier());

    DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP_FROM_TV(self, tv);

    this->m_last_remote_master_timestamp = tv;
    this->m_master_pending_counter = 0;
    this->m_master_sync_pending_counter = 0;
    this->m_sys_handler->write(this->m_sys);
  } else if (self->state() == dmn::DMesgStatePb::Ready) {
    assert("" != self->masteridentifier());
    assert(0 == this->m_master_pending_counter);

    if (other.identifier() == self->masteridentifier()) {
      if (other.state() == dmn::DMesgStatePb::Ready) {
        this->m_master_sync_pending_counter = 0;
        this->m_last_remote_master_timestamp = tv;
      } else {
        /* other node relinquish its self-proclaim master state
         * so local node also reset the master state
         */
        assert(other.masteridentifier() == "");

        DMESG_PB_SYS_NODE_SET_STATE(self, dmn::DMesgStatePb::MasterPending);
        DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, "");

        DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP_FROM_TV(self, tv);

        this->m_last_remote_master_timestamp = {};
        this->m_master_pending_counter = 0;
        this->m_master_sync_pending_counter = 0;
        this->m_sys_handler->write(this->m_sys);
      }
    } else if (other.state() == dmn::DMesgStatePb::Ready &&
               other.masteridentifier() != self->masteridentifier()) {
      assert("" != self->masteridentifier());
      assert("" != other.masteridentifier());

      if (other.initializedtimestamp().seconds() <
              self->initializedtimestamp().seconds() ||
          (other.initializedtimestamp().seconds() ==
               self->initializedtimestamp().seconds() &&
           other.initializedtimestamp().nanos() <
               self->initializedtimestamp().nanos())) {
        // follow other node's masteridentifier() as other node has higher
        // seniority than local node.

        DMESG_PB_SYS_NODE_SET_MASTERIDENTIFIER(self, other.masteridentifier());
        DMESG_PB_SYS_NODE_SET_UPDATEDTIMESTAMP_FROM_TV(self, tv);

        this->m_last_remote_master_timestamp = tv;
        this->m_master_pending_counter = 0;
        this->m_master_sync_pending_counter = 0;
        this->m_sys_handler->write(this->m_sys);
      }
    }
  } // if (self->state() == dmn::DMesgStatePb::Ready)

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

  if (other.state() == dmn::DMesgStatePb::Destroyed) {
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

    DMESG_PB_SYS_SET_NODELIST_ELEM_IDENTIFIER(this->m_sys, i,
                                              other.identifier());
    DMESG_PB_SYS_SET_NODELIST_ELEM_MASTERIDENTIFIER(this->m_sys, i,
                                                    other.masteridentifier());
    DMESG_PB_SYS_SET_NODELIST_ELEM_STATE(this->m_sys, i, other.state());
    DMESG_PB_SYS_SET_NODELIST_ELEM_INITIALIZEDTIMESTAMP(
        this->m_sys, i, other.initializedtimestamp());
    DMESG_PB_SYS_SET_NODELIST_ELEM_UPDATEDTIMESTAMP(this->m_sys, i,
                                                    other.updatedtimestamp());
  }
} // method reconciliateDmesgPbSys

} // namespace dmn
