cmake_minimum_required(VERSION 3.25)
project(terrain_rendering)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH $ENV{QT_PATH})
set(Eigen3_DIR "./dependencies/Eigen3.4.0/share/eigen3/cmake")
set(GDAL_DIR "./dependencies/warmerda/bld")
set(JSON_DIR "./dependencies/JSON")

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

find_package(Eigen3 REQUIRED)
find_package(OpenGL REQUIRED)

INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})

INCLUDE_DIRECTORIES(${JSON_DIR})

INCLUDE_DIRECTORIES(src)

file(GLOB ALLFILE
        "src/*.cpp" "src/*.h"
        "src/ui/*.cpp" "src/ui/*.h"
        "src/scene/element/*.cpp" "src/scene/element/*.h" "src/scene/accelerator/*.cpp" "src/scene/accelerator/*.h"
        "src/scene/*.cpp" "src/scene/*.h"
        "src/scene/common/*.cpp" "src/scene/common/*.h"
        "src/scene/camera/*.cpp" "src/scene/camera/*.h")

add_executable(terrain_rendering
        ${ALLFILE})

target_link_libraries(terrain_rendering
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        OpenGL::GL)

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt5${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
