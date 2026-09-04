cmake_minimum_required(VERSION 3.30)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/src/vendor/GameNetworkingSockets-1.6.0/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "" FORCE)
set(VCPKG_MANIFEST_MODE ON CACHE BOOL "" FORCE)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(valve_network)

add_subdirectory("src/vendor/GameNetworkingSockets-1.6.0")

file(GLOB PROJECT_SRC
    "src/*.cpp"
)

add_executable(${PROJECT_NAME} ${PROJECT_SRC})

target_link_libraries(${PROJECT_NAME} PUBLIC GameNetworkingSockets)

