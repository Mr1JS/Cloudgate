import QtQuick
import QtQuick.Controls
import Cloudgate_game 1.0

Page {
    id: endlessPage
    property bool showFallback: false
    property int completedLevels: 1

    Component.onCompleted: {
        // Kurz warten, bis Layout und GameView bereit sind
        startTimer.start()
    }

    Timer {
        id: startTimer
        interval: 150
        repeat: false
        onTriggered: startEndlessGame()
    }

    function startEndlessGame() {
        var gen = endlessGenerator
        if (!gen) {
            statusText.text = "Generator nicht bereit"
            return
        }
        var path = gen.generateLevel(completedLevels)
        if (path) {
            gameView.levelPath = path
            gameView.startGame()
            showFallback = false
        } else {
            statusText.text = "Level konnte nicht erstellt werden. Klicke auf 'Spiel starten'."
            showFallback = true
        }
        forceActiveFocus()
    }

    EndlessLevelGenerator {
        id: endlessGenerator
    }

    GameView {
        id: gameView
        anchors.fill: parent
        z: 0
        focus: true

        onGameStarted: forceActiveFocus()

        onLevelFinished: {
            completedLevels++
            gameView.stopGame()
            startTimer.restart()
        }

        onGameOver: {
            gameView.stopGame()
            stackView.pop()
            myMain.page_value = -1
        }

        Keys.onPressed: function (event) {
            if (event.key === Qt.Key_Escape) {
                gameView.stopGame()
                stackView.pop()
                myMain.page_value = -1
            }
        }
    }

    // Back-Button
    Button {
        text: "Back"
        height: 40
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 16
        anchors.leftMargin: 16
        z: 10
        onClicked: {
            gameView.stopGame()
            stackView.pop()
            myMain.page_value = -1
        }
    }

    Text {
        id: titleText
        text: "Endless Mode"
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 25
        font.pixelSize: 24
        font.bold: true
        color: "#ffffff"
        z: 10
    }

    Text {
        id: levelCounter
        text: "Levels: " + completedLevels
        anchors.top: titleText.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 8
        font.pixelSize: 18
        color: "#ffffff"
        z: 10
    }

    Text {
        id: levelTypeText
        text: endlessGenerator.lastLevelType
        anchors.top: levelCounter.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 4
        font.pixelSize: 14
        color: "#cccccc"
        z: 10
    }

    Column {
        id: fallbackColumn
        anchors.centerIn: parent
        z: 10
        spacing: 16
        visible: endlessPage.showFallback

        Text {
            id: statusText
            font.pixelSize: 16
            color: "#ffffff"
            horizontalAlignment: Text.AlignHCenter
            width: endlessPage.width - 40
        }

        Button {
            id: startButton
            text: "Spiel starten"
            anchors.horizontalCenter: parent.horizontalCenter
            visible: false
            onClicked: startEndlessGame()
        }
    }
}
