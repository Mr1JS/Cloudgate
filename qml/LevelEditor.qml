import QtQuick
import QtQuick.Controls
import Cloudgate_game 1.0

Page {
    id: levelEditorPage

    // Enable focus for keyboard input
    focus: true

    // ESC key handler
    Keys.onPressed: function (event) {
        if (event.key === Qt.Key_Escape) {
            console.log("ESC pressed - LevelEditor")
            stackView.pop()
            event.accepted = true
        }
    }

    // The "brain" - Controller coordinates everything
    LevelEditorController {
        id: editorController

        onLevelSaved: function (path) {
            statusText.text = "Level saved: " + path
        }

        onLevelLoaded: function (path) {
            statusText.text = "Level loaded: " + path
        }

        onLevelCleared: {
            statusText.text = "Level cleared"
        }

        onSelectedTileChanged: {
            statusText.text = "Tile " + selectedTileIndex + " selected"
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#5c82a1" // RGB 92, 130, 161

        Image {
            id: editorBackground
            anchors.fill: parent
            source: "/resources/images/clouds2.png"
            fillMode: Image.PreserveAspectCrop
            opacity: 0.3
        }

        // Top bar with buttons
        Rectangle {
            id: topBar
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 60
            color: "#404040"
            opacity: 0.9

            // Check if there's enough space for all buttons
            property bool showAllButtons: width > 700

            Row {
                anchors.centerIn: parent
                spacing: 20
                visible: topBar.showAllButtons

                Button {
                    text: "Back"
                    onClicked: stackView.pop()
                }

                Button {
                    text: "Clear Level"
                    onClicked: editorController.clearLevel()
                }

                Button {
                    text: "Save"
                    onClicked: editorController.saveLevel()
                }

                Button {
                    text: "Load"
                    onClicked: editorController.loadLevel()
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Select a tile on the left and place it on the right"
                    color: "white"
                    font.pixelSize: 14
                }
            }

            // Compact version with dropdown menu
            Row {
                anchors.centerIn: parent
                spacing: 15
                visible: !topBar.showAllButtons

                Button {
                    text: "Back"
                    onClicked: stackView.pop()
                }

                Button {
                    id: menuButton
                    text: "Menu ▼"
                    onClicked: dropdownMenu.open()

                    Menu {
                        id: dropdownMenu
                        y: menuButton.height

                        MenuItem {
                            text: "Clear Level"
                            onTriggered: editorController.clearLevel()
                        }
                        MenuItem {
                            text: "Save"
                            onTriggered: editorController.saveLevel()
                        }
                        MenuItem {
                            text: "Load"
                            onTriggered: editorController.loadLevel()
                        }
                    }
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Tile Editor"
                    color: "white"
                    font.pixelSize: 14
                }
            }
        }

        // Container for both areas
        Item {
            id: mainContainer
            anchors.top: topBar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10

            // Left sidebar - Tileset palette with ScrollView
            Rectangle {
                id: leftSidebar
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: Math.min(parent.width * 0.25, 300)
                color: "#606060"
                opacity: 0.95
                radius: 8

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 5
                    color: "#707070"
                    radius: 5

                    Column {
                        anchors.fill: parent
                        spacing: 5

                        // Switch
                        Item {
                            width: parent.width
                            height: toggleTileSets.implicitHeight + 2

                            Switch {
                                id: toggleTileSets
                                //anchors.top: parent.top
                                anchors.left: parent.left
                                anchors.topMargin: 2
                                anchors.leftMargin: 2

                                indicator: Rectangle {
                                    implicitWidth: 45
                                    implicitHeight: 17
                                    radius: height / 2
                                    color: "#b0b0b0"

                                    Rectangle {
                                        width: parent.height - 4
                                        height: parent.height - 4
                                        radius: width / 2
                                        y: 2
                                        x: toggleTileSets.checked ? parent.width - width - 2 : 2
                                        color: "#f5f5f5"

                                        Behavior on x {
                                            NumberAnimation {
                                                duration: 120
                                                easing.type: Easing.InOutQuad
                                            }
                                        }
                                    }
                                }

                                onCheckedChanged: {
                                    editorController.toggleExtraTileset(checked)
                                    console.log("Tileset switched:",
                                                checked ? "2x1" : "1x1")
                                }
                            }
                        }

                        Flickable {
                            id: tilesetFlickable
                            //anchors.fill: parent
                            anchors.margins: 5
                            clip: true
                            width: parent.width
                            height: parent.height - (toggleTileSets.implicitHeight
                                                     + 2) - parent.spacing
                            contentWidth: tilesetPalette.width
                            contentHeight: tilesetPalette.calculatedHeight

                            boundsBehavior: Flickable.StopAtBounds

                            ScrollBar.vertical: ScrollBar {
                                policy: ScrollBar.AsNeeded
                                anchors.right: parent.right
                                anchors.rightMargin: 5
                            }

                            TilesetPalette {
                                id: tilesetPalette
                                width: calculatedWidth
                                height: calculatedHeight

                                property int calculatedWidth: {
                                    if (tileCount === 0)
                                        return tilesetFlickable.width - 8

                                    var tileSize = 32 + 8 // tile + spacing
                                    var availableWidth = tilesetFlickable.width - 8
                                    var columns = Math.max(
                                                1, Math.floor(
                                                    availableWidth / tileSize))

                                    var actualWidth = columns * tileSize + 6

                                    return Math.min(actualWidth, availableWidth)
                                }

                                property int calculatedHeight: {
                                    if (tileCount === 0)
                                        return 100

                                    var tileSize = 32 + 8
                                    var availableWidth = tilesetFlickable.width - 8
                                    var columns = Math.max(
                                                1, Math.floor(
                                                    availableWidth / tileSize))
                                    var rows = Math.ceil(tileCount / columns)
                                    var totalHeight = rows * tileSize + 12

                                    return totalHeight
                                }
                            }
                        }
                    }
                }

                Text {
                    id: statusText
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottomMargin: 10
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    text: "Select a tile"
                    color: "white"
                    font.pixelSize: 12
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            // Right canvas - Level editor with ScrollView
            Rectangle {
                id: rightCanvas
                anchors.left: leftSidebar.right
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.leftMargin: 10

                color: "#808080"
                opacity: 0.95
                radius: 8

                Rectangle {
                    id: canvasBackground
                    anchors.fill: parent
                    anchors.margins: 5
                    color: "white"
                    radius: 5

                    Flickable {
                        id: canvasFlickable
                        anchors.fill: parent
                        anchors.margins: 5
                        clip: true

                        contentWidth: levelCanvas.width
                        contentHeight: levelCanvas.height

                        boundsBehavior: Flickable.StopAtBounds

                        ScrollBar.horizontal: ScrollBar {
                            policy: ScrollBar.AsNeeded
                        }
                        ScrollBar.vertical: ScrollBar {
                            policy: ScrollBar.AsNeeded
                        }

                        LevelCanvas {
                            id: levelCanvas
                            x: Math.max(0, (canvasFlickable.width - width) / 2)
                            y: Math.max(0,
                                        (canvasFlickable.height - height) / 2)
                            width: 640 // 20 tiles * 32px
                            height: 800 // 25 tiles * 32px
                            gridWidth: 20
                            gridHeight: 25
                        }
                    }

                    Text {
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottomMargin: 5
                        text: "Scroll to see the entire level | Right-click to delete tiles"
                        color: "#666666"
                        font.pixelSize: 10
                        visible: levelCanvas.width > canvasFlickable.width
                                 || levelCanvas.height > canvasFlickable.height
                    }
                }
            }
        }
    }

    // Initialize AFTER all components are created
    Component.onCompleted: {
        // Register components with controller
        editorController.registerPalette(tilesetPalette)
        editorController.registerCanvas(levelCanvas)

        // Load tileset through controller
        editorController.loadTileset(":/resources/images/tileset2.png", 32,
                                     32, 4, 105)

        // Set focus when page loads
        levelEditorPage.forceActiveFocus()
    }
}
