cmake_minimum_required(VERSION 3.16)
project(FaceRecognitionLogin VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
    MultimediaWidgets
    Sql
)

set(SOURCES
    main.cpp
    appcontroller.cpp
    logindialog.cpp
    registerdialog.cpp
    mainwindow.cpp
    facedetector.cpp
    facerecognizer.cpp
    databasemanager.cpp
)

set(HEADERS
    appcontroller.h
    logindialog.h
    registerdialog.h
    mainwindow.h
    facedetector.h
    facerecognizer.h
    databasemanager.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::Sql
)

# Windows-specific settings
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()