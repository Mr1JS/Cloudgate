import QtQuick
import QtQuick.Controls

Page {
    id: secondPage

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
        }
    }
}
