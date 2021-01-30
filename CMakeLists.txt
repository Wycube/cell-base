if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory.")
endif()

cmake_minimum_required(VERSION 3.10)

project(cell C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_SKIP_INSTALL_RULES True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/bin)

# This silences some warnings using _CRT_SECURE_NO_WARNINGS for glfw
if(MSVC)
	add_compile_options(/D_CRT_SECURE_NO_WARNINGS)
else()
	add_compile_options(-D_CRT_SECURE_NO_WARNINGS)
endif()

#
# Add libraries here (lua)
#

# Add glfw
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_DOCS OFF)

# This was causing an error with clang receiving MSVC-like options, disable if you need to
set(CMAKE_C_SIMULATE_ID "GNU")

include_directories(lib/glfw/include)
add_subdirectory(lib/glfw)

# Add glad
include_directories(lib/glad/include)
add_subdirectory(lib/glad)


# Add project includes
include_directories(include)

# Add src as subdirectory
add_subdirectory(src)