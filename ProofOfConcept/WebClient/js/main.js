require(['domReady!', 'jsmpeg', 'ois'], function(doc, jsmpeg, ois) {
    // Show loading notice
    var canvas = doc.getElementById('videoCanvas');
    var ctx = canvas.getContext('2d');
    ctx.fillStyle = '#444';
    ctx.fillText('Loading...', canvas.width / 2 - 30, canvas.height / 3);

    var mouse = new ois.Mouse(canvas);
    var keyboard = new ois.Keyboard(canvas);
    var inputSocket;

    var renderSocket;
    var videoPlayer;

    var button = doc.getElementById('toggleButton');
    button.textContent = 'Start';
    button.onclick = function () {
        if (this.textContent === 'Start') {
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
            this.textContent = 'Stop';
        } else {
            renderSocket.close();
            inputSocket.close();
            this.textContent = 'Start';
        }
    };
});
