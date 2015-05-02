var bridge;

window.onload = function() {
    initiateWebChannel();
}


function log() {
    var logger;
    if (bridge && bridge.log) {
        logger = function() {
            var args = Array.prototype.slice.call(arguments);
            return bridge.log(args.join(" "));
        }
    } else {
        logger = console.log.bind(console);
    }
    logger.apply(this, arguments);
}

function initiateWebChannel() {
    log("Setting up WebChannel");
    new QWebChannel(navigator.qtWebChannelTransport, function(channel) {
        log("WebChannel initiated, further log should go to debuger console");
        bridge = channel.objects.bridge;

        // connect to viewportChanged signal
        bridge.viewportChanged.connect(onViewportChanged);

        actualWork();
    });
}

function onViewportChanged(width, height) {
    log("Viewport changed to", width, height);
}

function actualWork() {
    var canvas = document.getElementById("chartCanvas");

    log("Handle window resize event");
    window.onresize = function() {
        var height = canvas.height = document.body.offsetHeight;
        var width = canvas.width = document.body.offsetWidth;
        log("window size changed to: " + width + ", " + height)
    };

    log("Setup charting object");
    var ctx = canvas.getContext("2d");
    Chart.defaults.global.responsive = true;
    Chart.defaults.global.maintainAspectRatio = false;
    var chartCtrl = new Chart(ctx);

    log("Preparing charting data");
    var data = {
        labels: ["January", "February", "March", "April", "May", "June", "July"],
        datasets: [{
            label: "My First dataset",
            fillColor: "rgba(220,220,220,0.2)",
            strokeColor: "rgba(220,220,220,1)",
            pointColor: "rgba(220,220,220,1)",
            pointStrokeColor: "#fff",
            pointHighlightFill: "#fff",
            pointHighlightStroke: "rgba(220,220,220,1)",
            data: [65, 59, 80, 81, 56, 55, 40]
        }, {
            label: "My Second dataset",
            fillColor: "rgba(151,187,205,0.2)",
            strokeColor: "rgba(151,187,205,1)",
            pointColor: "rgba(151,187,205,1)",
            pointStrokeColor: "#fff",
            pointHighlightFill: "#fff",
            pointHighlightStroke: "rgba(151,187,205,1)",
            data: [28, 48, 40, 19, 86, 27, 90]
        }]
    };

    log("Draw line chart");
    chartCtrl.Line(data)
}
