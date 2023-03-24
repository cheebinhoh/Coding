/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program demonstrates some features of std::thread.
 *
 * The WorkerTask is a task wrapped under an object and
 * added to the WorkerExecution' queue, and WorkerExecution
 * will have certain # of threads that runs and executes the
 * task.
 *
 * We will maintain a multiple producer (adding WorkerTask) and
 * multiple consumer (thread) implementation.
 */

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class WorkerTask {
public:
  WorkerTask(
      std::string name,
      void (*fn)(void *) =
          [](void *data) { std::cout << "default noop for task\n"; },
      int sleep = 0)
      : m_name{name}, m_fn{fn}, m_sleep{sleep} {}

  WorkerTask() : WorkerTask{"unknown"} {}

  ~WorkerTask() { std::cout << "Delete workertask: " << m_name << "\n"; }

  WorkerTask(const WorkerTask &copy) = delete;
  WorkerTask &operator=(const WorkerTask &copy) = delete;

private:
  std::string m_name{};
  int m_sleep{};
  void (*m_fn)(void *){};
};

class WorkerExecution;

void doWorkerExecution(WorkerExecution *we);

class WorkerExecution {
public:
  enum State { Initialized, Stop };

  friend void doWorkerExecution(WorkerExecution *we);

  WorkerExecution(int num_workers = 1) : m_num_workers{num_workers} {
    int i;

    m_state = State::Initialized;
    for (i = 0; i < m_num_workers; i++) {
      m_workers.emplace_back(doWorkerExecution, this);
    }
  }

  ~WorkerExecution() {
    int i;

    m_mutex.lock();
    m_state = Stop;
    m_mutex.unlock();

    for (i = 0; i < m_num_workers; i++) {
      m_workers[i].join();
    }
  }

  bool addWorkerTask(std::unique_ptr<WorkerTask> t) {
    m_queue.push_back(std::move(t));
    std::cout << "# of queue size: " << m_queue.size() << "\n";

    return true;
  }

private:
  void func() { std::cout << "calling func\n"; }

  std::vector<std::unique_ptr<WorkerTask>> m_queue{};
  std::vector<std::thread> m_workers{};
  int m_num_workers{};
  State m_state{};
  std::mutex m_mutex{};
};

void doWorkerExecution(WorkerExecution *we) {
  while (1) {
    we->m_mutex.lock();
    if (WorkerExecution::State::Stop == we->m_state) {
      we->m_mutex.unlock();
      break;
    }
    we->m_mutex.unlock();
    std::this_thread::yield();
  }
}

int main(int argc, char *argv[]) {
  using std::chrono::system_clock;

  std::time_t tt = system_clock::to_time_t(system_clock::now());
  struct std::tm *ptm = std::localtime(&tt);
  std::cout << "Current time is " << std::put_time(ptm, "%X") << "\n";

  std::srand(std::time(nullptr));

  {
    std::unique_ptr<WorkerTask> t1 = std::make_unique<WorkerTask>(
        "first", [](void *data) { std::cout << "first task"; }, 5);
    std::unique_ptr<WorkerTask> t2 = std::make_unique<WorkerTask>(
        "second", [](void *data) { std::cout << "second task"; }, 3);
    WorkerExecution exec{2};

    exec.addWorkerTask(std::move(t1));
    exec.addWorkerTask(std::move(t2));
  }

  tt = system_clock::to_time_t(system_clock::now());
  ptm = std::localtime(&tt);
  std::cout << "Current time before end is " << std::put_time(ptm, "%X")
            << "\n";

  return 0;
}
