import QtQuick
import QtQuick.Controls
import Cloudgate_game 1.0

Page {
    id: secondPage

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
        }
    }
}
