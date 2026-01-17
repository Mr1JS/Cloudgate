import QtQuick
import QtQuick.Controls
import Cloudgate_game

Page {
    id: leveleditor

    Rectangle {
        width: 800
        height: 1024

        Image {
            id: editor_background
            anchors.fill: parent
            source: "/resources/images/clouds2.png"
            fillMode: Image.PreserveAspectCrop
        }

        Rectangle {
            id: sidebar
            anchors.bottom: parent.bottom
            anchors.left: parent.left

            width: parent.width * 0.2
            height: parent.height * 0.9
            color: "#808080"
            opacity: 0.8

            LevelEditor {
               id: levelEditor
               anchors.fill: parent
               Component.onCompleted: {
                   // Lade Tileset
                   levelEditor.loadTileset(":/resources/images/tileset.png", 32, 32)
               }
           }
        }


        Rectangle {
            id: topbar
            anchors.top: parent.top
            anchors.left: parent.left

            width: parent.width
            height: parent.height * 0.1
            color: "#808080"
            opacity: 0.8
        }
    }
}
