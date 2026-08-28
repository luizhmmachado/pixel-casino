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

if(ANDROID)
    add_library(pixel-casino SHARED ${PROJECT_SOURCES})
else()
    add_executable(pixel-casino ${PROJECT_SOURCES})
endif()

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
    find_library(ANDROID_LIB android)
    find_library(DL_LIB dl)
    find_library(EGL_LIB EGL)
    find_library(GLESV2_LIB GLESv2)
    find_library(LOG_LIB NAMES log
        PATHS
            "${CMAKE_SYSROOT}/usr/lib/${CMAKE_ANDROID_ARCH_TRIPLE}/${CMAKE_SYSTEM_VERSION}"
            "${CMAKE_SYSROOT}/usr/lib/${CMAKE_ANDROID_ARCH_TRIPLE}"
        NO_DEFAULT_PATH
    )
    if(NOT LOG_LIB)
        find_library(LOG_LIB log)
    endif()
    find_library(M_LIB m)
    find_library(Z_LIB NAMES z
        PATHS
            "${CMAKE_SYSROOT}/usr/lib/${CMAKE_ANDROID_ARCH_TRIPLE}/${CMAKE_SYSTEM_VERSION}"
            "${CMAKE_SYSROOT}/usr/lib/${CMAKE_ANDROID_ARCH_TRIPLE}"
        NO_DEFAULT_PATH
    )
    if(NOT Z_LIB)
        find_library(Z_LIB z)
    endif()
    find_library(CPP_SHARED_LIB c++_shared)

    set(ANDROID_SYSROOT_LIB_DIR "${CMAKE_SYSROOT}/usr/lib/${CMAKE_ANDROID_ARCH_TRIPLE}/${CMAKE_SYSTEM_VERSION}")
    set(ANDROID_SYSROOT_LIB_DIR_FALLBACK "${CMAKE_SYSROOT}/usr/lib/${CMAKE_ANDROID_ARCH_TRIPLE}")

    # Qt shared libraries pull these transitively; keep them even if
    # the app itself doesn't reference their symbols directly.
    target_link_options(pixel-casino PRIVATE "-Wl,--no-as-needed")
    if(EXISTS "${ANDROID_SYSROOT_LIB_DIR}")
        target_link_options(pixel-casino PRIVATE "-Wl,-rpath-link,${ANDROID_SYSROOT_LIB_DIR}")
    endif()
    if(EXISTS "${ANDROID_SYSROOT_LIB_DIR_FALLBACK}")
        target_link_options(pixel-casino PRIVATE "-Wl,-rpath-link,${ANDROID_SYSROOT_LIB_DIR_FALLBACK}")
    endif()
    if(EXISTS "${QT_INSTALL_PREFIX}/lib")
        target_link_options(pixel-casino PRIVATE "-Wl,-rpath-link,${QT_INSTALL_PREFIX}/lib")
    endif()

    target_link_libraries(pixel-casino PRIVATE
        ${ANDROID_LIB}
        ${DL_LIB}
        ${EGL_LIB}
        ${GLESV2_LIB}
        ${LOG_LIB}
        ${M_LIB}
        ${Z_LIB}
        ${CPP_SHARED_LIB}
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

    set(ANDROID_SDK_ROOT "$ENV{ANDROID_SDK_ROOT}")
    if(NOT ANDROID_SDK_ROOT)
        set(ANDROID_SDK_ROOT "$ENV{ANDROID_HOME}")
    endif()

    set(ANDROID_NDK_ROOT "$ENV{ANDROID_NDK_ROOT}")
    if(NOT ANDROID_NDK_ROOT)
        set(ANDROID_NDK_ROOT "$ENV{ANDROID_NDK_HOME}")
    endif()

    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
        set(ANDROID_NDK_HOST "linux-x86_64")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        set(ANDROID_NDK_HOST "darwin-x86_64")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        set(ANDROID_NDK_HOST "windows-x86_64")
    endif()

    set(ANDROID_STL_PATH "")
    if(ANDROID_NDK_ROOT AND ANDROID_NDK_HOST)
        set(ANDROID_STL_PATH "${ANDROID_NDK_ROOT}/toolchains/llvm/prebuilt/${ANDROID_NDK_HOST}/sysroot/usr/lib")
    endif()

    set(ANDROID_ARCH_TRIPLE "")
    if(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
        set(ANDROID_ARCH_TRIPLE "aarch64-linux-android")
    elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
        set(ANDROID_ARCH_TRIPLE "arm-linux-androideabi")
    endif()

    set(ANDROID_ARCHITECTURES_JSON "{}")
    if(CMAKE_ANDROID_ARCH_ABI AND ANDROID_ARCH_TRIPLE)
        set(ANDROID_ARCHITECTURES_JSON "{\"${CMAKE_ANDROID_ARCH_ABI}\":\"${ANDROID_ARCH_TRIPLE}\"}")
    endif()

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/android-pixel-casino-deployment-settings.json.in
        ${CMAKE_CURRENT_BINARY_DIR}/android-pixel-casino-deployment-settings.json
        @ONLY
    )
endif()
