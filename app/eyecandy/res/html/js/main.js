var bridge, theChart;
var enableRemoteLog = false;

$(initiateWebChannel);
// $(actualWork);

function log() {
    var logger;
    if (enableRemoteLog && bridge && bridge.log) {
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
        bridge.doneInitiation();
    });
}

function onViewportChanged(width, height) {
    log("Viewport changed to", width, height);
}

function actualWork() {
    // Give the points a 3D feel by adding a radial gradient
    Highcharts.getOptions().colors =
        $.map(Highcharts.getOptions().colors, function(color) {
            return {
                radialGradient: {
                    cx: 0.4,
                    cy: 0.3,
                    r: 0.5
                },
                stops: [
                    [0, color],
                    [1, Highcharts.Color(color).brighten(-0.2).get('rgb')]
                ]
            };
        });
    // Set up the chart
    theChart = new Highcharts.Chart({
        chart: {
            renderTo: 'chartContainer',
            type: 'scatter',
            margin: 95,
            animation: {
                duration: 1000,
                easing: "swing"
            },
            options3d: {
                enabled: true,
                alpha: 10,
                beta: 30,
                depth: 550,
                viewDistance: 10,
                frame: {
                    bottom: {
                        size: 1,
                        color: 'rgba(0,0,0,0.02)'
                    },
                    back: {
                        size: 1,
                        color: 'rgba(0,0,0,0.04)'
                    },
                    side: {
                        size: 1,
                        color: 'rgba(0,0,0,0.06)'
                    }
                }
            }
        },
        credits: {
            enabled: false
        },
        legend: {
            enabled: false
        },
        plotOptions: {
            scatter: {
                cropThreshold: 500,
                dataLabels: {
                    enabled: false
                },
                enableMouseTracking: false
            }
        },
        title: {
            text: 'Title text'
        },
        subtitle: {
            text: 'Click and drag'
        },
        tooltip: {
            enabled: false
        },
        xAxis: {
            min: 0,
            max: 1,
            gridLineWidth: 1
        },
        yAxis: {
            min: -1,
            max: 1,
            title: null
        },
        zAxis: {
            min: 0,
            max: 1,
            labels: {
                enabled: true
            }
        },
        series: [{
            name: 'Training Data',
            colorByPoint: false,
            marker: { radius: 2 },
            data: []
        }, {
            name: 'Test Data',
            colorByPoint: false,
            marker: { radius: 2 },
            data: []
        }, {
            name: 'Prediction',
            colorByPoint: false,
            marker: { radius: 3 },
            data: []
        }]
    });

    // Add mouse events for rotation
    $(theChart.container).on('mousedown.ucw touchstart.ucw', function(e) {
        e = theChart.pointer.normalize(e);

        var posX = e.pageX,
            posY = e.pageY,
            alpha = theChart.options.chart.options3d.alpha,
            beta = theChart.options.chart.options3d.beta,
            newAlpha,
            newBeta,
            sensitivity = 5; // lower is more sensitive

        $(document).on('mousemove.ucw.tmp touchdrag.ucw.tmp', function(e) {
            // Run beta
            newBeta = beta + (posX - e.pageX) / sensitivity;
            newBeta = Math.min(100, Math.max(-100, newBeta));
            theChart.options.chart.options3d.beta = newBeta;

            // Run alpha
            newAlpha = alpha + (e.pageY - posY) / sensitivity;
            newAlpha = Math.min(100, Math.max(-100, newAlpha));
            theChart.options.chart.options3d.alpha = newAlpha;

        });
        $(document).on('mouseup.ucw touchend.ucw', function() {
            $(document).off('.ucw.tmp');
            theChart.redraw();
        });
    });
    // fix mouse drag
    fixMouseEvent(theChart.container);

    // listen to future data
    bridge.handler.predictionUpdated.connect(setPrediction);
    bridge.handler.trainingDataUpdated.connect(setTrainingData);
    bridge.handler.testingDataUpdated.connect(setTestingData);
}

function setPrediction(data) {
    log("Setting prediction plain with length", data.length);
    theChart.series[2].setData(data);
}

function setTrainingData(data) {
    log("Setting training data with length", data.length);
    theChart.series[0].setData(data);
}

function setTestingData(data) {
    log("Setting testing data with length", data.length);
    theChart.series[1].setData(data);
}

// Mouse event handling is broken in QtQuick WebView
// see http://www.mail-archive.com/webkit-qt@lists.webkit.org/msg02835.html
// fix it for a particular using an indirect method,
// which may have performance issues.
function fixMouseEvent(target) {
    bridge.mouseDown.connect(function(x, y) {
        $(target).trigger($.Event("mousedown", { pageX: x, pageY: y}));
    });
    bridge.mouseMove.connect(function(x, y) {
        $(target).trigger($.Event("mousemove", { pageX: x, pageY: y}));
    });
    bridge.mouseUp.connect(function(x, y) {
        $(target).trigger($.Event("mouseup", { pageX: x, pageY: y}));
    });
}