cmake_minimum_required(VERSION 3.22)
set(APP_NAME AndroidApp)
project(${APP_NAME} LANGUAGES CXX)

set(CORE
    main.cpp game.cpp
    resources.qrc
    settings.cpp tasks.cpp style.cpp

    base/choose.cpp
    base/task.cpp
    base/taskload.cpp
    base/options.cpp
    base/converse.cpp
    base/externs.cpp
    base/scenes.cpp
    base/quals.cpp

    extra/itemopts.cpp
    extra/drag.cpp
    extra/bgitem.cpp

    wids/flow.cpp
    wids/house.cpp
    wids/icobtn.hpp
    wids/slider.cpp
    wids/secret.cpp
    wids/taskbbl.cpp
    wids/txtedit.cpp
    wids/confirm.cpp
    wids/taskOverlay.cpp
    wids/renameOverl.cpp
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Xml Svg SvgWidgets Widgets)
qt_policy(SET QTP0002 NEW) # Shushes a warning

include(FetchContent)
FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG master
)
FetchContent_MakeAvailable(yaml-cpp)

qt_add_executable(${APP_NAME} ${CORE})
target_link_libraries(${APP_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Xml Qt6::Svg Qt6::SvgWidgets Qt6::Widgets yaml-cpp::yaml-cpp)

target_include_directories(${APP_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

set_property(TARGET ${APP_NAME} PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
