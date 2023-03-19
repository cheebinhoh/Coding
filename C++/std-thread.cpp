/**
 * Copyright © 2023 Chee Bin HOH. All rights reserved.
 *
 * This program demonstrates some features of std::thread.
 */

#include <chrono>
#include <iostream>
#include <memory>
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
  friend void doWorkerExecution(WorkerExecution *we);

  WorkerExecution(int num_workers = 1) : m_num_workers{num_workers} {
    int i;

    for (i = 0; i < m_num_workers; i++) {
      m_workers.emplace_back(doWorkerExecution, this);
    }
  }

  ~WorkerExecution() {
    int i;

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
};

void doWorkerExecution(WorkerExecution *we) { std::cout << "worker is run\n"; }

int main(int argc, char *argv[]) {
  std::unique_ptr<WorkerTask> t1 = std::make_unique<WorkerTask>(
      "first", [](void *data) { std::cout << "first task"; }, 5);
  std::unique_ptr<WorkerTask> t2 = std::make_unique<WorkerTask>(
      "second", [](void *data) { std::cout << "second task"; }, 3);
  WorkerExecution exec{2};

  exec.addWorkerTask(std::move(t1));
  exec.addWorkerTask(std::move(t2));

  return 0;
}
