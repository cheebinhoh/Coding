# Copyright © 2025 Chee Bin HOH. All rights reserved.

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/teepipe-test-data-1.txt
       DESTINATION ${CMAKE_BINARY_DIR}/test)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/teepipe-test-data-2.txt
       DESTINATION ${CMAKE_BINARY_DIR}/test)

ADD_TEST_EXECUTABLE(dmn
                      dmn-test-async
                      dmn-test-buffer
                      dmn-test-dmesg-1
                      dmn-test-dmesg-2
                      dmn-test-dmesg-3
                      dmn-test-dmesgnet-1
                      dmn-test-dmesgnet-2
                      dmn-test-dmesgnet-3
                      dmn-test-dmesgnet-4
                      dmn-test-dmesgnet-5
                      dmn-test-dmesgnet-6
                      dmn-test-dmesg-pb
                      dmn-test-dmesg-pb-util
                      dmn-test-io
                      dmn-test-pipe
                      dmn-test-pub-sub
                      dmn-test-singleton
                      dmn-test-socket
                      dmn-test-teepipe
                      dmn-test-timer
                      dmn-test)
