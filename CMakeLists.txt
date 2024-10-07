cmake_minimum_required(VERSION 3.9)

project(pmu
    VERSION 0.0.0
    DESCRIPTION "Lightweight event monitoring"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall -Wextra -Werror -ffunction-sections -fdata-sections")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fno-omit-frame-pointer")
endif()

# =========== Main shared library
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB src_files "src/*.c")
file(GLOB public_headers "include/*.h")
add_library(${PROJECT_NAME} SHARED ${src_files})
target_link_options(${PROJECT_NAME} BEFORE PUBLIC -Wl,--gc-sections PUBLIC)
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    PUBLIC_HEADER "${public_headers}"
)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

# =========== Test Suite
enable_testing()
file(GLOB files "tests/*.c")
foreach(file ${files})
    cmake_path(GET file STEM filename)
    add_executable(${filename} ${file})
    set_target_properties(${filename} PROPERTIES COMPILE_FLAGS "-g -fno-omit-frame-pointer")
    set_target_properties(${filename} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")
    target_link_options(${PROJECT_NAME} BEFORE PUBLIC -Wl,--gc-sections PUBLIC)
    target_link_libraries(${filename} ${PROJECT_NAME})
    add_test(${filename} tests/${filename})
endforeach()

# =========== Examples
option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
	file(GLOB files "examples/*.c")
	foreach(file ${files})
		cmake_path(GET file STEM filename)
		add_executable(${filename} ${file})
		set_target_properties(${filename} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples")
        target_link_options(${PROJECT_NAME} BEFORE PUBLIC -Wl,--gc-sections PUBLIC)
		target_link_libraries(${filename} ${PROJECT_NAME})
	endforeach()
endif()
