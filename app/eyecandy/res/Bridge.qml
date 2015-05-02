import QtQuick 2.0
import QtWebChannel 1.0

QtObject {
    id: root
    WebChannel.id: "bridge"

    signal viewportChanged(real width, real height)

    function log(msg) {
        var args = Array.prototype.slice.call(arguments)
        args.unshift("WebView:")
        return console.log.apply(console, args)
    }

    property var con: console
}

