import QtQuick
import QtQuick.Controls
import Cloudgate_game 1.0

Page {
    id: secondPage
    
    // start game with level selected
    function initLevel(file) {
        console.log("SecondPage initLevel called:", file)
        gameView.levelPath = file
        gameView.startGame()
        forceActiveFocus()
    }

    // GameView fills the entire window
    GameView {
        id: gameView
        anchors.fill: parent
        z: 0
        focus: true

        // Called when game starts
        onGameStarted: {
            console.log("Game started")
            forceActiveFocus()
        }

        // Called when game stops
        onGameStopped: {
            console.log("Game stopped")
        }

        // Called when game over (all hearts lost or player fell)
        onGameOver: {
            gameView.stopGame()
            stackView.pop()
            myMain.page_value = -1
        }

        // ESC key handler
        Keys.onPressed: function (event) {
            if (event.key === Qt.Key_Escape) {
                console.log("ESC pressed - StartPage")
                gameView.stopGame()
                stackView.pop()
                myMain.page_value = -1
            }
        }
    }
}
