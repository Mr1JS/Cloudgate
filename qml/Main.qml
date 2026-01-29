import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import Cloudgate_game 1.0

Window {
    id: window
    minimumHeight: 800
    minimumWidth: 638
    maximumHeight: 800
    maximumWidth: 638
    visible: true
    title: qsTr("Hello World")

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: mainPage
    }

    Component {
        id: mainPage
        Page {
            Rectangle {
                anchors.fill: parent

                Image {
                    anchors.fill: parent
                    source: "qrc:/resources/images/test_image.jpg"
                    fillMode: Image.PreserveAspectCrop
                    smooth: true
                }

                Column {
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 20
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 10

                    StyledButton {
                        text: "Start"
                        onClicked: {
                            // Wechsle zur Spiel-Seite
                            stackView.push(Qt.resolvedUrl("SecondPage.qml"))
                            myMain.page_value = 0
                        }
                    }
                    StyledButton {
                        text: "LevelEditor"
                        onClicked: {
                            stackView.push(Qt.resolvedUrl("LevelEditor.qml"))
                            myMain.page_value = 1
                        }
                    }
                    StyledButton {
                        text: "Setting"
                        onClicked: {
                            stackView.push(Qt.resolvedUrl("SettingsPage.qml"))
                            myMain.page_value = 2
                        }
                    }
                }
            }
        }
    }
}
