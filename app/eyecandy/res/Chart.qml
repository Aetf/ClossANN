import QtQuick 2.3
import QtWebChannel 1.0
import QtWebKit 3.0
import QtWebKit.experimental 1.0

WebView {
    id: root
    url: "qrc:///html/index.html"
//    url: "qrc:///html/test.html"

    experimental.webChannel.registeredObjects: [bridge]
    experimental.preferredMinimumContentsWidth: 100
    experimental.preferences.developerExtrasEnabled: developerExtrasEnabled

    property bool developerExtrasEnabled: false
    property var bridge

    // fix broken mouse event handling in WebView
    MouseArea {
        anchors.fill: parent
        onPressed: {
            root.bridge.mouseDown(mouse.x, mouse.y);
        }
        onMouseXChanged: {
            root.bridge.mouseMove(mouseX, mouseY);
        }
        onMouseYChanged: {
            root.bridge.mouseMove(mouseX, mouseY);
        }
        onReleased: {
            root.bridge.mouseUp(mouse.x, mouse.y);
        }
    }
}
