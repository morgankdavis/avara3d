cmake_minimum_required(VERSION 3.20)

project(a3de VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets OpenGLWidgets)
#find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets OpenGLWidgets)
#find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(Qt6 COMPONENTS Widgets OpenGLWidgets QUIET)

if (Qt6_FOUND)

    set(PROJECT_SOURCES
            main.cpp
            MainWindow.h
            MainWindow.cpp
            MainWindow.ui
            QtViewport.h
            QtViewport.cpp
            QtInputManager.h
            QtInputManager.cc
    )

    #if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_add_executable(a3de
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
        )
    # Define target properties for Android with Qt 6 as:
    #    set_property(TARGET a3de APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
    #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
    # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
    #else()
    #    if(ANDROID)
    #        add_library(a3de SHARED
    #            ${PROJECT_SOURCES}
    #        )
    ## Define properties for Android with Qt 5 after find_package() calls as:
    ##    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    #    else()
    #        add_executable(a3de
    #            ${PROJECT_SOURCES}
    #        )
    #    endif()
    #endif()

    target_include_directories(a3de PRIVATE ${CMAKE_SOURCE_DIR}/external/imgui-1.92.5)
    #target_include_directories(a3de PRIVATE ${CMAKE_SOURCE_DIR}/external/magic_enum-0.9.3/include)

    #target_link_libraries(a3de PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::OpenGLWidgets)
    target_link_libraries(a3de PRIVATE
            Qt6::Widgets
            Qt6::OpenGLWidgets
            avara3d
            imgui)

    # Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
    # If you are developing for iOS or macOS you should consider setting an
    # explicit, fixed bundle identifier manually though.
    #if(${QT_VERSION} VERSION_LESS 6.1.0)
    #  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER net.avara3d.editor)
    #endif()
    set_target_properties(a3de PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER net.avara3d.editor
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
    )

    #include(GNUInstallDirs)
    #install(TARGETS a3de
    #    BUNDLE DESTINATION .
    #    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    #    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    #)

    #if(QT_VERSION_MAJOR EQUAL 6)
        qt_finalize_executable(a3de)
    #endif()

else ()

    message(STATUS "Qt6 not found – skipping a3de.")
    return()

endif ()
