cmake_minimum_required(VERSION 3.30)

if (WIN32)
    set(CMAKE_C_COMPILER cl.exe)
    set(CMAKE_CXX_COMPILER cl.exe)
    # Hard-fail if invoked from an environment where mingw is leaking in
    if (DEFINED ENV{PATH})
        string(FIND "$ENV{PATH}" "msys64" MSYS_POS)
        if (NOT MSYS_POS EQUAL -1)
            message(WARNING "msys64 detected in PATH — this can cause MSVC to pick up MinGW headers. Using CMAKE_IGNORE_PATH below to suppress it.")
        endif()
    endif()

    list(APPEND CMAKE_IGNORE_PATH
        "C:/msys64/mingw64"
        "C:/msys64/mingw64/bin"
        "C:/msys64/mingw64/include"
        "C:/msys64/mingw64/lib"
        "C:/msys64/usr/bin"
        "C:/msys64/usr/include"
    )
    list(APPEND CMAKE_SYSTEM_IGNORE_PATH ${CMAKE_IGNORE_PATH})
else()
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
endif()
set(CMAKE_TOOLCHAIN_FILE
    "${CMAKE_SOURCE_DIR}/src/vendor/GameNetworkingSockets-1.6.0/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "" FORCE)
set(VCPKG_MANIFEST_MODE ON CACHE BOOL "" FORCE)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(valve_network)

add_subdirectory("src/vendor/GameNetworkingSockets-1.6.0")
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory("src/vendor/SDL3-3.4.16")
add_subdirectory("src/vendor/SDL_image-release-3.4.6")

file(GLOB PROJECT_SRC
    "src/*.cpp"
)

add_executable(${PROJECT_NAME} ${PROJECT_SRC})

target_include_directories(${PROJECT_NAME}
    PUBLIC "src/vendor/Nuklear-4.13.3"
    PUBLIC "src/vendor/Nuklear-4.13.3/demo"
)
target_link_libraries(${PROJECT_NAME} 
    PRIVATE GameNetworkingSockets_s
    PRIVATE SDL3::SDL3-static
    PRIVATE SDL3_image-static
)

function(copy_resource SOURCE_DIR DEST_DIR)
  file(GLOB COPY_SOURCES "${SOURCE_DIR}/*")
  file(MAKE_DIRECTORY ${DEST_DIR})
  foreach(SRC_FILE IN LISTS COPY_SOURCES)
    message(STATUS "Copy file ${SRC_FILE} to ${DEST_DIR}")
    file(COPY ${SRC_FILE} DESTINATION ${DEST_DIR})
  endforeach()
endfunction()

copy_resource("${CMAKE_SOURCE_DIR}/res/textures" "${CMAKE_BINARY_DIR}/res/textures")
