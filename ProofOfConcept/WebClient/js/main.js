require(['domReady!', 'jsmpeg', 'ois'], function(doc, jsmpeg, ois) {
    // Show loading notice
    var canvas = doc.getElementById('videoCanvas');

    var mouse = new ois.Mouse(canvas);
    var keyboard = new ois.Keyboard(canvas);
    var inputSocket;

    var renderSocket;
    var videoPlayer;

    var connectBtn = doc.getElementById('connectBtn');
    connectBtn.textContent = 'Connect';
    connectBtn.onclick = function () {
        if (this.textContent === 'Connect') {
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
            videoPlayer = new jsmpeg.Player(renderSocket, { canvas: canvas });
            this.textContent = 'Disconnect';
        } else {
            renderSocket.close();
            inputSocket.close();
            this.textContent = 'Connect';
        }
    };

    var statsTbl = doc.getElementById('statsTbl');
    statsTbl.style.top = canvas.offsetTop + 10 + "px";
    var statsIntervalId;
    var statsBtn = doc.getElementById('statsBtn');
    statsBtn.textContent = 'Show Stats';
    statsBtn.onclick = function() {
        if (this.textContent === 'Show Stats') {
            statsTbl.style.display = 'table';
            statsTbl.style.left = canvas.offsetLeft + canvas.width - 10 - statsTbl.offsetWidth + "px";
            statsIntervalId = setInterval(function () {
                if (videoPlayer) {
                    doc.getElementById('pingCell').textContent = videoPlayer.getPing();
                    doc.getElementById('fpsCell').textContent = videoPlayer.getFrameRate();
                    doc.getElementById('frameTimeCell').textContent = videoPlayer.getFrameTime();
                }
            }, 1000);
            this.textContent = 'Hide Stats';
        } else {
            statsTbl.style.display = 'none';
            clearInterval(statsIntervalId);
            this.textContent = 'Show Stats';
        }
    };
});
