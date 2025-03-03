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

#include "hal-dmesg.hpp"
#include "hal-io.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <string_view>

namespace Hal {

class Hal_DMesgNet : public Hal_DMesg {
public:
  Hal_DMesgNet(std::string_view name,
               std::shared_ptr<Hal_Io<std::string>> outputHandler = nullptr,
               std::shared_ptr<Hal_Io<std::string>> inputHandler = nullptr)
      : Hal_DMesg{name}, m_name{name}, m_outputHandler{outputHandler},
        m_inputHandler{inputHandler} {

    // subscriptHandler to read and write with local DMesg
    m_subscriptHandler = Hal_DMesg::openHandler(
        m_name,
        [this](const Hal::DMesgPb &dmesgPb) {
          return dmesgPb.source() != this->m_name;
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
            dmesgPb.set_source(this->m_name);
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

            if (dmesgPbRead.source() != this->m_name) {
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
              dmesgPbRead.set_source(this->m_name);

              this->m_subscriptHandler->write(dmesgPbRead);
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
};

} /* namespace Hal */

#endif /* HAL_DMESGNET_HPP_HAVE_SEEN */
