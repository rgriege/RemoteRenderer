define(function () {

    //-----------------------------------------------------------------------//
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

    //-----------------------------------------------------------------------//
    var DefaultMouseStringify = function (mouseState) {
        return 'M' +
            mouseState.width + ' ' +
            mouseState.height + ' ' +
            mouseState.X + ' ' +
            mouseState.Y + ' ' +
            mouseState.buttons;
    };

    //-----------------------------------------------------------------------//
    var Mouse = function (canvas, _stringify) {
        this.canvas = canvas;
        canvas.onmousedown = this.onmousedown.bind(this);
        canvas.onmouseup = this.onmouseup.bind(this);
        canvas.onmousemove = this.onmousemove.bind(this);
        this.state = new MouseState(canvas);
        this._stringify = _stringify || DefaultMouseStringify;
    };

    Mouse.prototype.onmousedown = function (evt) {
        this.state.buttons |= (1 << mouseMap[evt.which]); // (e.keyCode || e.which)
    };

    Mouse.prototype.onmouseup = function (evt) {
        this.state.buttons &= ~(1 << mouseMap[evt.which]);
    };

    Mouse.prototype.onmousemove = function (evt) {
        var rect = this.canvas.getBoundingClientRect();
        this.state.X = evt.clientX - rect.left;
        this.state.Y = evt.clientY - rect.top;
    };

    Mouse.prototype.stringify = function() {
        return this._stringify(this.state);
    };

    //-----------------------------------------------------------------------//
    var DefaultKeyboardStringify = function (keyBuffer) {
        var msg = 'K';
        for (var i = 0; i < 256; ++i)
            msg += keyBuffer[i] || '0';
        return msg;
    };

    //-----------------------------------------------------------------------//
    var Keyboard = function (canvas, _stringify) {
        this.canvas = canvas;
        canvas.onkeydown = this.onkeydown.bind(this);
        canvas.onkeyup = this.onkeyup.bind(this);
        this.keyBuffer = [];
        this._stringify = _stringify || DefaultKeyboardStringify;
    };

    Keyboard.prototype.onkeydown = function(evt) {
        this.keyBuffer[keyMap[evt.keyCode]] = 1;
    };

    Keyboard.prototype.onkeyup = function(evt) {
        this.keyBuffer[keyMap[evt.keyCode]] = 0;
    };

    Keyboard.prototype.stringify = function () {
        return this._stringify(this.keyBuffer);
    };

    //-----------------------------------------------------------------------//
    var mouseMap = [];
    mouseMap[1] = 0;
    mouseMap[3] = 1;
    mouseMap[2] = 2;

    //-----------------------------------------------------------------------//
    var keyMap = [];
    keyMap[8] = 14;
    keyMap[9] = 15;
    keyMap[13] = 28; // Main enter
    keyMap[16] = 42; // Only LShift
    keyMap[17] = 29; // Only LCtrl
    keyMap[18] = 38; // Only LAlt
    keyMap[20] = 58; // Caps Lock -> KC_CAPITAL?
    keyMap[27] = 1;
    keyMap[32] = 57;
    keyMap[33] = 201;
    keyMap[34] = 209;
    keyMap[35] = 207;
    keyMap[36] = 199; // Arrow Keys
    keyMap[37] = 203;
    keyMap[38] = 200;
    keyMap[39] = 205;
    keyMap[40] = 208;
    keyMap[48] = 11; // Main 0-9
    keyMap[49] = 2;
    keyMap[50] = 3;
    keyMap[51] = 4;
    keyMap[52] = 5;
    keyMap[53] = 6;
    keyMap[54] = 7;
    keyMap[55] = 8;
    keyMap[56] = 9;
    keyMap[57] = 10;
    keyMap[65] = 30; // A-Z
    keyMap[66] = 48;
    keyMap[67] = 46;
    keyMap[68] = 32;
    keyMap[69] = 18;
    keyMap[70] = 33;
    keyMap[71] = 34;
    keyMap[72] = 35;
    keyMap[73] = 23;
    keyMap[74] = 36;
    keyMap[75] = 37;
    keyMap[76] = 38;
    keyMap[77] = 50;
    keyMap[78] = 49;
    keyMap[79] = 24;
    keyMap[80] = 25;
    keyMap[81] = 16;
    keyMap[82] = 19;
    keyMap[83] = 31;
    keyMap[84] = 20;
    keyMap[85] = 22;
    keyMap[86] = 47;
    keyMap[87] = 17;
    keyMap[88] = 45;
    keyMap[89] = 21;
    keyMap[90] = 44;
    keyMap[96] = 82; // Numpad Keys
    keyMap[97] = 79;
    keyMap[98] = 80;
    keyMap[99] = 81;
    keyMap[100] = 75;
    keyMap[101] = 76;
    keyMap[102] = 77;
    keyMap[103] = 71;
    keyMap[104] = 72;
    keyMap[105] = 73;
    keyMap[106] = 55;
    keyMap[107] = 78;
    keyMap[109] = 74;
    keyMap[110] = 83;
    keyMap[111] = 181;
    keyMap[112] = 59; // Function Keys
    keyMap[113] = 60;
    keyMap[114] = 61;
    keyMap[115] = 62;
    keyMap[116] = 63;
    keyMap[117] = 64;
    keyMap[118] = 65;
    keyMap[119] = 66;
    keyMap[120] = 67;
    keyMap[121] = 68;
    keyMap[122] = 87;
    keyMap[123] = 88;
    keyMap[144] = 69;
    keyMap[186] = 39;
    keyMap[187] = 13;
    keyMap[188] = 51;
    keyMap[189] = 12;
    keyMap[190] = 52;
    keyMap[191] = 53;
    keyMap[192] = 41; //`~ -> KC_GRAVE?
    keyMap[219] = 26;
    keyMap[220] = 43;
    keyMap[221] = 27;
    keyMap[222] = 40;

    //-----------------------------------------------------------------------//
    return {
        MouseState: MouseState,
        Mouse: Mouse,
        Keyboard: Keyboard
    };
});