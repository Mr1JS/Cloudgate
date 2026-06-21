/**
 * @file Main.qml
 * @brief Main application window and navigation controller for the game,
 *        manages page switching between menu, character selection, level editor and gameplay
 */

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
    title: qsTr("Cloudgate")

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: mainPage
        property var mainPageInstance: null

        Component.onCompleted: {
            mainPageInstance = stackView.currentItem
        }
        // check if page turns back to main page and revert the size to standard
        onCurrentItemChanged: {
                if (stackView.currentItem === mainPageInstance) {

                    window.minimumWidth = 638
                    window.minimumHeight = 800
                    window.maximumWidth = 638
                    window.maximumHeight = 800
                }
            }
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
                            stackView.push(Qt.resolvedUrl("LevelSelector.qml"))
                            myMain.page_value = 0
                        }
                    }
                    StyledButton {
                        text: "Endless"
                        onClicked: {
                            stackView.push(Qt.resolvedUrl("EndlessPage.qml"))
                            myMain.page_value = 0
                        }
                    }
                    StyledButton {
                        text: "LevelEditor"
                        onClicked: {
                            // change size for Leveleditor
                            window.minimumWidth = 925
                            window.minimumHeight = 900
                            window.maximumWidth = 925
                            window.maximumHeight = 900
                            stackView.push(Qt.resolvedUrl("LevelEditor.qml"))
                            myMain.page_value = 1
                        }
                    }
                    StyledButton {
                        text: "Characters"
                        onClicked: {
                            stackView.push(Qt.resolvedUrl("Character.qml"))
                            myMain.page_value = 2
                        }
                    }
                }
            }
        }
    }
}
