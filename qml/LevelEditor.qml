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

    ButtonGroup {
        id: buttonGroup
        exclusive: true
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

    property string backgroundImageLevel: "qrc:/resources/images/backgrounds/mountain.png"

    Rectangle {
        anchors.fill: parent
        color: "#5c82a1" // RGB 92, 130, 16

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
                    text: "+ 5 Tile above"
                    onClicked: editorController.addRowsAbove(5)
                }

                Button {
                    text: "Background"
                    onClicked: editorController.loadBackground()
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
                anchors.horizontalCenterOffset: -161
                anchors.verticalCenterOffset: 0

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
                        MenuItem {
                            text: "Background"
                            onTriggered: editorController.loadBackground()
                        }

                        MenuItem {
                            text: "+ 5 Tile above"
                            onClicked: editorController.addRowsAbove(5)
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

            Text {
                id: text1
                y: 8
                width: 82
                height: 17
                color: "#ffffff"
                text: qsTr("Scroll Speed")
                font.pixelSize: 14
                anchors.right: parent.right
                anchors.rightMargin: 247
            }

            Slider {
                id: scrollSpeed
                objectName: "scrollSpeed"
                y: 31
                width: 76
                height: 13
                value: 4
                transformOrigin: Item.Center
                snapMode: RangeSlider.SnapAlways
                stepSize: 1
                to: 10
                from: 1
                anchors.right: parent.right
                anchors.rightMargin: 253
            }

            Text {
                id: text2
                y: 23
                width: 82
                height: 17
                color: "#ffffff"
                text: qsTr("Win Condition")
                font.pixelSize: 14
                anchors.right: parent.right
                anchors.rightMargin: 136
            }

            Button {
                id: timed
                objectName: "buttonTime"
                y: 41
                width: 25
                height: 19
                text: qsTr("⏳")
                checkable: true
                ButtonGroup.group: buttonGroup
                background: Rectangle {
                    border.color: parent.checked ? "green" : "#404040"
                    border.width: 2
                    radius: 6
                }
                anchors.right: parent.right
                anchors.rightMargin: 90
            }

            Button {
                id: coins
                objectName: "buttonCoins"
                x: 525
                y: 20
                width: 25
                height: 20
                text: qsTr("🪙")
                checkable: true
                ButtonGroup.group: buttonGroup
                background: Rectangle {
                    border.color: parent.checked ? "green" : "#404040"
                    border.width: 2
                    radius: 6
                }
                anchors.right: parent.right
                anchors.rightMargin: 90
            }

            Button {
                id: none
                objectName: "buttonNone"
                x: 525
                y: 0
                width: 25
                height: 19
                text: qsTr("⌀")
                checkable: true
                ButtonGroup.group: buttonGroup
                background: Rectangle {
                    border.color: parent.checked ? "green" : "#404040"
                    border.width: 2
                    radius: 6
                }
                anchors.right: parent.right
                anchors.rightMargin: 90
                checked: true
            }
            Item {
                y: 44
                width: 24
                height: 13
                anchors.right: parent.right
                anchors.rightMargin: 60

                Rectangle {
                    anchors.fill: parent
                    anchors.rightMargin: -5
                    color: "white"
                    radius: 2
                    border.color: "gray"
                    border.width: 1
                }
                TextInput {
                    id: timeInput
                    objectName: "timeInput"
                    text: qsTr("999")
                    font.pixelSize: 11
                    horizontalAlignment: Text.AlignHCenter
                    inputMask: "000"
                    anchors.fill: parent
                    anchors.margins: 1
                    anchors.rightMargin: -5
                }
            }

            Item {
                x: 556
                y: 23
                width: 24
                height: 13
                anchors.right: parent.right
                anchors.rightMargin: 60
                Rectangle {
                    color: "#ffffff"
                    radius: 2
                    border.color: "#808080"
                    border.width: 1
                    anchors.fill: parent
                    anchors.rightMargin: -5
                }

                TextInput {
                    id: coinInput
                    objectName: "coinInput"
                    text: qsTr("99")
                    anchors.fill: parent
                    anchors.margins: 1
                    anchors.rightMargin: -5
                    font.pixelSize: 11
                    horizontalAlignment: Text.AlignHCenter
                    inputMask: "00"
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
                    Image {
                        anchors.fill: parent  
                        source: backgroundImageLevel    
                        fillMode: Image.PreserveAspectCrop 
                        // not 100% to see the tiles better
                        opacity: 0.75
                    }

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
                            onBackgroundChanged: function (path) 
                            {
                                // cut away "res" and replace with "qrc:/resources"
                                backgroundImageLevel = "qrc:/resources" + path.slice(3)
                            }
                        }
                    }
                    
                    Rectangle {
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottomMargin: 5
                        color: "white"
                        radius: 4
                        width: textItem.implicitWidth + 1
                        height: textItem.implicitHeight + 1
                        visible: levelCanvas.width > canvasFlickable.width
                            || levelCanvas.height > canvasFlickable.height
                        opacity: 0.75

                        Text {
                            id: textItem
                            anchors.centerIn: parent
                            text: "Scroll to see the entire level | Right-click to delete tiles"
                            color: "#020202"
                            font.pixelSize: 12
                        }
                    }
                }
            }
        }

        Text {
            id: text3
            x: 588
            y: 45
            width: 82
            height: 13
            color: "#ffffff"
            text: qsTr("seconds")
            anchors.right: parent.right
            anchors.rightMargin: -30
            font.pixelSize: 11
        }

        Text {
            id: text4
            x: 588
            y: 24
            width: 82
            height: 12
            color: "#ffffff"
            text: qsTr("coins")
            anchors.right: parent.right
            anchors.rightMargin: -30
            font.pixelSize: 11
        }
    }

    // Initialize AFTER all components are created
    Component.onCompleted: {
        // Register components with controller
        editorController.registerPalette(tilesetPalette)
        editorController.registerCanvas(levelCanvas)

        // Load tileset through controller
        editorController.loadTileset("res/level_master.xml")
        // to access slider and goal condition in levelCanvas
        levelCanvas.setQML(levelEditorPage)

        // Set focus when page loads
        levelEditorPage.forceActiveFocus()
    }
}
