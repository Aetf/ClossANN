import QtQuick 2.3
import QtWebChannel 1.0
import QtWebKit 3.0
import QtWebKit.experimental 1.0

WebView {
    id: root
    url: "qrc:///html/index.html"

    experimental.webChannel.registeredObjects: [bridge]
    experimental.preferredMinimumContentsWidth: 100
    experimental.preferences.developerExtrasEnabled: developerExtrasEnabled

    property bool developerExtrasEnabled: false
    property var bridge
}
