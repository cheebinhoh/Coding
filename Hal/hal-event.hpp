/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#ifndef HAL_EVENT_HPP_HAVE_SEEN
#define HAL_EVENT_HPP_HAVE_SEEN

#include "hal-async.hpp"
#include "hal-singleton.hpp"

#include <iostream>
#include <mutex>

class Hal_Event_Manager : public Hal_Singleton, Hal_Async {
public:
  Hal_Event_Manager();
  virtual ~Hal_Event_Manager();

  void exitMainLoop();

  void enterMainLoop();

  template <class... U>
  static std::shared_ptr<Hal_Event_Manager> createInstanceInternal(U &&...u) {
    if (!Hal_Event_Manager::s_instance) {
      std::call_once(
          s_InitOnce,
          [](U &&...arg) {
            Hal_Event_Manager::s_instance =
                std::make_shared<Hal_Event_Manager>(std::forward<U>(arg)...);
          },
          std::forward<U>(u)...);
    }

    return Hal_Event_Manager::s_instance;
  }

private:
  void exitMainLoopPrivate();

  static std::shared_ptr<Hal_Event_Manager> s_instance;
  static std::once_flag s_InitOnce;
};

#endif /* HAL_EVENT_HPP_HAVE_SEEN */
