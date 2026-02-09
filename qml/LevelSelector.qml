import QtQuick 2.15
import QtQuick.Controls 2.15
import Cloudgate_game 1.0

Page {  
    id: levelSelectorPage
    Image {
        anchors.fill: parent
        source: "qrc:/resources/images/backgrounds/skyIslands.jpg"
        fillMode: Image.PreserveAspectCrop
        smooth: true
    }

    Button 
    {
        text: "Back"
        height: 40
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 16
        anchors.leftMargin: 16

        onClicked: stackView.pop()
    }

    Text 
    {
        text: "Wähle dein Level"
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 25
        font.pixelSize: 24
        font.bold: true
        color: "#000000"
    }

    Rectangle 
    {
        id: levelBox
        width: parent.width * 0.8
        height: parent.height * 0.8
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height * 0.15
        color: "#333333"
        border.color: "#999999"
        border.width: 2
        radius: 12

        ListView 
        {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 8
            model: LevelSelector { }
            clip: true

            delegate: Rectangle 
            {
                id: levelItem
                width: ListView.view.width
                height: 56
                color: "#f5f5f5"
                border.color: "#cfcfcf"
                border.width: 1
                radius: 6


                // level name
                Text 
                {
                    anchors.fill: parent
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: parent.height * 0.3
                    anchors.leftMargin: parent.width * 0.05
                    text: name
                    font.pixelSize: 18
                    color: "#222"
                }

                // goals
                Row 
                {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: parent.height * 0.3
                    anchors.leftMargin: parent.width * 0.63
                    spacing: 6
                    Text 
                    {
                        text: "goal: "
                        font.pixelSize: 18
                    }
                    Image 
                    {
                        width: 20
                        height: 20
                        source: goalType === 1 ? "qrc:/resources/images/Coin.png" : goalType === 2 ? "qrc:/resources/images/Time.png" : goalType === 0 ? "qrc:/resources/images/None.png" : ""
                        fillMode: Image.PreserveAspectFit
                        visible: goalType >= 0 && goalType <= 2
                    }
                    Text 
                    {
                        text: goalType === 1 ? goalValue + " coins" : goalType === 2 ? goalValue + " seconds" : goalType === 0 ? "        " : ""
                        font.pixelSize: 18
                        color: "#3d3d3d"
                    }
                }

                // check for hover and clicks
                MouseArea 
                {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: levelItem.color = "#888888"
                    onExited: levelItem.color = "#f5f5f5"

                    onClicked: 
                    {
                        console.log("Level gewählt:", file)

                        // switch to SecondPage (created as Component to send file path too)
                        var comp = Qt.createComponent("SecondPage.qml")
                        if (comp.status === Component.Ready) 
                        {
                            var page = comp.createObject(stackView)
                            if (page !== null) 
                            {
                                stackView.push(page)
                                // Übergabe des Levels
                                if (page.initLevel) 
                                {
                                    console.log(file)
                                    page.initLevel(file)
                                }
                            } else 
                            {
                                console.error("Failed to create SecondPage")
                            }
                        }
                    }
                }
            }

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }
        }
    }
}
