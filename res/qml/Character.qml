import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Cloudgate_game 1.0

Page {
    id: characterPage

    // Enable focus for keyboard input
    focus: true

    ActorSelector {
        id: actorSelector
    }

    // properties for Connection 
    property url currentActorImage: ""
    property string currentActorName: ""

    background: Rectangle {
        color: "#f0f0f0"
    }

    // ESC key handler
    Keys.onPressed: function (event) {
        if (event.key === Qt.Key_Escape) {
            console.log("ESC pressed - CharacterPage")
            stackView.pop()
            myMain.page_value = -1
            event.accepted = true
        }
    }

    // Header with title and back button
    header: Rectangle {
        width: parent.width
        height: 60
        color: "white"

        // Back button
        Button {
            id: backButton
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 10
            text: "Back"

            onClicked: {
                actorSelector.updateAllActorFiles(currentActorName)
                stackView.pop()
                myMain.page_value = -1
            }
        }

        // Title text
        Text {
            text: "Select Character"
            font.pixelSize: 28
            font.bold: true
            anchors.centerIn: parent
        }
        
    }
    // main part with image, buttons and name
    Item {
        id: container
        anchors.centerIn: parent
        Rectangle {
                id: actorBackground
                width: 320    
                height: 320
                color: "#e2e2e2"
                anchors.centerIn: parent

            Image {
                id: actorImage
                width: 300       
                height: 300
                fillMode: Image.PreserveAspectFit
                anchors.centerIn: parent
                source: currentActorImage
            }
            Text {
                id: actorNameText
                text: actorSelector.currentActorName
                font.pixelSize: 20
                font.bold: true
                color: "#333333"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: actorImage.bottom
                anchors.topMargin: 10
            }
        }

        // left arrow
        Button {
            text: "<"
            width: 50
            height: 50
            anchors.verticalCenter: actorBackground.verticalCenter
            anchors.right: actorBackground.left
            anchors.rightMargin: 20
            onClicked: actorSelector.prev()

            background: Rectangle {
                color: "#5c5c5c"   
                radius: 5          
                border.color: "#272727"
                border.width: 2
            }
        }

        // right arrow
        Button {
            text: ">"
            width: 50
            height: 50
            anchors.verticalCenter: actorBackground.verticalCenter
            anchors.left: actorBackground.right
            anchors.leftMargin: 20
            onClicked: actorSelector.next()

            background: Rectangle {
                color: "#5c5c5c"    
                radius: 5          
                border.color: "#272727"
                border.width: 2
            }
        }
    }
    // connect ActorPath and ActorName with qml
    Connections {
        target: actorSelector
        function onCurrentActorChanged() {
            currentActorImage = actorSelector.currentActor
            currentActorName = actorSelector.currentActorName
        }
    }

    // Set focus when page loads
    Component.onCompleted: {
        characterPage.forceActiveFocus()
        actorSelector.setStartActor()
    }
}
