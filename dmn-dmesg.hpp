/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * @file dmn-dmesg.hpp
 * @brief The header file for dmn-dmesg which implements a specific publisher
 *        subscriber model (inherit from dmn-pub-sub module) where the
 *        difference is that:
 *
 *        - the data item is a Protobuf message (dmn::DMesgPb) defined in
 *          proto/dmn-dmesg.proto, so instead of subclassing the Dmn_DMesg,
 *          clients can extend the dmn::DMesgPb protobuf message to varying
 *          the message without reinvent the wheel through subclass of the
 *          dmn-pub-sub module.
 *        - instead of subclass Dmn_Pub::Dmn_Sub class to implement specific
 *          subscriber, the client of the API asks Dmn_DMesg to return a handler
 *          that subscribes to a specific set of topic (optional, or all
 *          topics), and can use to handler to publish and subscribe
 *          dmn::DMesgPb message through the Dmn_Io like interface' read and
 *          write API methods, so instead of inherittence from Dmn_DMesg,
 *          clients can use object composition with it.
 *        - it supports the concept that subscriber can subscribe to certain
 *          topic as defined in the dmn::DMesgPb message.
 *        - it allows various clients of the Dmn_Dmesg to publish data of
 *          certain topic at the same time, and implements a simple conflict
 *          detection and resolution for participated clients of the same
 *          Dmn_Dmesg object, Dmn_DMesg object takes care of synchronizing
 *          message between publisher and all subscribers of the same Dmn_DMesg
 *          object.
 */

#ifndef DMN_DMESG_HPP_

#define DMN_DMESG_HPP_

#include <sys/time.h>

#include <atomic>
#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "dmn-pub-sub.hpp"

#include "proto/dmn-dmesg.pb.h"

namespace dmn {

extern const char *kDMesgSysIdentifier;

class Dmn_DMesg : public Dmn_Pub<dmn::DMesgPb> {
public:
  using FilterTask = std::function<bool(const dmn::DMesgPb &)>;
  using AsyncProcessTask = std::function<void(dmn::DMesgPb)>;
  using HandlerConfig = std::unordered_map<std::string, std::string>;

  /**
   * @brief Default handler configuration values where
   *        kHandlerConfig_IncludeSys is set to "no"
   *        kHandlerConfig_NoTopicFilter is set to "no"
   */
  static const HandlerConfig kHandlerConfig_Default;

  /**
   * @brief If the config value is "yes" or "1", then the openHandler will
   *        also include dmesgpb which message type is "sys".
   */
  static const std::string kHandlerConfig_IncludeSys;

  /**
   * @brief If the config value is "yes" or "1", then the openHandler will not
   *        set message topic upon sending write dmesgpb and will return
   *        dmesgpb regardless of topic value.
   */
  static const std::string kHandlerConfig_NoTopicFilter;

  /**
   * @brief The key (std::string) and value (std::string) for Dmn_DMesg
   *        configuration.
   */
  using KeyValueConfiguration = std::unordered_map<std::string, std::string>;

  /**
   * @brief The Dmn_DMesgHandler is intentionalled modelled to inherit from
   *        Dmn_Io that provides read/write IO interface across a range of
   *        IO, like socket, file, kafka, etc, and the Dmn_DMesgHandler is
   *        just another IO, because of that we do NOT able to inherit from
   *        Dmn_Pub as Dmn_Pub inherits Dmn_Async which inherits from Dmn_Pipe
   *        which inherit Dmn_Io with template type specialized to functor,
   *        this is a diamond shape multiple inheritance where common parent
   *        has to have same instantiated template type. Instead the class
   *        Dmn_DMesgHandler uses a wrapper class Dmn_DMesgHandlerSub which
   *        inherits from Dmn_Pub<dmn::DMesgPb>::Dmn_Sub.
   */
  class Dmn_DMesgHandler : public Dmn_Io<dmn::DMesgPb> {
  private:
    using ConflictCallbackTask =
        std::function<void(Dmn_DMesgHandler &handler, const dmn::DMesgPb &)>;

    class Dmn_DMesgHandlerSub : public dmn::Dmn_Pub<dmn::DMesgPb>::Dmn_Sub {
    public:
      Dmn_DMesgHandlerSub() = default;
      virtual ~Dmn_DMesgHandlerSub() noexcept;

