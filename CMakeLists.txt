cmake_minimum_required(VERSION 3.16.3)

project(diggable VERSION 17.17.17)

set(CMAKE_CXX_STANDARD 17)
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
    get_filename_component(matchmaker_name "${matchmaker_DIR}/.." REALPATH)
    get_filename_component(matchmaker_name "${matchmaker_name}" NAME)
    set(
        CMAKE_INSTALL_RPATH
        "$ORIGIN/../lib/${matchmaker_name}"
        "${rpath_dir}"
    )
    find_package(matchmaker 4.0.0 REQUIRED)
endif()


find_package(FLTK REQUIRED)


find_package(Threads REQUIRED)

include_directories(
    ${FLTK_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(
    diggable_srcs
    src/AbstractViewer.cpp
    src/AbstractViewer.h
    src/CellViewer.cpp
    src/CellViewer.h
    src/BookViewer.cpp
    src/BookViewer.h
    src/Data.cpp
    src/Data.h
    src/LocationViewer.cpp
    src/LocationViewer.h
    src/Settings.h
    src/TermViewer.cpp
    src/TermViewer.h
    src/CompletionStack.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/diggable.cpp
    src/matchmaker.cpp
    src/matchmaker.h
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

file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/${CMAKE_PROJECT_NAME})

install(
    DIRECTORY
    ${PROJECT_SOURCE_DIR}/data/images
    DESTINATION share/${CMAKE_PROJECT_NAME}
)
