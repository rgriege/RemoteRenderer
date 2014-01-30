require(['domReady!', 'jsmpeg', 'ois', 'jquery'], function(doc, jsmpeg, ois) {
    // Show loading notice
    var canvas = doc.getElementById('videoCanvas');

    var mouse = new ois.Mouse(canvas);
    var keyboard = new ois.Keyboard(canvas);
    var inputSocket;

    var renderSocket;
    var videoPlayer;

    var serverSocket = new WebSocket('ws://192.168.0.75:9001');
    var gameSelected = false;
    var gamePort;
    serverSocket.onmessage = function (serverMsg) {
        if (!gameSelected) {
            serverSocket.send('ObjectBrowser');
            gameSelected = true;
        } else {
            gamePort = parseInt(serverMsg.data);
            // Setup the WebSocket connection and start the player
            renderSocket = new WebSocket('ws://192.168.0.75:' + gamePort);
            inputSocket = new WebSocket('ws://192.168.0.75:' + (gamePort + 1));
            inputSocket.onmessage = function (inputMsg) {
                var f = new FileReader();
                f.readAsText(inputMsg.data);
                f.onload = function () {
                    if (inputSocket.readyState !== inputSocket.OPEN)
                        return;
                    if (this.result.charCodeAt(0) === 77)
                        inputSocket.send(mouse.stringify());
                    else if (this.result.charCodeAt(0) === 75)
                        inputSocket.send(keyboard.stringify());
                };
                f.onerror = function (e) { console.log("Error", e); };
            };
            videoPlayer = new jsmpeg.Player(renderSocket, { canvas: canvas, renderer: 'webgl' });
        }
    };
    
    var footer = $('#footer');
    function detectFooterRequest() {
        window.onmousemove = function(evt) {
            if (evt.clientY > window.innerHeight - 20) {
                footer.slideDown(500);
                window.onmousemove = null;
            }
        };
    }
    function hideFooter() {
        if (list.is(':visible'))
            list.hide(0, function() { footer.slideUp(500, detectFooterRequest);});
        else
            footer.slideUp(500, detectFooterRequest);
    }
    footer.mouseleave(function() { footerTimeout = setTimeout(hideFooter, 2000); });
    footer.mouseenter(function() { clearTimeout(footerTimeout); });
    var footerTimeout = setTimeout(hideFooter, 2000);

    var TimeSpan = function () {
        var now = Date.now();
        return {
            start: now,
            end: now
        };
    };

    TimeSpan.prototype.diff = function () {
        return this.end - this.start;
    };

    var statsListener = {
        bufferSize: 10,
        active: false,
        packets: Array(this.bufferSize),
        packetIdx: 0,
        frames: Array(this.bufferSize),
        frameIdx: 0,
        intervalId: undefined,
        onPacket: function() {
            statsListener.packets[this.packetIdx] = Date.now();
            statsListener.packetIdx = (this.packetIdx + 1) % this.bufferSize;
        },
        onFrameStart: function() {
            statsListener.frames[this.frameIdx] = new TimeSpan();
        },
        onFrameEnd: function() {
            statsListener.frames[statsListener.frameIdx].end = Date.now();
            statsListener.frameIdx = (statsListener.frameIdx + 1) % statsListener.bufferSize;
        },
        start: function(broadcaster, interval) {
            broadcaster.setFrameListener(statsListener);
            broadcaster.setPacketListener(statsListener);
            statsListener.intervalId = setInterval(statsListener.show, interval);
            statsListener.active = true;
        },
        stop: function() {
            if (statsListener.intervalId)
                clearInterval(statsListener.intervalId);
            statsListener.active = false;
        },
        getPing: function() {
            var result = 0;
            var count = 0;
            var prev = 0;
            statsListener.packets.forEach(function (elem) {
                if (prev && elem > prev) {
                    result += elem - prev;
                    ++count;
                }
                prev = elem;
            });
            return Math.round(result / count);
        },
        getFrameRate: function() {
            var result = 0;
            var count = 0;
            var prev = undefined;
            statsListener.frames.forEach(function (elem) {
                if (prev && elem.start > prev.start) {
                    result += (elem.start - prev.start);
                    ++count;
                }
                prev = elem;
            });
            return Math.round(result / count);
        },
        getFrameTime: function () {
            var result = 0;
            var count = 0;
            statsListener.frames.forEach(function (elem) {
                result += (elem.end - elem.start);
                ++count;
            });
            return Math.round(result / count);
        },
        show: function() {
            $('#pingCell').text(statsListener.getPing());
            $('#fpsCell').text(statsListener.getFrameRate());
            $('#frameTimeCell').text(statsListener.getFrameTime());
        }
    };
    var statsTbl = $('#statsTbl');
    var statsBtn = $('#statsBtn');
    statsBtn.click(function() {
        if (!statsBtn.hasClass('active')) {
            statsBtn.addClass('active');
            statsTbl.show();
            if (videoPlayer)
                statsListener.start(videoPlayer, 1000);
        } else {
            statsBtn.removeClass('active');
            statsTbl.hide();
            if (statsListener.active)
                statsListener.stop();
        }
    });
    statsTbl.hide();

    var btn = $('#resolutionBtn');
    var list = $('#resolutionList');
    btn.click(function (evt) {
        if (list.is(':visible'))
            list.hide();
        else
            list.show();
        evt.preventDefault();
    });
    var items = list.find('li');
    items.click(function () {
        var succeeded = true;
        var val = $(this).attr('data-value');
        var res = [];
        if (val === 'native') {
            if (videoPlayer)
                res = [videoPlayer.sourceWidth, videoPlayer.sourceHeight];
            else
                succeeded = false;
        } else if (val === 'fullscreen') {
            res = [window.innerWidth, window.innerHeight];
            canvas.style.border = 'none';
        } else {
            res = val.split('x');
        }
        if (succeeded) {
            if (videoPlayer) {
                videoPlayer.resizeCanvas(res[0], res[1]);
            } else {
                canvas.width = res[0];
                canvas.height = res[1];
            }
            items.removeClass('selected');
            $(this).addClass('selected');
            list.hide();
            if (btn.text() === 'Fullscreen')
                canvas.style.border = '#C08717 solid 1px';
            btn.text($(this).text());
            footerTimeout = setTimeout(hideFooter, 2000);
        }
    });
});
