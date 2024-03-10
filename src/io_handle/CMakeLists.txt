file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${CppUtils_SOURCE_DIR}/include/io_handle/*.h")

add_library(io_handle ${HEADER_LIST})

target_sources(io_handle PRIVATE
    DeviceHandle.cpp
    FileHandle.cpp
)

target_include_directories(io_handle PUBLIC "${CppUtils_SOURCE_DIR}/include")

# target_link_libraries(io_handle PRIVATE fmt::fmt)

target_compile_features(io_handle PUBLIC cxx_std_14)
