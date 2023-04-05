add_executable(speedtest speedtest.cc)
add_executable(speedtesttune speedtest-tune.cc)
target_link_libraries(speedtest btree)
target_link_libraries(speedtesttune btree)