      Dmn_DMesgHandlerSub(const Dmn_DMesgHandlerSub &obj) = delete;
      const Dmn_DMesgHandlerSub &
      operator=(const Dmn_DMesgHandlerSub &obj) = delete;
      Dmn_DMesgHandlerSub(Dmn_DMesgHandlerSub &&obj) = delete;
      Dmn_DMesgHandlerSub &operator=(Dmn_DMesgHandlerSub &&obj) = delete;

      /**
       * @brief The method is called by the dmn::Pub (publisher) object to
       *        notify the Dmn_DMesgHandlerSub about the new DMesgPB message.
       *
       *        The Dmn_DMesgHandlerSub will follow the following to process
       *        the message:
       *        - skip messages sent through the same Dmn_DMesgHandler
       *          handler, unless it is a system message (of DMesgPb).
       *        - skip message which running counter is lower than running
       *          counter of the topic the Dmn_DMesgHandlerSub has received.
       *        - store a copy of the message as last system message if it is.
       *        - if it is system message and the Dmn_DMesgHandler (owner of
       *          the Dmn_DMesgHandlerSub) is opened with
       *          m_include_dmesgpb_sys as true, the Dmn_DMesgPB message will
       *          be either pushed into the buffer waiting to be read through
       *          Dmn_DMesgHandler' read or handling through
       *          m_async_process_fn callback.
       *
       * @param dmesgPb The DMesgPb message notified by publisher object
       */
      void notify(const dmn::DMesgPb &dmesgpb) override;

      // WARNING: it is marked as public so that a closure function
      // to Dmn_DMesg can access and manipulate it, as there is no
      // direct way to declare an inline closure as friend (we can
      // define a function that accept object of type Dmn_DMesgHandlerSub
      // class and then pass the object as capture to a closure
      // created within the function and returns the closure to
      // DMesg, but all the trouble to access this.
      //
      // But no external client will access the nested Dmn_DMesgHandlerSub
      // class and it is just a composite object within the Dmn_DMesgHandler
      // class to integrate with dmn-pub-sub through Dmn_Pub::Dmn_sub, so
      // marking m_owner as public does not violate data encapsulation.

      Dmn_DMesgHandler *m_owner{};
    }; // class Dmn_DMesgHandlerSub

  public:
    /**
     * @brief The primitive constructor for Dmn_DMesgHandler that reads or
     *        writes a specific topic ("" which is a valid topic).
     *
     * @param name             The name or unique identification to the
     *                         handler
     * @param topic            The topic (DMesgPb.topic) that the handler
     *                         subscribes to for reading or set as topic for
     *                         written message
     * @param filter_fn        The functor callback that returns false to
     *                         filter out DMesgPB message, if no functor is
     *                         provided, no filter is performed
     * @param async_process_fn The functor callback to process each notified
     *                         DMesgPb message
     * @param config           The Handler configuration values
     */
    Dmn_DMesgHandler(std::string_view name, std::string_view topic,
                     FilterTask filter_fn, AsyncProcessTask async_process_fn,
                     HandlerConfig configs);

    /**
     * @brief The primitive constructor for Dmn_DMesgHandler that reads or
     *        writes a specific topic ("" which is a valid topic), with default
     *        handler configuration values.
     *
     * @param name             The name or unique identification to the
     *                         handler
     * @param topic            The topic (DMesgPb.topic) that the handler
     *                         subscribes to for reading or set as topic for
     *                         written message
     * @param filter_fn        The functor callback that returns false to
     *                         filter out DMesgPB message, if no functor is
     *                         provided, no filter is performed
     * @param async_process_fn The functor callback to process each notified
     */
    Dmn_DMesgHandler(std::string_view name, std::string_view topic,
                     FilterTask filter_fn, AsyncProcessTask async_process_fn);

    /**
     * @brief The primitive constructor for Dmn_DMesgHandler that reads or
     *        writes a specific topic (include "" which is a valid topic),
     *        without async_process_fn functor, and with default handler
     *        configuration values.
     *
     * @param name      The name or unique identification to the handler
     * @param topic     The topic (DMesgPb.topic) that the handler subscribes
     *                  to for reading or set as topic for written message
     * @param filter_fn The functor callback that returns false to filter out
     *                  DMesgPB message, if no functor is provided, no filter
     *                  is performed
     */
    Dmn_DMesgHandler(std::string_view name, std::string_view topic,
                     FilterTask filter_fn);

