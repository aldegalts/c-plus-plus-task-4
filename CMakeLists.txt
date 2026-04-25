cmake_minimum_required(VERSION 3.28)
project(task4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.8.3/mingw_64")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
    main.cpp
    src/MainWindow.cpp
    src/DeclensionDatabase.cpp
    src/FileProcessor.cpp
    src/Logger.cpp
)

set(HEADERS
    include/MainWindow.h
    include/DeclensionDatabase.h
    include/FileProcessor.h
    include/Logger.h
)

add_executable(task4 ${SOURCES} ${HEADERS})

target_include_directories(task4 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(task4 PRIVATE Qt6::Widgets)

set_target_properties(task4 PROPERTIES
    WIN32_EXECUTABLE $<CONFIG:Release>
)
