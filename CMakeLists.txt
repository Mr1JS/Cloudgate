cmake_minimum_required(VERSION 3.5)

project(Cloudgate_game VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Qt6_DIR to use the installed Qt 6.10.1
# This ensures CMake finds the correct Qt version
set(Qt6_DIR "$ENV{HOME}/Qt/6.10.1/gcc_64/lib/cmake/Qt6" CACHE PATH "Qt6 directory")

# OPTIONAL: Setup C++ compiler flags for debug mode
# set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")

# Add cmake modules path for finding SDL2
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Find SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_IMAGE REQUIRED)

# Boost
find_package(Boost REQUIRED COMPONENTS filesystem)


# Find HDF5
find_package(HDF5 REQUIRED COMPONENTS C CXX HL)
# Add HighFive (header-only library)
include_directories(/ext/HighFive/include)


# Print SDL2 information for debugging
if(SDL2_FOUND)
    message(STATUS "SDL2 found!")
    message(STATUS "SDL2 include dir: ${SDL2_INCLUDE_DIR}")
    message(STATUS "SDL2 libraries: ${SDL2_LIBRARY}")
endif()

if(SDL2_IMAGE_FOUND)
    message(STATUS "SDL2_image found!")
    message(STATUS "SDL2_image include dir: ${SDL2_IMAGE_INCLUDE_DIR}")
    message(STATUS "SDL2_image libraries: ${SDL2_IMAGE_LIBRARY}")
endif()

if(Boost_FOUND)
    message(STATUS "Boost found! Version: ${Boost_VERSION}")
    message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
endif()

if(HDF5_FOUND)
    message(STATUS "HDF5 found! Version: ${HDF5_VERSION}")
    message(STATUS "HDF5 include dirs: ${HDF5_INCLUDE_DIRS}")
    message(STATUS "HDF5 libraries: ${HDF5_LIBRARIES}")
endif()

# Add game source directory to include path
include_directories(/game/)

# Game source files from uebung10 - INCLUDE ACTUAL .CPP FILES
set(GAME_SOURCES
    # Headers
    game/include/Actor.hpp
    game/include/ActorForces.hpp
    game/include/AnimatedRenderable.hpp
    game/include/Camera.hpp
    game/include/LayerManager.hpp
    game/include/Level.hpp
    game/include/LevelForces.hpp
    game/include/LevelParser.hpp
    game/include/MainWindow.hpp
    game/include/MovingRenderable.hpp
    game/include/Physics.hpp
    game/include/SDLRenderable.hpp
    game/include/TileArray.hpp
    game/include/TileSet.hpp
    game/include/Util.hpp
    game/io/BaseHdf5IO.hpp
    game/io/Hdf5Util.hpp
    game/io/TextureIO.hpp
    game/io/TileSetIO.hpp

    # Source files - ADD THESE
    game/src/Actor.cpp
    game/src/ActorForces.cpp
    game/src/AnimatedRenderable.cpp
    game/src/Camera.cpp
    game/src/LayerManager.cpp
    game/src/Level.cpp
    game/src/LevelForces.cpp
    game/src/LevelParser.cpp
    game/src/MainWindow.cpp
    game/src/MovingRenderable.cpp
    game/src/Physics.cpp
    game/src/SDLRenderable.cpp
    game/src/TileArray.cpp
    game/src/TileSet.cpp
    game/src/Util.cpp
)

set(CMAKE_AUTOMOC ON) # added
set(CMAKE_AUTORCC ON) # added
qt_standard_project_setup(REQUIRES 6.8)

# Add CMake module path for SDL2
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Find SDL2 libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_IMAGE REQUIRED)

# Find and add HighFive
set(HIGHFIVE_EXAMPLES FALSE CACHE INTERNAL "")
set(HIGHFIVE_UNIT_TESTS FALSE CACHE INTERNAL "")
add_subdirectory(ext/HighFive)
include_directories("ext/HighFive/include")
find_package(HDF5 REQUIRED COMPONENTS C CXX HL)
include_directories(${HDF5_INCLUDE_DIRS})

