define(function() {
    var MouseState = function (canvas) {
        this.width = canvas.width || 50;
        this.height = canvas.height || 50;
        this.X = 0;
        this.Y = 0;
        this.buttons = 0;
    };

    MouseState.prototype.clear = function () {
        this.X = 0;
        this.Y = 0;
        this.buttons = 0;
    };

    var DefaultMouseStringify = function (mouseState) {
        return 'M' +
            mouseState.width + ' ' +
            mouseState.height + ' ' +
            mouseState.X + ' ' +
            mouseState.Y + ' ' +
            mouseState.buttons;
    };

    var Mouse = function (canvas, _stringify) {
        this.canvas = canvas;
        canvas.onmousedown = this.onmousedown.bind(this);
        canvas.onmouseup = this.onmouseup.bind(this);
        canvas.onmousemove = this.onmousemove.bind(this);
        this.state = new MouseState(canvas);
        this.tempstate = new MouseState(canvas);
        this._stringify = _stringify || DefaultMouseStringify;
    };

    Mouse.prototype.onmousedown = function (evt) {
        this.tempstate.buttons |= (1 << evt.which); // (e.keyCode || e.which)
    };

    Mouse.prototype.onmouseup = function (evt) {
        this.tempstate.buttons &= ~(1 << evt.which);
    };

    Mouse.prototype.onmousemove = function (evt) {
        var rect = this.canvas.getBoundingClientRect();
        this.tempstate.X = evt.clientX - rect.left;
        this.tempstate.Y = evt.clientY - rect.top;
    };

    Mouse.prototype.capture = function () {
        this.state = this.tempstate;
    };

    Mouse.prototype.stringify = function() {
        return this._stringify(this.state);
    };

    return {
        Mouse: Mouse,
        MouseState: MouseState
    };
});