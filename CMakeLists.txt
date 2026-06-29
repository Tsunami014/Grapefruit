cmake_minimum_required(VERSION 3.22)
set(APP_NAME AndroidApp)
project(${APP_NAME} LANGUAGES CXX)

set(CORE
    main.cpp
    src/game.cpp
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_policy(SET QTP0002 NEW) # Shushes a warning

qt_add_executable(${APP_NAME} ${CORE})
target_link_libraries(${APP_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

set_property(TARGET ${APP_NAME} PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
