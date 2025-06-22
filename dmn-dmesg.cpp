/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * @file dmn-dmesg.cpp
 * @brief The source implementation file for dmn-dmesg.
 */

#include "dmn-dmesg.hpp"

#include <sys/time.h>

#include <algorithm>
#include <atomic>
#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "dmn-debug.hpp"
#include "dmn-dmesg-pb-util.hpp"
#include "dmn-pub-sub.hpp"
#include "dmn-util.hpp"

#include "proto/dmn-dmesg.pb.h"

namespace dmn {

const char *kDMesgSysIdentifier = "sys.dmn-dmesg";

const Dmn_DMesg::HandlerConfig Dmn_DMesg::kHandlerConfig_Default = {};
const std::string Dmn_DMesg::kHandlerConfig_IncludeSys = "Handler_IncludeSys";
const std::string Dmn_DMesg::kHandlerConfig_NoTopicFilter =
    "Handler_NoTopicFilter";

// class Dmn_DMesg::Dmn_DMesgHandler::Dmn_DMesgHandlerSub
Dmn_DMesg::Dmn_DMesgHandler::Dmn_DMesgHandlerSub::
    ~Dmn_DMesgHandlerSub() noexcept try {

} catch (...) {
  // explicit return to resolve exception as destructor must be noexcept
  return;
}

void Dmn_DMesg::Dmn_DMesgHandler::Dmn_DMesgHandlerSub::notify(
    const dmn::DMesgPb &dmesgpb) {
  if (dmesgpb.sourcewritehandleridentifier() != m_owner->m_name ||
      dmesgpb.type() == dmn::DMesgTypePb::sys) {
    std::string id = dmesgpb.topic();
    unsigned long runningCounter = m_owner->m_topic_running_counter[id];

    if (dmesgpb.runningcounter() > runningCounter) {
      m_owner->m_topic_running_counter[id] = dmesgpb.runningcounter();

      if (dmesgpb.type() == dmn::DMesgTypePb::sys) {
        m_owner->m_last_dmesgpb_sys = dmesgpb;
      }

      if ((dmn::DMesgTypePb::sys != dmesgpb.type() ||
           m_owner->m_include_dmesgpb_sys) &&
          (m_owner->m_no_topic_filter || dmesgpb.topic() == m_owner->m_topic) &&
          (!m_owner->m_filter_fn || m_owner->m_filter_fn(dmesgpb))) {
        if (m_owner->m_async_process_fn) {
          m_owner->m_async_process_fn(std::move_if_noexcept(dmesgpb));
        } else {
          // FIXME
          dmn::DMesgPb copied = dmesgpb;
          m_owner->m_buffers.push(copied);
        }
      }
    }
  }
}

// class Dmn_DMesg::Dmn_DMesgHandler
Dmn_DMesg::Dmn_DMesgHandler::Dmn_DMesgHandler(std::string_view name,
                                              std::string_view topic,
                                              FilterTask filter_fn,
                                              AsyncProcessTask async_process_fn,
                                              HandlerConfig configs)
    : m_name{name}, m_topic{topic}, m_filter_fn{filter_fn},
      m_async_process_fn{async_process_fn}, m_configs{configs} {
  // set the chained of owner for composite Dmn_DMesgHandlerSub object
  auto it = m_configs.find(kHandlerConfig_IncludeSys);
  if (m_configs.end() != it) {
    m_include_dmesgpb_sys =
        stringCompare(it->second, "1") || stringCompare(it->second, "yes");
  }

  it = m_configs.find(kHandlerConfig_NoTopicFilter);
  if (m_configs.end() != it) {
    m_no_topic_filter =
        stringCompare(it->second, "1") || stringCompare(it->second, "yes");
  }

  m_sub.m_owner = this;
}

Dmn_DMesg::Dmn_DMesgHandler::Dmn_DMesgHandler(std::string_view name,
                                              std::string_view topic,
                                              FilterTask filter_fn,
                                              AsyncProcessTask async_process_fn)
    : Dmn_DMesgHandler{name, topic, filter_fn, async_process_fn,
                       kHandlerConfig_Default} {}

Dmn_DMesg::Dmn_DMesgHandler::Dmn_DMesgHandler(std::string_view name,
                                              std::string_view topic,
                                              FilterTask filter_fn)
    : Dmn_DMesgHandler{name, topic, filter_fn, nullptr} {}

Dmn_DMesg::Dmn_DMesgHandler::Dmn_DMesgHandler(std::string_view name,
                                              std::string_view topic)
    : Dmn_DMesgHandler{name, topic, nullptr} {}

Dmn_DMesg::Dmn_DMesgHandler::Dmn_DMesgHandler(std::string_view name,
                                              FilterTask filter_fn,
                                              AsyncProcessTask async_process_fn,
                                              HandlerConfig configs)
    : Dmn_DMesgHandler{name, "", filter_fn, async_process_fn, configs} {}

Dmn_DMesg::Dmn_DMesgHandler::Dmn_DMesgHandler(std::string_view name,
                                              FilterTask filter_fn,
                                              AsyncProcessTask async_process_fn)
    : Dmn_DMesgHandler{name, filter_fn, async_process_fn,
                       kHandlerConfig_Default} {}

Dmn_DMesg::Dmn_DMesgHandler::Dmn_DMesgHandler(std::string_view name,
                                              FilterTask filter_fn)
    : Dmn_DMesgHandler{name, filter_fn, (AsyncProcessTask) nullptr} {}

Dmn_DMesg::Dmn_DMesgHandler::Dmn_DMesgHandler(std::string_view name)
    : Dmn_DMesgHandler{name, (FilterTask) nullptr} {}

Dmn_DMesg::Dmn_DMesgHandler::~Dmn_DMesgHandler() noexcept try {
  m_sub.waitForEmpty();
} catch (...) {
  // explicit return to resolve exception as destructor must be noexcept
  return;
}

std::optional<dmn::DMesgPb> Dmn_DMesg::Dmn_DMesgHandler::read() {
  if (nullptr != m_owner) {
    try {
      dmn::DMesgPb mesgPb = m_buffers.pop();

      return mesgPb;
    } catch (...) {
      // do nothing
    }
  }

  return {};
}

void Dmn_DMesg::Dmn_DMesgHandler::resolveConflict() {
  m_owner->resetHandlerConflictState(this);
}

void Dmn_DMesg::Dmn_DMesgHandler::setConflictCallbackTask(
    ConflictCallbackTask conflict_fn) {
  m_conflict_callback_fn = conflict_fn;
}

void Dmn_DMesg::Dmn_DMesgHandler::write(dmn::DMesgPb &&dmesgpb) {
  if (nullptr == m_owner) {
    return;
  }

  dmn::DMesgPb moved_dmesgpb = std::move_if_noexcept(dmesgpb);

  auto waitHandler = m_sub.addExecTaskWithWait(
      [this, &moved_dmesgpb] { writeDMesgInternal(moved_dmesgpb, true); });
  waitHandler->wait();
}

void Dmn_DMesg::Dmn_DMesgHandler::write(dmn::DMesgPb &dmesgpb) {
  if (nullptr == m_owner) {
    return;
  }

  auto waitHandler = m_sub.addExecTaskWithWait(
      [this, &dmesgpb] { writeDMesgInternal(dmesgpb, false); });
  waitHandler->wait();
}

void Dmn_DMesg::Dmn_DMesgHandler::waitForEmpty() { m_sub.waitForEmpty(); }

void Dmn_DMesg::Dmn_DMesgHandler::writeDMesgInternal(dmn::DMesgPb &dmesgpb,
                                                     bool move) {
  assert(nullptr != m_owner);

  if (m_in_conflict) {
    throw std::runtime_error("last write results in conflicted, "
                             "handler needs to be reset");
  }

  struct timeval tv;
  gettimeofday(&tv, NULL);

  DMESG_PB_SET_MSG_TIMESTAMP_FROM_TV(dmesgpb, tv);
  DMESG_PB_SET_MSG_SOURCEWRITEHANDLERIDENTIFIER(dmesgpb, m_name);

  if (m_no_topic_filter) {
  } else {
    DMESG_PB_SET_MSG_TOPIC(dmesgpb, m_topic);
  }

  if ("" == dmesgpb.sourceidentifier()) {
    DMESG_PB_SET_MSG_SOURCEIDENTIFIER(dmesgpb, m_name);
  }

  std::string topic = dmesgpb.topic();

  unsigned long next_running_counter =
      incrementByOne(m_topic_running_counter[topic]);

  DMESG_PB_SET_MSG_RUNNINGCOUNTER(dmesgpb, next_running_counter);

  if (move) {
    m_owner->publish(std::move_if_noexcept(dmesgpb));
  } else {
    m_owner->publish(dmesgpb);
  }

  m_topic_running_counter[topic] = next_running_counter;
}

bool Dmn_DMesg::Dmn_DMesgHandler::isInConflict() const { return m_in_conflict; }

void Dmn_DMesg::Dmn_DMesgHandler::resolveConflictInternal() {
  m_in_conflict = false;
}

void Dmn_DMesg::Dmn_DMesgHandler::throwConflict(const dmn::DMesgPb dmesgpb) {
  m_in_conflict = true;

  if (m_conflict_callback_fn) {
    m_sub.write(
        [this, dmesgpb]() { this->m_conflict_callback_fn(*this, dmesgpb); });
  }
}

// class Dmn_DMesg
Dmn_DMesg::Dmn_DMesg(std::string_view name)
    : Dmn_Pub{name, 0, // Dmn_DMesg manages re-send per topic
              [](const Dmn_Sub *const sub, const dmn::DMesgPb &msg) {
                const Dmn_DMesgHandler::Dmn_DMesgHandlerSub *const handler_sub =
                    dynamic_cast<
                        const Dmn_DMesgHandler::Dmn_DMesgHandlerSub *const>(
                        sub);
                assert(handler_sub != nullptr);

                Dmn_DMesgHandler *handler = handler_sub->m_owner;

                return nullptr != handler && nullptr != handler->m_owner &&
                       (true == msg.playback() ||
                        handler->m_after_initial_playback) &&
                       (handler->m_no_topic_filter ||
                        msg.topic() == handler->m_topic);
              }},
      m_name{name} {}

Dmn_DMesg::~Dmn_DMesg() noexcept try { this->waitForEmpty(); } catch (...) {
  // explicit return to resolve exception as destructor must be noexcept
  return;
}

void Dmn_DMesg::closeHandler(
    std::shared_ptr<Dmn_DMesg::Dmn_DMesgHandler> &handler) {
  this->unregisterSubscriber(&(handler->m_sub));
  handler->waitForEmpty();
  handler->m_owner = nullptr;

  Dmn_DMesgHandler *handler_ptr = handler.get();
  handler = {};

  DMN_ASYNC_CALL_WITH_CAPTURE(
      {
        std::vector<std::shared_ptr<Dmn_DMesgHandler>>::iterator it =
            std::find_if(m_handlers.begin(), m_handlers.end(),
                         [handler_ptr](auto handler) {
                           return handler.get() == handler_ptr;
                         });

        if (it != m_handlers.end()) {
          m_handlers.erase(it);
        }
      },
      this, handler_ptr);
}

void Dmn_DMesg::playbackLastTopicDMesgPbInternal() {
  for (auto &topic_dmesgpb : m_topic_last_dmesgpb) {
    dmn::DMesgPb pb = topic_dmesgpb.second;

    DMESG_PB_SET_MSG_PLAYBACK(pb, true);

    this->publishInternal(pb);
  }
}

void Dmn_DMesg::publishInternal(const dmn::DMesgPb &dmesgpb) {
  // if it is a playback, we do not check if it is in conflict.
  if (dmesgpb.playback()) {
    Dmn_Pub::publishInternal(dmesgpb);

    return;
  }

  std::vector<std::shared_ptr<Dmn_DMesgHandler>>::iterator it = std::find_if(
      m_handlers.begin(), m_handlers.end(), [&dmesgpb](auto handler) {
        return handler->m_name == dmesgpb.sourcewritehandleridentifier();
      });

  // if source is still in conflict, we do not allow it to send any message
  // until it resolves conflict state.
  if (it != m_handlers.end() && (*it)->isInConflict()) {
    // avoid throw conflict multiple times
    return;
  }

  std::string id = dmesgpb.topic();

  unsigned long next_running_counter =
      incrementByOne(m_topic_running_counter[id]);

  // if this is a message is out of date and put the sender in conflict
  if (dmesgpb.runningcounter() < next_running_counter) {
    if (it != m_handlers.end()) {
      (*it)->throwConflict(dmesgpb);
    }

    return;
  }

  dmn::DMesgPb copied = dmesgpb;

  DMESG_PB_SET_MSG_RUNNINGCOUNTER(copied, next_running_counter);
  Dmn_Pub::publishInternal(copied);
  m_topic_running_counter[id] = next_running_counter;
  m_topic_last_dmesgpb[id] = copied;
}

void Dmn_DMesg::publishSysInternal(const dmn::DMesgPb &dmesgpb_sys) {
  assert(dmesgpb_sys.topic() == kDMesgSysIdentifier);
  assert(dmesgpb_sys.type() == dmn::DMesgTypePb::sys);

  std::string id = dmesgpb_sys.topic();
  unsigned long next_running_counter =
      incrementByOne(m_topic_running_counter[id]);

  dmn::DMesgPb copied = dmesgpb_sys;

  DMESG_PB_SET_MSG_RUNNINGCOUNTER(copied, next_running_counter);
  Dmn_Pub::publishInternal(copied);
  m_topic_running_counter[id] = next_running_counter;
}

void Dmn_DMesg::resetHandlerConflictState(const Dmn_DMesgHandler *handler_ptr) {
  DMN_ASYNC_CALL_WITH_CAPTURE(
      { this->resetHandlerConflictStateInternal(handler_ptr); }, this,
      handler_ptr);
}

void Dmn_DMesg::resetHandlerConflictStateInternal(
    const Dmn_DMesgHandler *handler_ptr) {
  std::vector<std::shared_ptr<Dmn_DMesgHandler>>::iterator it = std::find_if(
      m_handlers.begin(), m_handlers.end(),
      [handler_ptr](auto handler) { return handler.get() == handler_ptr; });

  if (it != m_handlers.end()) {
    (*it)->resolveConflictInternal();
  }
}

} // namespace dmn
