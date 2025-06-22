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
                      # FIXME: the following test is not deterministic
                      # dmn-test-dmesg-4
                      dmn-test-dmesg-5
                      dmn-test-dmesg-6
                      dmn-test-dmesg-7
                      dmn-test-dmesg-8
                      dmn-test-dmesgnet-1
                      dmn-test-dmesgnet-2
                      dmn-test-dmesgnet-3
                      dmn-test-dmesgnet-4
                      dmn-test-dmesgnet-5
                      dmn-test-dmesgnet-6
                      dmn-test-dmesgnet-7
                      dmn-test-dmesgnet-8
                      dmn-test-dmesg-pb
                      dmn-test-dmesg-pb-util
                      dmn-test-io
                      dmn-test-pipe
                      dmn-test-pub-sub
                      dmn-test-singleton
                      dmn-test-socket
                      dmn-test-teepipe
                      dmn-test-timer
                      # FIXME: the following test needs to be refactored to avoid none-stop running
                      # dmn-test
                    )

if (BUILD_KAFKA_TEST)
ADD_TEST_EXECUTABLE(dmn-kafka
                      dmn-test-dmesgnet-with-kafka-1
                      dmn-test-dmesgnet-with-kafka-2
                      dmn-test-dmesgnet-with-kafka-3
                      dmn-test-dmesgnet-with-kafka-4
                      dmn-test-dmesgnet-with-kafka-5
                      dmn-test-kafka
                      dmn-test-kafka-protobuf
                    )
endif()
