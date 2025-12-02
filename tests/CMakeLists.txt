add_executable(unit_tests test_miners.cpp)
target_link_libraries(unit_tests PRIVATE miners_lib Catch2::Catch2WithMain)
