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
    src/GameController.cpp
    include/GameController.hpp
    src/GameView.cpp
    include/GameView.hpp
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
    RESOURCES
        res/assets.qrc
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
