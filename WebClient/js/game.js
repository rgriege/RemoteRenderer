define(['ois', 'jsmpeg'], function(ois, jsmpeg) {

    var Game = function(canvas, contextType, renderSocket, inputSocket) {
        this.renderSocket = renderSocket;
        this.inputSocket = inputSocket;
        var mouse = new ois.Mouse(canvas);
        var keyboard = new ois.Keyboard(canvas);

        this.inputSocket.onmessage = function (inputMsg) {
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

        var videoPlayer = new jsmpeg.Player(renderSocket, { canvas: canvas, renderer: contextType });
    };

    Game.prototype.close = function() {
        this.renderSocket.close();
        this.inputSocket.close();
    };

    return {
        create: function (canvas, contextType, renderSocket, inputSocket) {
            return new Game(canvas, contextType, renderSocket, inputSocket);
        }
    };
});