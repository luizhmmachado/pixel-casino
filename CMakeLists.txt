cmake_minimum_required(VERSION 3.16)

project(pixel-casino VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 5.15 REQUIRED COMPONENTS Quick Sql Concurrent Svg)

get_filename_component(QT_INSTALL_PREFIX "${Qt5Core_DIR}/../../.." ABSOLUTE)

set(PROJECT_SOURCES
    control/blackjack/blackjackcontrol.cpp
    control/blackjack/blackjackcontrol.h
    control/database/databasecontrol.cpp
    control/database/databasecontrol.h
    control/database/offlinequeue.cpp
    control/database/offlinequeue.h
    control/database/profilecontrol.cpp
    control/database/profilecontrol.h
    control/database/supabaseapi.cpp
    control/database/supabaseapi.h
    control/database/transactioncontrol.cpp
    control/database/transactioncontrol.h
    control/horserace/horsemodel.cpp
    control/horserace/horsemodel.h
    control/horserace/horseracecontrol.cpp
    control/horserace/horseracecontrol.h
    main.cpp
    qml.qrc
    resources.qrc
)

add_executable(pixel-casino ${PROJECT_SOURCES})

if(ANDROID AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/android_openssl/android_openssl.cmake")
    include("${CMAKE_CURRENT_SOURCE_DIR}/android_openssl/android_openssl.cmake")
    add_android_openssl_libraries(pixel-casino)
endif()

target_include_directories(pixel-casino PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(pixel-casino PRIVATE
    Qt5::Quick
    Qt5::Sql
    Qt5::Concurrent
    Qt5::Svg
)

if(ANDROID)
    find_library(EGL_LIB EGL)
    find_library(GLESV2_LIB GLESv2)
    find_library(Z_LIB z)

    target_link_libraries(pixel-casino PRIVATE
        ${EGL_LIB}
        ${GLESV2_LIB}
        ${Z_LIB}
    )
endif()

install(TARGETS pixel-casino
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

if(ANDROID)
    set_property(TARGET pixel-casino PROPERTY QT_ANDROID_MIN_SDK_VERSION 23)
    set_property(TARGET pixel-casino PROPERTY QT_ANDROID_TARGET_SDK_VERSION 31)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/android-pixel-casino-deployment-settings.json.in
        ${CMAKE_CURRENT_BINARY_DIR}/android-pixel-casino-deployment-settings.json
        @ONLY
    )
endif()
