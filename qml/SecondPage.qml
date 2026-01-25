import QtQuick
import QtQuick.Controls
<<<<<<< HEAD
import Cloudgate_game 1.0
=======
>>>>>>> 746825d15eebc7077e26ef8a65aa5e78b625d557

Page {
    id: secondPage

<<<<<<< HEAD
    // GameView füllt das gesamte Fenster
    GameView {
        id: gameView
        anchors.fill: parent
        z: 0
        focus: true

        onGameStarted: {
            console.log("Game started")
            forceActiveFocus()
        }

        onGameStopped: {
            console.log("Game stopped")
        }

        Component.onCompleted: {
            gameView.levelPath = "res/level_master.xml"
            forceActiveFocus()
            gameView.startGame()
        }

        Keys.onPressed: function (event) {
            // ESC-Taste führt direkt zurück zum Hauptmenü
            if (event.key === Qt.Key_Escape) {
                gameView.stopGame()
                stackView.pop()
                myMain.page_value = -1
            }
=======
    Rectangle {
        anchors.fill: parent
        color: "#3498db"

        Text {
            anchors.centerIn: parent
            text: "This is the second page!"
            font.pixelSize: 20
            color: "white"
        }

        Button {
            text: "Back"
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: 20
            onClicked: {
                stackView.pop()
                myMain.page_value = -1
            } // go back to previous page
>>>>>>> 746825d15eebc7077e26ef8a65aa5e78b625d557
        }
    }
}
