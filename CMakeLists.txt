cmake_minimum_required(VERSION 3.5)

project(Cloudgate_game VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
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
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/game)

# Game source files from uebung10
set(GAME_SOURCES
    src/game/Actor.cpp
    src/game/ActorForces.cpp
    src/game/AnimatedRenderable.cpp
    src/game/Camera.cpp
    src/game/LayerManager.cpp
    src/game/Level.cpp
    src/game/LevelForces.cpp
    src/game/LevelParser.cpp
    src/game/MainWindow.cpp
    src/game/MovingRenderable.cpp
    src/game/Physics.cpp
    src/game/SDLRenderable.cpp
    src/game/TileArray.cpp
    src/game/TileSet.cpp
    src/game/Util.cpp
)

qt_add_executable(appCloudgate_game
    main.cpp
    GameController.cpp
    GameController.hpp
    GameView.cpp
    GameView.hpp
    ${GAME_SOURCES}
)

qt_add_qml_module(appCloudgate_game
    URI Cloudgate_game
    VERSION 1.0
    QML_FILES
        Main.qml
        StyledButton.qml
        SecondPage.qml
        LevelEditor.qml
    SOURCES
        pages.h
        pages.cpp
        LevelEditorController.cpp
        LevelEditorController.hpp
        TilesetPalette.hpp
        TilesetPalette.cpp
        LevelCanvas.hpp
        LevelCanvas.cpp
    RESOURCES
        assets.qrc
)

# Link libraries
target_link_libraries(appCloudgate_game
    PRIVATE Qt6::Quick
    ${SDL2_LIBRARY}
    ${SDL2_IMG_LIBRARY}
    m
    ${HDF5_LIBRARIES}
    ${HDF5_HL_LIBRARIES}
    ${Boost_LIBRARIES}
)

set_target_properties(appCloudgate_game PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS appCloudgate_game
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
