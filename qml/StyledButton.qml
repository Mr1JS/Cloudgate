/**
 * @file StyledButton.qml
 * @brief Custom styled button component with consistent appearance,
 *        reusable UI element for all menu buttons throughout the application
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

DelayButton {
    id: control

    width: 220
    height: 42

    font.pixelSize: 15
    font.bold: true

    background: Rectangle {
        radius: 8
        color: control.pressed ? "#b35f05" : control.hovered ? "#db9040" : "#c47f35"

        border.color: "#d19352"
        border.width: 1
    }

    contentItem: Text {
        text: control.text
        color: "white"
        font: control.font
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.fill: parent
    }
}
