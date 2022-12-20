cmake_minimum_required(VERSION 3.13)

project(diggable VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    find_program(ld_gold ld.gold)
    if(NOT "${ld_gold}" STREQUAL "ld_gold-NOTFOUND")
        message(STATUS "found gold linker!")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
    endif()
endif()

if(q)
    add_definitions(-DQ_ONLY)
endif()

if(matchmaker_DL STREQUAL "ON")
    add_definitions(-DMM_DYNAMIC_LOADING)
else()
    get_filename_component(rpath_dir "${matchmaker_DIR}/.." REALPATH)
    set(CMAKE_INSTALL_RPATH "${rpath_dir}")
    find_package(matchmaker 3.1.0 REQUIRED)
endif()


find_package(FLTK REQUIRED)
find_package(Threads REQUIRED)

include_directories(
    ${FLTK_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(
    diggable_srcs
    src/diggable.cpp
    src/BookViewer.cpp
    src/BookViewer.h
    src/LocationViewer.cpp
    src/LocationViewer.h
    # src/TermViewer.cpp
    # src/TermViewer.h
    # src/CompletionStack.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/matchmaker.cpp
    src/matchmaker.h
    src/Settings.h
    src/Viewer.cpp
    src/Viewer.h
)


add_executable(diggable ${diggable_srcs})
target_link_libraries(diggable Threads::Threads ${FLTK_LIBRARIES} stdc++fs)

if(matchmaker_DL STREQUAL "ON")
    target_link_libraries(diggable dl)
else()
    target_link_libraries(diggable matchmaker)
endif()

target_include_directories(
    diggable
    PRIVATE
    include
    matchable/include
)
install(TARGETS diggable DESTINATION bin)
