import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: settingsPage

    // Enable focus for keyboard input
    focus: true

    background: Rectangle {
        color: "#f0f0f0"
    }

    // ESC key handler
    Keys.onPressed: function (event) {
        if (event.key === Qt.Key_Escape) {
            console.log("ESC pressed - SettingsPage")
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
                console.log("Back button clicked")
                stackView.pop()
                myMain.page_value = -1
            }
        }

        // Title text
        Text {
            text: "Settings"
            font.pixelSize: 28
            font.bold: true
            anchors.centerIn: parent
        }
    }

    // Set focus when page loads
    Component.onCompleted: {
        settingsPage.forceActiveFocus()
    }
}
