# Copyright © 2025 Chee Bin HOH. All rights reserved.

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/teepipe-test-data-1.txt
       DESTINATION ${CMAKE_BINARY_DIR}/test)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/teepipe-test-data-2.txt
       DESTINATION ${CMAKE_BINARY_DIR}/test)

ADD_TEST_EXECUTABLE(hal
                      hal-test-async
                      hal-test-buffer
                      hal-test-dmesg-1
                      hal-test-dmesg-2
                      hal-test-dmesg-3
                      hal-test-dmesgnet-1
                      hal-test-dmesgnet-2
                      hal-test-dmesgnet-3
                      hal-test-dmesgnet-4
                      hal-test-dmesgnet-5
                      hal-test-dmesg-pb
                      hal-test-dmesg-pb-util
                      hal-test-io
                      hal-test-pipe
                      hal-test-pub-sub
                      hal-test-singleton
                      hal-test-socket
                      hal-test-teepipe
                      hal-test-timer
                      hal-test)
