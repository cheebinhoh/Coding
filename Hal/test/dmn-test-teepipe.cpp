/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
 *
 * This is a test file for teepipe that two threads are reading data and feed
 * into teepipe that will sort the data and make sure that it is processed
 * in order, if the data already sorted in the source data, the teepipe will
 * coordinate high level sorting between two stream of data.
 */

#include "dmn-proc.hpp"
#include "dmn-teepipe.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  
  char *dirpath = dirname(argv[0]);
  std::vector<std::string> files{std::string(dirpath) + "/teepipe-test-data-1.txt",
                                 std::string(dirpath) + "/teepipe-test-data-2.txt"};
  Dmn::Dmn_TeePipe<long> tpipe{
      "teepipe", [](long val) { std::cout << val << "\n"; },
      [](std::vector<long> list) { std::sort(list.begin(), list.end()); }};
  std::vector<std::unique_ptr<Dmn::Dmn_Proc>> proclist{};

  for (auto &filename : files) {
    auto tpipeSource = tpipe.addDmn_TeePipeSource();
    auto proc = std::make_unique<Dmn::Dmn_Proc>(
        filename, [&tpipe, tpipeSource, filename, prog = argv[0]]() {
          int fd{};
          FILE *file{};
          char buf[BUFSIZ]{};

          fd = open(filename.c_str(), O_RDONLY);
          if (-1 == fd) {
            perror(prog);
            exit(1);
          }

          file = fdopen(fd, "r");
          if (NULL == file) {
            perror(prog);
            exit(1);
          }

          while (fgets(buf, sizeof(buf), file) != NULL) {
            if ('\n' == buf[strlen(buf) - 1]) {
              buf[strlen(buf) - 1] = '\0';
            }

            long val{};
            val = strtol(buf, NULL, 10);

            tpipeSource->write(val);

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
          }

          if (ferror(file)) {
            perror(prog);
            exit(1);
          }

          fclose(file);
        });

    proclist.push_back(std::move(proc));
  }

  for (auto &proc : proclist) {
    proc->exec();
    Dmn::Dmn_Proc::yield();
  }

  Dmn::Dmn_Proc::yield();

  for (auto &proc : proclist) {
    proc->wait();
  }

  tpipe.waitForEmpty();

  return RUN_ALL_TESTS();
}
