#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

#include "hal-pipe.hpp"
#include "hal-proc.hpp"

std::mutex log_mutex {};

#define safethread_log(stmt) do { \
  log_mutex.lock(); \
  stmt; \
  log_mutex.unlock(); \
} while (0)

int main(int argc, char *argv[])
{
  Hal_Proc gpr_input {"gpr input"};
  Hal_Proc ext_input {"ext input"};

  std::srand(std::time(NULL));

  Hal_Pipe<std::string> out_pipe {"out_pipe", [](std::string item) {
     std::cout << "out_pipe: pop: " << item << "\n";
  } };

  Hal_Pipe<std::string> staging_pipe {"staging_input", [&out_pipe](std::string item) {
    out_pipe.push(item);
  } };

  gpr_input.exec([&out_pipe]() {
    for (int i = 0; i < 10; i++) {
       std::string item = "gpr_input " + std::to_string(i);
       out_pipe.push(item);

       int to_sleep = ((std::rand() % 10) + 1) * 100;

       //safethread_log(std::cout << "gpr input: milliseconds:" << to_sleep << "\n");
       std::this_thread::sleep_for(std::chrono::milliseconds(to_sleep));
    }

    safethread_log(std::cout << "gpr input: end\n");
  } );

  ext_input.exec([&out_pipe]() {
    for (int i = 0; i < 10; i++) {
       std::string item = "ext_input " + std::to_string(i);
       out_pipe.push(item);

       int to_sleep = ((std::rand() % 10) + 1) * 100;

       //safethread_log(std::cout << "ext input: milliseconds:" << to_sleep << "\n");
       std::this_thread::sleep_for(std::chrono::milliseconds(to_sleep));
    }

    safethread_log(std::cout << "ext input: end\n");
  } );


  safethread_log(std::cout << "Before mainloop\n");
  while (true) {
    std::this_thread::yield();
  }

  return 0;
}