    /**
     * @brief The primitive constructor for Dmn_DMesgHandler that reads or
     *        writes a specific topic (include "" which is a valid topic),
     *        without filter_fn functor, without async_process_fn functor,
     *        and with default handler configuration values.
     *
     * @param name  The name or unique identification to the handler
     * @param topic The topic (DMesgPb.topic) that the handler subscribes
     *              to for reading or set as topic for written message
     */
    Dmn_DMesgHandler(std::string_view name, std::string_view topic);

    /**
     * @brief The primitive constructor for Dmn_DMesgHandler that reads or
     *        writes message with topic set to "" with filter_fn and
     *        async_process_fn functors and the customized handler
     *        configuration values.
     *
     * @param name             The name or unique identification to the
     *                         handler
     * @param filter_fn        The functor callback that returns false to
     *                         filter out DMesgPB message, if no functor is
     *                         provided, no filter is performed
     * @param async_process_fn The functor callback to process each notified
     *                         DMesgPb message
     * @param config           The handler configuration values
     */
    Dmn_DMesgHandler(std::string_view name, FilterTask filter_fn,
                     AsyncProcessTask async_process_fn, HandlerConfig configs);

    /**
     * @brief The primitive constructor for Dmn_DMesgHandler that reads or
     *        writes message with topic set to "" with a filter_fn and
     *        async_process_fn functors and the default open handler
     *        configuration values.
     *
     * @param name             The name or unique identification to the
     *                         handler
     * @param filter_fn        The functor callback that returns false to
     *                         filter out DMesgPB message, if no functor is
     *                         provided, no filter is performed
     * @param async_process_fn The functor callback to process each notified
     *                         DMesgPb message
     */
    Dmn_DMesgHandler(std::string_view name, FilterTask filter_fn,
                     AsyncProcessTask async_process_fn);

    /**
     * @brief The primitive constructor for Dmn_DMesgHandler that reads or
     *        writes message with topic set to "" with a filter_fn functor,
     *        but no async_process_fn functor, and the default open handler
     *        configuration values.
     *
     * @param name      The name or unique identification to the handler
     * @param filter_fn The functor callback that returns false to filter out
     *                  DMesgPB message, if no functor is provided, no filter
     *                  is performed
     */
    Dmn_DMesgHandler(std::string_view name, FilterTask filter_fn);

    /**
     * @brief The primitive constructor for Dmn_DMesgHandler that reads or
     *        writes message with topic set to "", no filter_fn functor, no
     *        async_process_fn functor, and the default handler open
     *        configuration values.
     *
     * @param name The name or unique identification to the handler
     */
    Dmn_DMesgHandler(std::string_view name);

    virtual ~Dmn_DMesgHandler() noexcept;

    Dmn_DMesgHandler(const Dmn_DMesgHandler &obj) = delete;
    const Dmn_DMesgHandler &operator=(const Dmn_DMesgHandler &obj) = delete;
    Dmn_DMesgHandler(Dmn_DMesgHandler &&obj) = delete;
    Dmn_DMesgHandler &operator=(Dmn_DMesgHandler &&obj) = delete;

    /**
     * @brief The method reads a DMesgPb message out of the handler
     *        opened with DMesg. This is a blocking call until a DMesgPb
     *        message is available or exception is thrown, then nullopt
     *        is returned.
     *
     * @return DMesgPb The next DMesgPb message or nullopt if exception
     *                 is thrown.
     */
    std::optional<dmn::DMesgPb> read() override;

    /**
     * @brief The method marks the handler as conflict resolved by posting an
     *        asynchronous action on publisher singleton asynchronous thread
     *        context to reset the handler' context state.
     */
    void resolveConflict();

    /**
     * @brief The method set the callback function for conflict.
     *
     * @param conflict_fn The conflict callback function
     */
    void setConflictCallbackTask(ConflictCallbackTask conflict_fn);

