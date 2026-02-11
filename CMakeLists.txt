cmake_minimum_required(VERSION 3.5)

project(Cloudgate_game VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OPTIONAL: Setup C++ compiler flags for debug mode
# set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")

# Set CMake policies to suppress warnings
cmake_policy(SET CMP0071 NEW)  # Let AUTOMOC and AUTOUIC process GENERATED files

# Add cmake modules path for finding SDL2
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Enable Qt's auto features
set(CMAKE_AUTOMOC ON) # added
set(CMAKE_AUTORCC ON) # added

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
set(HIGHFIVE_EXAMPLES FALSE CACHE INTERNAL "")
set(HIGHFIVE_UNIT_TESTS FALSE CACHE INTERNAL "")
add_subdirectory(ext/HighFive)

string (REGEX MATCH "\\arch[1-9]-[1-9]" arch_version_suffix ${CMAKE_SYSTEM})

# Box2D Physics Engine (System-Paket: sudo apt install libbox2d-dev)
if(APPLE OR arch_version_suffix)
    # apparenty arch uses the C libraries for box2d, so we need to manually download the correct lib for arch
    include(FetchContent)

    # ---- Build dependencies static ----
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)

    # ---- Box2D 2.4.x: disable extras that cause trouble (unit tests, samples) ----
    set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
    set(BOX2D_BUILD_TESTBED    OFF CACHE BOOL "" FORCE)
    set(BOX2D_BUILD_DOCS       OFF CACHE BOOL "" FORCE)
    # (if contained in version) set(BOX2D_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        box2d
        GIT_REPOSITORY https://github.com/erincatto/box2d.git
        GIT_TAG v2.4.1
    )
    FetchContent_MakeAvailable(box2d)


    if(NOT TARGET box2d::box2d)
        add_library(box2d::box2d ALIAS box2d)
    endif()
else()
    find_package(box2d CONFIG REQUIRED)
endif()

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

if(box2d_FOUND)
    message(STATUS "Box2D found (System-Paket libbox2d-dev)")
endif()

# Game source files from uebung10 - INCLUDE ACTUAL .CPP FILES
set(GAME_SOURCES
    # Headers
    game/include/Actor.hpp
    game/include/ActorForces.hpp
    game/include/AnimatedRenderable.hpp
    game/include/Camera.hpp
    game/include/TimerDigit.hpp
    game/include/LayerManager.hpp
    game/include/Level.hpp
    game/include/LevelForces.hpp
    game/include/LevelParser.hpp
    game/include/MainWindow.hpp
    game/include/Monster.hpp
    game/include/MovingRenderable.hpp
    game/include/Physics.hpp
    game/include/SDLRenderable.hpp
    game/include/StateController.hpp
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
    game/src/TimerDigit.cpp
    game/src/LayerManager.cpp
    game/src/Level.cpp
    game/src/LevelForces.cpp
    game/src/LevelParser.cpp
    game/src/MainWindow.cpp
    game/src/Monster.cpp
    game/src/MovingRenderable.cpp
    game/src/Physics.cpp
    game/src/SDLRenderable.cpp
    game/src/StateController.cpp
    game/src/TileArray.cpp
    game/src/TileSet.cpp
    game/src/Util.cpp
)

qt_standard_project_setup(REQUIRES 6.8)

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
        qml/Character.qml
        qml/LevelSelector.qml

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
        include/LevelSelector.hpp
        src/LevelSelector.cpp
        include/ActorSelector.hpp
        src/ActorSelector.cpp
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

# Include directories - use the actual variable names from the Find modules
target_include_directories(appCloudgate_game PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/game
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMG_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
    ${HDF5_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/HighFive/include
)

# Link libraries - use the actual variable names from the Find modules
target_link_libraries(appCloudgate_game PRIVATE
    Qt6::Quick
    Qt6::Widgets
    ${SDL2_LIBRARY}
    ${SDL2_IMG_LIBRARY}
    ${Boost_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${HDF5_HL_LIBRARIES}
    box2d::box2d
)

# Windows DLL copying
if(WIN32)
    # Find SDL2.dll
    get_filename_component(SDL2_LIB_DIR ${SDL2_LIBRARY} DIRECTORY)
    find_file(SDL2_DLL
        NAMES SDL2.dll
        HINTS ${SDL2_LIB_DIR}/../bin ${SDL2_LIB_DIR}
        NO_DEFAULT_PATH
    )

    # Find SDL2_image.dll - use SDL2_IMG_LIBRARY variable
    get_filename_component(SDL2_IMAGE_LIB_DIR ${SDL2_IMG_LIBRARY} DIRECTORY)
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

# ============================================================================
# Doxygen Documentation
# ============================================================================
option(BUILD_DOC "Build documentation with Doxygen" ON)

if(BUILD_DOC)
    find_package(Doxygen OPTIONAL_COMPONENTS dot)
    
    if(DOXYGEN_FOUND)
        # Set input directories for documentation
        set(DOXYGEN_INPUT_DIR "\"${CMAKE_CURRENT_SOURCE_DIR}/game\" \"${CMAKE_CURRENT_SOURCE_DIR}/include\" \"${CMAKE_CURRENT_SOURCE_DIR}/src\" \"${CMAKE_CURRENT_SOURCE_DIR}/qml\"")
        set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs")
        
        # Configure Doxyfile with project variables
        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
            ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            @ONLY
        )
        
        # Add custom target to generate documentation
        add_custom_target(doc
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
        
        message(STATUS "Doxygen found! Generate docs with: cmake --build . --target doc")
        message(STATUS "Documentation will be in: ${DOXYGEN_OUTPUT_DIR}/html/index.html")
        
        if(DOXYGEN_DOT_FOUND)
            message(STATUS "Graphviz/dot found - class diagrams and call graphs will be generated")
        else()
            if(WIN32)
                message(STATUS "Graphviz/dot not found - install with: choco install graphviz")
            else()
                message(STATUS "Graphviz/dot not found - install with: sudo apt install graphviz")
            endif()
        endif()
    else()
        if(WIN32)
            message(WARNING "Doxygen not found - install with: choco install doxygen.install")
        else()
            message(WARNING "Doxygen not found - install with: sudo apt install doxygen graphviz")
        endif()
    endif()
endif()
