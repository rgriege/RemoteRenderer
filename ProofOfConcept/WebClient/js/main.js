require(['domReady!', 'jsmpeg', 'ois', 'jquery'], function(doc, jsmpeg, ois) {
    // Show loading notice
    var canvas = doc.getElementById('videoCanvas');

    var mouse = new ois.Mouse(canvas);
    var keyboard = new ois.Keyboard(canvas);
    var inputSocket;

    var renderSocket;
    var videoPlayer;
    
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

    var connectBtn = $('#connectBtn');
    var playing = false;
    connectBtn.click(function() {
        if (!playing) {
            // Setup the WebSocket connection and start the player
            renderSocket = new WebSocket('ws://192.168.0.75:9002');
            inputSocket = new WebSocket('ws://192.168.0.75:9003');
            inputSocket.onmessage = function(msg) {
                var f = new FileReader();
                f.readAsText(msg.data);
                f.onload = function() {
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
    });

    var statsTbl = $('#statsTbl');
    var statsIntervalId;
    var statsBtn = $('#statsBtn');
    var monitoring = false;
    statsBtn.click(function() {
        if (!monitoring) {
            statsBtn.addClass('active');
            statsTbl.show();
            statsIntervalId = setInterval(function() {
                if (videoPlayer) {
                    $('#pingCell').text(videoPlayer.getPing());
                    $('#fpsCell').text(videoPlayer.getFrameRate());
                    $('#frameTimeCell').text(videoPlayer.getFrameTime());
                }
            }, 1000);
            monitoring = true;
        } else {
            statsBtn.removeClass('active');
            statsTbl.hide();
            clearInterval(statsIntervalId);
            monitoring = false;
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