    /**
     * @brief The method writes and publishes the DMesgPb message through
     *        DMesg publisher queue to all subscribers. This method will move
     *        the DMesg data.
     *
     * @param dMesgPb The DMesgPb message to be published
     */
    void write(dmn::DMesgPb &&dmesgpb) override;

    /**
     * @brief The method writes and publishes the DMesgPb message through
     *        DMesg publisher queue to all subscribers. This method will copy
     *        the DMesg data.
     *
     * @param dMesgPb The DMesgPb message to be published
     */
    void write(dmn::DMesgPb &dmesgpb) override;

    /**
     * @brief The method returns true if the handler has NO pending to be
     *         executed asynchronous jobs (example is notify methods).
     */
    void waitForEmpty();

    friend class Dmn_DMesg;
    friend class Dmn_DMesgHandlerSub;

  protected:
    /**
     * @brief The method writes and publishes the DMesgPb message through
     *        DMesg publisher queue to all subscribers. This method will
     *        move the DMesgPb message if move argument is true, otherwise
     *        copy the message.
     *
     * @param dmesgPb The DMesgPb messsgeto be published
     * @param move    True to move than copy the data
     */
    void writeDMesgInternal(dmn::DMesgPb &dmesgpb, bool move);

  private:
    /**
     * @brief The method returns true if the handler is in conflict state.
     *
     * @return True if the handler is in conflict state, false otherwise
     */
    bool isInConflict() const;

    /**
     * @brief The method marks the handler as conflict resolved, and to be
     *        executed in the publisher's singleton asynchronous thread
     *        context (to avoid the need of additional mutex).
     */
    void resolveConflictInternal();

    /**
     * @brief The method marks the handler as in conflict state and executes
     *        the conflict callback function in the handler singleton
     *        asynchronous thread context.
     *
     * @param mesgPb The dmesgPb data that results in conflict state
     */
    void throwConflict(const dmn::DMesgPb dmesgpb);

    /**
     * data member for constructor to instantiate the object.
     */
    std::string m_name{};
    std::string m_topic{};
    FilterTask m_filter_fn{};
    AsyncProcessTask m_async_process_fn{};
    HandlerConfig m_configs{};

    /**
     * data members for internal logic
     */
    bool m_include_dmesgpb_sys{};
    bool m_no_topic_filter{};

    Dmn_DMesg *m_owner{};
    Dmn_DMesgHandlerSub m_sub{};

    Dmn_Buffer<dmn::DMesgPb> m_buffers{};
    dmn::DMesgPb m_last_dmesgpb_sys{};
    std::unordered_map<std::string, unsigned long> m_topic_running_counter{};

    ConflictCallbackTask m_conflict_callback_fn{};
    std::atomic<bool> m_in_conflict{};

    bool m_after_initial_playback{};
  }; // class Dmn_DMesgHandler

  /**
   * @brief The constructor for Dmn_DMesg.
   *
   * @param name   The identification name for the instantiated object
   */
  Dmn_DMesg(std::string_view name);
  virtual ~Dmn_DMesg() noexcept;

  Dmn_DMesg(const Dmn_DMesg &obj) = delete;
  const Dmn_DMesg &operator=(const Dmn_DMesg &obj) = delete;
  Dmn_DMesg(Dmn_DMesg &&obj) = delete;
  Dmn_DMesg &operator=(Dmn_DMesg &&obj) = delete;

  /**
   * @brief The method creates a new Dmn_DMesgHandler, registers the handler
   *        to receive published message and returns the handler to the caller.
   *        It takes forward arguments as in Dmn_DMesgHandler::openHandler(...).
   *
   * @param name             The name or unique identification to the
   *                         handler
   * @param topic            The topic (DMesgPb.topic) that the handler
   *                         subscribes to for reading or set as topic for
   *                         written message
   * @param filter_fn        The functor callback that returns false to
   *                         filter out DMesgPB message, if no functor is
   *                         provided, no filter is performed
   * @param async_process_fn The functor callback to process each notified
   *                         DMesgPb message
   * @param config           The Handler configuration values
   *
   * @return newly created Dmn_DMesgHandler
   */
  template <class... U>
  std::shared_ptr<Dmn_DMesgHandler> openHandler(U &&...arg);

