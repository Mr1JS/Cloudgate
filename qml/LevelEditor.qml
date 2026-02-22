import QtQuick
import QtQuick.Controls
import Cloudgate_game 1.0

Page {
    id: levelEditorPage

    // Enable focus for keyboard input
    focus: true

    // load font for the new Icons
    FontLoader {
        id: fontAwesome
        source: "qrc:/resources/fonts/Font-Solid-900.otf"
    }

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
            property bool showAllButtons: width > 1000

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
                    text: "- 5 Tile above"
                    enabled: levelCanvas.gridHeight > 25
                    onClicked: editorController.removeRowsAbove(5)
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

                        MenuItem {
                            text: "- 5 Tile above"
                            enabled: levelCanvas.gridHeight > 25
                            onClicked: editorController.removeRowsAbove(5)
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

            // Right side - minimal controls
            Row {
                anchors.right: parent.right
                anchors.rightMargin: 30
                anchors.verticalCenter: parent.verticalCenter
                spacing: 30

                // Scroll Speed
                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 3

                    Text {
                        id: text1
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "SCROLL SPEED"
                        color: "#FFFFFF"
                        font.pixelSize: 14
                        font.letterSpacing: 1
                        font.bold: true
                    }

                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 6

                        Slider {
                            id: scrollSpeed
                            objectName: "scrollSpeed"
                            // y: 31
                            width: 76
                            height: 13
                            value: 4
                            transformOrigin: Item.Center
                            snapMode: RangeSlider.SnapAlways
                            stepSize: 1
                            to: 10
                            from: 1

                            background: Rectangle {
                                x: scrollSpeed.leftPadding
                                y: scrollSpeed.topPadding + scrollSpeed.availableHeight / 2 - height / 2
                                width: scrollSpeed.availableWidth
                                height: 3
                                radius: 2
                                color: "#404040"

                                Rectangle {
                                    width: scrollSpeed.visualPosition * parent.width
                                    height: parent.height
                                    color: "#4a9eff"
                                    radius: 2
                                }
                            }

                            handle: Rectangle {
                                x: scrollSpeed.leftPadding + scrollSpeed.visualPosition * (scrollSpeed.availableWidth - width)
                                y: scrollSpeed.topPadding + scrollSpeed.availableHeight / 2 - height / 2
                                width: 14
                                height: 14
                                radius: 7
                                color: scrollSpeed.pressed ? "#3a8eef" : "#4a9eff"
                                border.color: "#2a7edf"
                                border.width: 2
                            }
                        }

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: scrollSpeed.value
                            color: "#e0e0e0"
                            font.pixelSize: 11
                            font.bold: true
                            width: 15
                        }
                    }
                }

                Rectangle {
                    width: 2
                    height: 40
                    color: "#404040"
                    anchors.verticalCenter: parent.verticalCenter
                }

                // Win Condition
                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 2

                    Text {
                        id: text2
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "WIN CONDITION"
                        color: "#909090"
                        font.pixelSize: 9
                        font.bold: true
                    }

                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 30

                        // None button
                        Column {
                            spacing: 3

                            Button {
                                id: none
                                objectName: "buttonNone"
                                width: 25
                                height: 25
                                anchors.horizontalCenter: parent.horizontalCenter
                                checkable: true
                                checked: true
                                ButtonGroup.group: buttonGroup

                                contentItem: Text {
                                    text: "\uf05e"
                                    font.family: fontAwesome.name
                                    font.pixelSize: 14
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                    color: parent.checked ? "#4a9eff" : "#606060"
                                }

                                background: Rectangle {
                                    color: parent.checked ? "#1a1a1a" : "#2a2a2a"
                                    radius: 6
                                    border.color: parent.checked ? "#4a9eff" : "#404040"
                                    border.width: 2
                                }
                            }

                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: "None"
                                color: none.checked ? "#4a9eff" : "#606060"
                                font.pixelSize: 8
                                font.bold: true
                            }
                        }

                        // Coins
                        Column {
                            spacing: 3

                            Button {
                                id: coins
                                objectName: "buttonCoins"
                                width: 25
                                height: 25
                                anchors.horizontalCenter: parent.horizontalCenter
                                checkable: true
                                ButtonGroup.group: buttonGroup

                                contentItem: Text {
                                    text: "\uf51e"
                                    font.family: fontAwesome.name
                                    font.pixelSize: 14
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                    color: parent.checked ? "#ffd700" : "#606060"
                                }

                                background: Rectangle {
                                    color: parent.checked ? "#1a1a1a" : "#2a2a2a"
                                    radius: 6
                                    border.color: parent.checked ? "#ffd700" : "#404040"
                                    border.width: 2
                                }
                            }

                            Row {
                                anchors.horizontalCenter: parent.horizontalCenter
                                spacing: 3

                                Rectangle {
                                    width: 25
                                    height: 13
                                    anchors.verticalCenter: parent.verticalCenter
                                    color: "#1a1a1a"
                                    radius: 3
                                    border.color: coins.checked ? "#ffd700" : "#404040"
                                    border.width: 1
                                    opacity: coins.checked ? 1.0 : 0.4

                                    TextInput {
                                        id: coinInput
                                        objectName: "coinInput"
                                        text: "99"
                                        anchors.centerIn: parent
                                        font.pixelSize: 10
                                        font.bold: true
                                        color: "#e0e0e0"
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                        inputMask: "00"
                                        selectByMouse: true
                                        enabled: coins.checked
                                    }
                                }

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: "coins"
                                    color: coins.checked ? "#FFFFFF" : "#909090"
                                    font.pixelSize: 8
                                }
                            }
                        }

                        // Timer
                        Column {
                            spacing: 3

                            Button {
                                id: timed
                                objectName: "buttonTime"
                                width: 25
                                height: 25
                                anchors.horizontalCenter: parent.horizontalCenter
                                checkable: true
                                ButtonGroup.group: buttonGroup

                                contentItem: Text {
                                    text: "\uf017"
                                    font.family: fontAwesome.name
                                    font.pixelSize: 14
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                    color: parent.checked ? "#ff6b6b" : "#606060"
                                }

                                background: Rectangle {
                                    color: parent.checked ? "#1a1a1a" : "#2a2a2a"
                                    radius: 6
                                    border.color: parent.checked ? "#ff6b6b" : "#404040"
                                    border.width: 2
                                }
                            }

                            Row {
                                anchors.horizontalCenter: parent.horizontalCenter
                                spacing: 3

                                Rectangle {
                                    width: 25
                                    height: 13
                                    anchors.verticalCenter: parent.verticalCenter
                                    color: "#1a1a1a"
                                    radius: 3
                                    border.color: timed.checked ? "#ff6b6b" : "#404040"
                                    border.width: 1
                                    opacity: timed.checked ? 1.0 : 0.4

                                    TextInput {
                                        id: timeInput
                                        objectName: "timeInput"
                                        text: "999"
                                        anchors.centerIn: parent
                                        font.pixelSize: 10
                                        font.bold: true
                                        color: "#e0e0e0"
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                        inputMask: "000"
                                        selectByMouse: true
                                        enabled: timed.checked
                                    }
                                }

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: "sec"
                                    color: timed.checked ? "#FFFFFF" : "#909090"
                                    font.pixelSize: 8
                                }
                            }
                        }
                    }
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
                        opacity: 0.5
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
