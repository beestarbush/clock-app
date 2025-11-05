cmake_minimum_required(VERSION 3.16)

project(ClockApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Network)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(clockapp
    main.cpp
    git_version.h
    drivers/Container.cpp
    drivers/Container.h
    drivers/network/Driver.cpp
    drivers/network/Driver.h
    drivers/screen/Driver.cpp
    drivers/screen/Driver.h
    drivers/storage/Driver.cpp
    drivers/storage/Driver.h
    drivers/system/Driver.cpp
    drivers/system/Driver.h
    drivers/temperature/Driver.cpp
    drivers/temperature/Driver.h
    qmlcomponents/QmlUtils.cpp
    qmlcomponents/QmlUtils.h
    qmlcomponents/RoundAnimatedImage.cpp
    qmlcomponents/RoundAnimatedImage.h
    services/datetime/Service.cpp
    services/datetime/Service.h
    services/media/Item.cpp
    services/media/Item.h
    services/media/Model.cpp
    services/media/Model.h
    services/media/Service.cpp
    services/media/Service.h
    services/notification/Service.cpp
    services/notification/Service.h
    services/qmlinterface/Service.cpp
    services/qmlinterface/Service.h
    services/remoteapi/Service.cpp
    services/remoteapi/Service.h
    services/systemmonitor/Service.cpp
    services/systemmonitor/Service.h
    services/version/Service.cpp
    services/version/Service.h
    services/remoteapi/SerializableObject.h
    services/remoteapi/DeviceStatus.cpp
    services/remoteapi/DeviceStatus.h
    services/remoteapi/MediaInfo.cpp
    services/remoteapi/MediaInfo.h
    services/remoteapi/MediaList.cpp
    services/remoteapi/MediaList.h
    services/remoteapi/DeviceConfiguration.cpp
    services/remoteapi/DeviceConfiguration.h
    services/configuration/Service.cpp
    services/configuration/Service.h
    services/notification/Item.cpp
    services/notification/Item.h
    services/notification/Model.cpp
    services/notification/Model.h
    services/notification/Service.cpp
    services/notification/Service.h
    services/Container.cpp
    services/Container.h
    applications/common/Application.h
    applications/common/Application.cpp
    applications/common/Configuration.h
    applications/common/Configuration.cpp
    applications/common/TimerConfiguration.h
    applications/common/TimerConfiguration.cpp
    applications/common/Types.h
    applications/Container.cpp
    applications/Container.h
    applications/clock/Application.cpp
    applications/clock/Application.h
    applications/clock/Configuration.cpp
    applications/clock/Configuration.h
    applications/timeelapsed/Application.h
    applications/timeelapsed/Application.cpp
    applications/countdown/Application.cpp
    applications/countdown/Application.h
    applications/debug/Application.cpp
    applications/debug/Application.h
    applications/menu/Application.cpp
    applications/menu/Application.h
    applications/menu/Item.cpp
    applications/menu/Item.h
    applications/menu/Model.cpp
    applications/menu/Model.h
    applications/setup/Application.cpp
    applications/setup/Application.h
    applications/watchface/Application.cpp
    applications/watchface/Application.h
)

target_include_directories(clockapp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# This only contains default media (when lookup on filesystem is not possible)
qt_add_resources(clockapp "media"
    PREFIX "/"
    FILES
        "media/default.gif"
)

if (CMAKE_CXX_COMPILER MATCHES "aarch64-poky-linux")
    set(PLATFORM "target")
    add_definitions(-DPLATFORM_IS_TARGET)
elseif(UNIX AND NOT APPLE)
    set(PLATFORM "host")
    add_definitions(-DPLATFORM_IS_HOST)
endif()

add_subdirectory(qml)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(clockapp PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appClockApp
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(clockapp
    PRIVATE Qt6::Quick Qt6::Network MainLibplugin ComponentLibplugin PanelLibplugin ResourcesLibplugin
)

include(GNUInstallDirs)
install(TARGETS clockapp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
