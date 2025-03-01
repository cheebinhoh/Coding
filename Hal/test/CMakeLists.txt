ADD_TEST_EXECUTABLE(hal
                    hal-test-async
                    hal-test-buffer
                    hal-test-dmesg
                    hal-test-dmesg-2
                    hal-test-dmesg-pb
                    hal-test-io
                    hal-test-pipe
                    hal-test-pub-sub
                    hal-test-singleton
                    hal-test-teepipe
                    hal-test-timer
                    hal-test)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/teepipe-test-data-1.txt
       DESTINATION ${CMAKE_BINARY_DIR}/test)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/teepipe-test-data-2.txt
       DESTINATION ${CMAKE_BINARY_DIR}/test)
