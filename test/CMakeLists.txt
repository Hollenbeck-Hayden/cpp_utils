FetchContent_Declare(
    catch
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v2.13.6)
FetchContent_MakeAvailable(catch)

add_executable(test_io TestIO.cpp)

target_compile_features(test_io PRIVATE cxx_std_17)

target_include_directories(test_io PRIVATE "${CppUtils_SOURCE_DIR}/include")
target_link_libraries(test_io PRIVATE io_handle)

target_link_libraries(test_io PRIVATE Catch2::Catch2)

add_test(NAME test_io_test COMMAND test_io)
