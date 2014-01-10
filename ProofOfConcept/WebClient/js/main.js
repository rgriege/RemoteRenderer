require(['domReady!', 'jsmpeg', 'ois'], function(doc, jsmpeg, ois) {
    // Show loading notice
    var canvas = doc.getElementById('videoCanvas');

    var mouse = new ois.Mouse(canvas);
    var keyboard = new ois.Keyboard(canvas);
    var inputSocket;

    var renderSocket;
    var videoPlayer;
    
    var connectBtn = doc.getElementById('connectBtn');
    var playing = false;
    connectBtn.onclick = function () {
        if (!playing) {
            // Setup the WebSocket connection and start the player
            renderSocket = new WebSocket('ws://172.16.1.63:9002');
            inputSocket = new WebSocket('ws://172.16.1.63:9003');
            inputSocket.onmessage = function (msg) {
                var f = new FileReader();
                f.readAsText(msg.data);
                f.onload = function () {
                    if (inputSocket.readyState !== inputSocket.OPEN)
                        return;
                    if (this.result.charCodeAt(0) === 77)
                        inputSocket.send(mouse.stringify());
                    else if (this.result.charCodeAt(0) === 75)
                        inputSocket.send(keyboard.stringify());
                };
                f.onerror = function(e) { console.log("Error", e); };
            };
            videoPlayer = new jsmpeg.Player(renderSocket, { canvas: canvas, renderer: 'webgl' });
            playing = true;
            this.src = 'images/pause.png';
        } else {
            renderSocket.close();
            inputSocket.close();
            playing = false;
            this.src = 'images/play.png';
        }
    };

    var statsTbl = doc.getElementById('statsTbl');
    var statsIntervalId;
    var statsBtn = doc.getElementById('statsBtn');
    var monitoring = false;
    statsBtn.onclick = function() {
        if (!monitoring) {
            statsTbl.style.display = 'table';
            statsIntervalId = setInterval(function () {
                if (videoPlayer) {
                    doc.getElementById('pingCell').textContent = videoPlayer.getPing();
                    doc.getElementById('fpsCell').textContent = videoPlayer.getFrameRate();
                    doc.getElementById('frameTimeCell').textContent = videoPlayer.getFrameTime();
                }
            }, 1000);
            monitoring = true;
        } else {
            statsTbl.style.display = 'none';
            clearInterval(statsIntervalId);
            monitoring = false;
        }
    };

    var resolutionSelector = doc.getElementById('resolutionSelector');
    resolutionSelector.onchange = function () {
        var val = resolutionSelector.selectedOptions[0].value;
        if (val === 'native') {
            if(videoPlayer)
                videoPlayer.resizeCanvas();
        } else {
            var res = val.split('x');
            if (videoPlayer) {
                videoPlayer.resizeCanvas(res[0], res[1]);
            } else {
                canvas.width = res[0];
                canvas.height = res[1];
            }
        }
    };
});
