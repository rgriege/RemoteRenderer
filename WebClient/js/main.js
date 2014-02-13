require(['domReady!', 'jsmpeg', 'ois', 'game', 'jquery'], function (doc, jsmpeg, ois, game) {
    // Show loading notice
    var canvas = doc.getElementById("videoCanvas");
    var contextType = 'webgl';
    var serverAddress = 'ws://24.155.244.209:';
    var videoPlayer;

    var serverSocket = new WebSocket(serverAddress +'9001');
    var gamePort;
    var library;
    var libIdx = 0;
    var loadLibrary = function (obj) {
        library = obj;
        var wait = Math.max(connectionTime + maxWait - Date.now(), maxWait);
        setTimeout(loadLibraryView, wait);
    };
    var currentGame;
    var unloadGame = function (callback) {
        $('#library').fadeOut(400, callback);
    };
    var loadGame = function(idx, callback) {
        $('#play').text(library[idx]['Name']);
        $('#preview').attr('src', library[idx]['Preview']);
        $('#description').text(library[idx]['Summary']);
        $('#library').fadeIn(400, callback);
    };
    $('#rightArrow').click(function() {
        unloadGame(function() {
            loadGame(++libIdx % library.length);
        });
    });
    $('#leftArrow').click(function () {
        unloadGame(function () {
            loadGame(--libIdx === -1 ? library.length-1 : libIdx);
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
        $('#loader h3').text('Loading Game...');
        $('#library').fadeOut(400, function() { $('#loader').fadeIn(400); });
        console.log($('#play').text());
        serverSocket.send($('#play').text());
    });

    var listenForGameMsg = function (serverMsg) {
        gamePort = parseInt(serverMsg.data);
        // Setup the WebSocket connection and start the player
        var renderSocket = new WebSocket(serverAddress + gamePort);
        renderSocket.onclose = closeGame;
        var inputSocket = new WebSocket(serverAddress + (gamePort + 1));

        currentGame = game.create(canvas, contextType, renderSocket, inputSocket);
        currentGame.onload = function() {
            $('#loader').fadeOut(400, function() { $('#videoCanvas').fadeIn(400); });
        };
    };
    window.onkeydown = function (key) {
        if (key.keyCode === 9 && key.shiftKey && currentGame && currentGame.active)
            $('#footer').toggle();
    };
    $('#footer').hide();
    var closeGame = function () {
        $('#footer').hide();
        currentGame.close();
        $('#videoCanvas').fadeOut(400, function () { $('#library').fadeIn(400); });
    };
    $('#stopBtn').click(closeGame);

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