# Find required boost libraries
find_package(Boost 1.54 COMPONENTS filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# Add SDL2 include directory to search path
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2_IMG_INCLUDE_DIR})

# Add game source directory to include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/game)

# Game source files from uebung10
set(GAME_SOURCES
    game/src/Actor.cpp
    game/src/ActorForces.cpp
    game/src/AnimatedRenderable.cpp
    game/src/Camera.cpp
    game/src/LayerManager.cpp
    game/src/Level.cpp
    game/src/LevelForces.cpp
    game/src/LevelParser.cpp
    game/src/MainWindow.cpp
    game/src/MovingRenderable.cpp
    game/src/Physics.cpp
    game/src/SDLRenderable.cpp
    game/src/TileArray.cpp
    game/src/TileSet.cpp
    game/src/Util.cpp
)

qt_add_executable(appCloudgate_game
    src/main.cpp
    res/assets.qrc
    ${GAME_SOURCES}
)

qt_add_qml_module(appCloudgate_game
    URI Cloudgate_game
    VERSION 1.0

    QML_FILES
        qml/Main.qml
        qml/StyledButton.qml
        qml/SecondPage.qml
        qml/LevelEditor.qml

    SOURCES
        include/pages.h
        src/pages.cpp
        src/LevelEditorController.cpp
        include/LevelEditorController.hpp
        include/TilesetPalette.hpp
        src/TilesetPalette.cpp
        include/LevelCanvas.hpp
        src/LevelCanvas.cpp
        include/GameView.hpp
        src/GameView.cpp
        include/GameController.hpp
        src/GameController.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appCloudgate_game PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Include SDL2 directories
target_include_directories(appCloudgate_game PRIVATE
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMG_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
    ${HDF5_INCLUDE_DIRS}
)

target_link_libraries(appCloudgate_game
    PRIVATE Qt6::Quick
    ${SDL2_LIBRARY}
    ${SDL2_IMG_LIBRARY}
    ${Boost_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${HDF5_HL_LIBRARIES}
    Qt6::Widgets
)

# On Windows, copy SDL2.dll and SDL2_image.dll to output directory
if(WIN32)
    # Find SDL2.dll
    get_filename_component(SDL2_LIB_DIR ${SDL2_LIBRARY} DIRECTORY)
    find_file(SDL2_DLL
        NAMES SDL2.dll
        HINTS ${SDL2_LIB_DIR}/../bin ${SDL2_LIB_DIR}
        NO_DEFAULT_PATH
    )

    # Find SDL2_image.dll
    get_filename_component(SDL2_IMAGE_LIB_DIR ${SDL2_IMAGE_LIBRARY} DIRECTORY)
    find_file(SDL2_IMAGE_DLL
        NAMES SDL2_image.dll
        HINTS ${SDL2_IMAGE_LIB_DIR}/../bin ${SDL2_IMAGE_LIB_DIR}
        NO_DEFAULT_PATH
    )

    if(SDL2_DLL)
        message(STATUS "SDL2 DLL found at: ${SDL2_DLL}")
        add_custom_command(TARGET appCloudgate_game POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${SDL2_DLL}
                $<TARGET_FILE_DIR:appCloudgate_game>
            COMMENT "Copying SDL2.dll to output directory"
        )
    else()
        message(WARNING "SDL2.dll not found - you may need to copy it manually")
    endif()

    if(SDL2_IMAGE_DLL)
        message(STATUS "SDL2_image DLL found at: ${SDL2_IMAGE_DLL}")
        add_custom_command(TARGET appCloudgate_game POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${SDL2_IMAGE_DLL}
                $<TARGET_FILE_DIR:appCloudgate_game>
            COMMENT "Copying SDL2_image.dll to output directory"
        )
    else()
        message(WARNING "SDL2_image.dll not found - you may need to copy it manually")
    endif()
endif()

include(GNUInstallDirs)
install(TARGETS appCloudgate_game
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
