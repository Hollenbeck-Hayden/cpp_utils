cmake_minimum_required(VERSION 3.14..3.28)

project(CppUtils VERSION 0.1.0 LANGUAGES CXX)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

include(GNUInstallDirs)
include(FetchContent)

# FetchContent_Declare(
#     fmtlib
#     GIT_REPOSITORY https://github.com/fmtlib/fmt.git
#     GIT_TAG 5.3.0)
# FetchContent_MakeAvailable(fmtlib)

# Installation paths
set(CppUtils_INSTALL_BIN_DIR "${CMAKE_INSTALL_BINDIR}")
set(CppUtils_INSTALL_LIB_DIR "${CMAKE_INSTALL_LIBDIR}/CppUtils")
set(CppUtils_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/CppUtils")

# Build paths
set(CppUtils_BUILD_INCLUDE_DIR "${CppUtils_SOURCE_DIR}/src")

# Options
set(CppUtils_CXX_STD "cxx_std_14")


# Add actual library cmakes
add_subdirectory(src/CppUtils)

# Build testing
if (BUILD_TESTING)
    include(CTest)
    add_subdirectory(test)
endif()
