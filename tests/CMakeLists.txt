add_executable(data-test data-test.c ../src/mqtt_pub.c)

target_link_libraries(data-test data ${SDR_LIBRARIES})

add_executable(baseband-test baseband-test.c ../src/baseband.c)

if(UNIX)
target_link_libraries(baseband-test m)
endif()
