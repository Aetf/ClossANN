import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import UCWNeuralNet 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 800
    height: 600
    title: qsTr("Hello World")

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: console.log("Open action triggered")
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit()
            }
        }
    }

    toolBar: ToolBar {
        RowLayout {
            anchors.fill: parent
            ToolButton {
                text: qsTr("Refresh")
                onClicked: mainChart.reload()
            }
        }
    }

    Chart {
        id: mainChart
        anchors.fill: parent

        developerExtrasEnabled: true

        bridge: Bridge {
            signal predictionUpdated(var data)

            function sayHello() {
                console.log("Hello")
                return "done"
            }

            property var handler: Handler

            onInitiationDone: {
                console.log("Loading finished")
                Handler.configure();
                Handler.runAsync();

                timer.start()
            }
        }
        Timer {
            id: timer
            interval: 2000
            repeat: true
            onTriggered: {
                Handler.requestPrediction()
            }
        }
    }
}