  /**
   * @brief The method unregisters and removes the handler from the DMesg and
   *        then free the handler.
   *
   * @param handlerToClose The handler to be closed
   */
  void closeHandler(std::shared_ptr<Dmn_DMesgHandler> &handlerToClose);

protected:
  using Dmn_Pub::publish;

  /**
   * @brief The method publishes system message through async context queue.
   *
   * @param dmesgSysPb The system DMesgPb message
   */
  void publishSysInternal(const dmn::DMesgPb &dmesgSysPb);

  /**
   * @brief The method publishes dmesgPb to registered subscribers. If the to
   *        be published dmesgPb' topic has smaller runningcounter than what is
   *        in the m_topic_running_counter, it means that the writer is out of
   *        sync and in race condition that its published dmn::DMesgPb' topic
   *        has a runningcounter that is early in value than the same topic's
   *        running counter published by the DMesg. In this case, we put the
   *        writer handler to be in conflict state, and throws exception for
   *        future write (of the writer) until the client of the handler
   *        manually marks it as conflict resolved. We do not put all handlers
   *        in conflict, but the particular writer' handler.
   *
   * @param dmesgPb The dmesgPb to be published
   */
  void publishInternal(const dmn::DMesgPb &dmesgPb) override;

  /**
   * @brief The method posts an asynchronous action in the publisher's
   *        singleton asynchronous thread context to reset the handler's
   *        conflict state.
   *
   * @param handlerName the identification string for the open handler
   */
  void resetHandlerConflictState(const Dmn_DMesgHandler *handlerPtr);

private:
  /**
   * @brief The internal method is to be called in the publisher's singleton
   *        asynchronous thread context to playback last message of each
   *        topic.
   */
  void playbackLastTopicDMesgPbInternal();

  /**
   * @brief The method resets the handler conflict state, it must be called
   *        within the publisher' singleton asynchronous thread context to be
   *        thread safe and that is the same thread that puts the handler in
   *        conflict state.
   *
   * @param handlerName the identification string for the open handler
   */
  void resetHandlerConflictStateInternal(const Dmn_DMesgHandler *handlerPtr);

  /**
   * data members for constructor to instantiate the object.
   */
  std::string m_name{};

  /**
   * data members for internal logic.
   */
  std::vector<std::shared_ptr<Dmn_DMesgHandler>> m_handlers{};
  std::unordered_map<std::string, unsigned long> m_topic_running_counter{};
  std::unordered_map<std::string, dmn::DMesgPb> m_topic_last_dmesgpb{};
}; // class Dmn_DMesg

template <class... U>
std::shared_ptr<Dmn_DMesg::Dmn_DMesgHandler>
Dmn_DMesg::openHandler(U &&...arg) {
  // this is primitive openHandler() method that will
  // - create the handler
  // - register the handler as subscriber
  // - chain up the different level of objects to its owner, handler's
  //   subscriber to handler, handler to the DMesg publisher
  // - then add an asynchronous task to run in the publisher singleton
  //   asynchronous thread context and the task will add the handler to the
  //   list of handlers known to the DMesg subscriber, playback prior data per
  //   topic, and set flag that the newly created handler has been fully
  //   initialized after playback of prior data.
  //
  // This allows us to maintain a singleton asynchronous thread context is
  // responsible for publishing and notifying data between subscriber and
  // publisher.

  std::shared_ptr<Dmn_DMesg::Dmn_DMesgHandler> handler =
      std::make_shared<Dmn_DMesg::Dmn_DMesgHandler>(std::forward<U>(arg)...);

  /* The topic filter is executed within the DMesg singleton asynchronous
   * thread context, but the filter value is maintained per Dmn_DMesgHandler,
   * and this allow the DMesg to be mutex free while thread safe.
   */
  handler->m_owner = this;

  this->registerSubscriber(&(handler->m_sub));

  auto handler_ret = handler;

  DMN_ASYNC_CALL_WITH_CAPTURE(
      {
        this->m_handlers.push_back(handler);
        this->playbackLastTopicDMesgPbInternal();
        handler->m_after_initial_playback = true;
      },
      this, handler);

  return handler_ret;
}

} // namespace dmn

#endif // DMN_DMESG_HPP_
