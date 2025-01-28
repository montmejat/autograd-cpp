add_executable(test_autograd test_autograd.cpp)
target_link_libraries(test_autograd PRIVATE autograd gtest gtest_main)

add_test(NAME AutoGradTest COMMAND test_autograd)
