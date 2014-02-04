require(['domReady!', 'jsmpeg', 'ois', 'jquery'], function(doc, jsmpeg, ois) {
    // Show loading notice
    var canvas = doc.getElementById("videoCanvas");
    var contextType = '2d';
    var videoPlayer;

    var serverSocket = new WebSocket('ws://192.168.0.75:9001');
    var gamePort;
    var library;
    var libIdx = 0;
    var loadLibrary = function (obj) {
        library = obj;
        console.log(library.length + ' Games');
        var wait = Math.max(connectionTime + maxWait - Date.now(), maxWait);
        setTimeout(loadLibraryView, wait);
    };
    var unloadGame = function(callback) {
        $('#library').fadeOut(400, callback);
    };
    var loadGame = function(idx, callback) {
        $('#name').text(library[0]['Name']);
        var preview = new Image();
        preview.src = library[0]['Preview'];
        canvas.getContext(contextType).fillStyle = "black";
        canvas.getContext(contextType).fillRect(0, 0, canvas.width, canvas.height);
        canvas.getContext(contextType).drawImage(preview, (canvas.width - preview.width) / 2, (canvas.height - preview.height)/2);
        $('#description').text(library[0]['Summary']);
        $('#library').fadeIn(400, callback);
    };
    $('#rightArrow').click(function() {
        unloadGame(function() {
            loadGame((libIdx + 1) % library.length);
        });
    });
    $('#leftArrow').click(function () {
        unloadGame(function () {
            loadGame(libIdx-1 === -1 ? library.length-1 : libIdx-1);
        });
    });
    var loadLibraryView = function () {
        serverSocket.onmessage = listenForGameMsg;
        $('#loader').fadeOut(400, function () {
            loadGame(0);
        });
    };
    var maxWait = 500;
    var connectionTime;
    var listenForLibraryMsg = function (msg) {
        var f = new FileReader();
        f.readAsText(msg.data);
        f.onload = function () { loadLibrary(JSON.parse(this.result)); };
        f.onerror = function (e) { console.log("Error", e); };
    };
    var pageLoadTime = Date.now();
    serverSocket.onopen = function () {
        var now = Date.now();
        var wait = Math.min(Math.max(pageLoadTime + maxWait - now, 0), maxWait);
        connectionTime = now + wait;
        serverSocket.onmessage = listenForLibraryMsg;
        setTimeout(function() {
            $('#loader h3').text('Loading Library...');
        }, wait);
    };
    $('#play').click(function () {
        console.log($('#play').text());
        serverSocket.send($('#play').text());
    });

    var listenForGameMsg = function (serverMsg) {
        gamePort = parseInt(serverMsg.data);
        // Setup the WebSocket connection and start the player
        var renderSocket = new WebSocket('ws://192.168.0.75:' + gamePort);
        var inputSocket = new WebSocket('ws://192.168.0.75:' + (gamePort + 1));

        var mouse = new ois.Mouse(canvas);
        var keyboard = new ois.Keyboard(canvas);

        /*renderSocket.onopen = function() {
            var res = $('#resolutionList .selected').attr('data-value').split('x');
            canvas.width = res[0];
            canvas.height = res[1];
        };*/

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

        videoPlayer = new jsmpeg.Player(renderSocket, { canvas: canvas, renderer: contextType });
    };

    var windowKeyboard = new ois.Keyboard(window);
    windowKeyboard.setEventListener({
        keyPressed: function (keyCode) {
            if (keyCode == ois.KeyCode.KC_TAB && windowKeyboard.keydown(ois.KeyCode.KC_LSHIFT))
                $('#footer').toggle();
        },
        keyReleased: function () {}
    });
    $('#footer').hide();

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
        }
    });
});
