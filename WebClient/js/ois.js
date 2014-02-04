define(function() {

    //-----------------------------------------------------------------------//
    var MouseState = function (elem) {
        this.width = elem.width || 50;
        this.height = elem.height || 50;
        this.X = 0;
        this.Y = 0;
        this.buttons = 0;
    };

    MouseState.prototype.clear = function() {
        this.X = 0;
        this.Y = 0;
        this.buttons = 0;
    };

    //-----------------------------------------------------------------------//
    var DefaultMouseStringify = function(mouseState) {
        return 'M' +
            mouseState.width + ' ' +
            mouseState.height + ' ' +
            mouseState.X + ' ' +
            mouseState.Y + ' ' +
            mouseState.buttons;
    };

    //-----------------------------------------------------------------------//
    var Mouse = function (elem, stringify) {
        elem.onmousedown = this.onmousedown.bind(this);
        elem.onmouseup = this.onmouseup.bind(this);
        elem.onmousemove = this.onmousemove.bind(this);
        this.elem = elem;
        this.state = new MouseState(elem);
        this._stringify = stringify || DefaultMouseStringify;
    };

    Mouse.prototype.onmousedown = function(evt) {
        this.state.buttons |= (1 << mouseMap[evt.which]); // (e.keyCode || e.which)
    };

    Mouse.prototype.onmouseup = function(evt) {
        this.state.buttons &= ~(1 << mouseMap[evt.which]);
    };

    Mouse.prototype.onmousemove = function(evt) {
        var rect = this.elem.getBoundingClientRect();
        this.state.X = evt.clientX - rect.left;
        this.state.Y = evt.clientY - rect.top;
    };

    Mouse.prototype.stringify = function() {
        return this._stringify(this.state);
    };

    //-----------------------------------------------------------------------//
    var DefaultKeyboardStringify = function(keyBuffer) {
        var msg = 'K';
        for (var i = 0; i < 256; ++i)
            msg += keyBuffer[i] || '0';
        return msg;
    };

    //-----------------------------------------------------------------------//
    var Keyboard = function (elem, stringify) {
        elem.onkeydown = this.onkeydown.bind(this);
        elem.onkeyup = this.onkeyup.bind(this);
        this.keyBuffer = [];
        this._stringify = stringify || DefaultKeyboardStringify;
        this.listener = undefined;
    };

    Keyboard.prototype.onkeydown = function(evt) {
        var oisKeyCode = keyMap[evt.keyCode];
        this.keyBuffer[oisKeyCode] = 1;
        if (this.listener)
            this.listener.keyPressed(oisKeyCode);
    };

    Keyboard.prototype.onkeyup = function (evt) {
        var oisKeyCode = keyMap[evt.keyCode];
        this.keyBuffer[oisKeyCode] = 0;
        if (this.listener)
            this.listener.keyReleased(oisKeyCode);
    };

    Keyboard.prototype.keydown = function(keyCode) {
        return this.keyBuffer[keyCode] === 1;
    };

    Keyboard.prototype.keyup = function(keyCode) {
        return this.keyBuffer[keyCode] === 0;
    };

    Keyboard.prototype.setEventListener = function(listener) {
        this.listener = listener;
    };

    Keyboard.prototype.stringify = function() {
        return this._stringify(this.keyBuffer);
    };

    //-----------------------------------------------------------------------//
    var mouseMap = [];
    mouseMap[1] = 0;
    mouseMap[3] = 1;
    mouseMap[2] = 2;

    //-----------------------------------------------------------------------//
    var KeyCode = {
        KC_UNASSIGNED: 0x00,
        KC_ESCAPE: 0x01,
        KC_1: 0x02,
        KC_2: 0x03,
        KC_3: 0x04,
        KC_4: 0x05,
        KC_5: 0x06,
        KC_6: 0x07,
        KC_7: 0x08,
        KC_8: 0x09,
        KC_9: 0x0A,
        KC_0: 0x0B,
        KC_MINUS: 0x0C, // - on main keyboard
        KC_EQUALS: 0x0D,
        KC_BACK: 0x0E, // backspace
        KC_TAB: 0x0F,
        KC_Q: 0x10,
        KC_W: 0x11,
        KC_E: 0x12,
        KC_R: 0x13,
        KC_T: 0x14,
        KC_Y: 0x15,
        KC_U: 0x16,
        KC_I: 0x17,
        KC_O: 0x18,
        KC_P: 0x19,
        KC_LBRACKET: 0x1A,
        KC_RBRACKET: 0x1B,
        KC_RETURN: 0x1C, // Enter on main keyboard
        KC_LCONTROL: 0x1D,
        KC_A: 0x1E,
        KC_S: 0x1F,
        KC_D: 0x20,
        KC_F: 0x21,
        KC_G: 0x22,
        KC_H: 0x23,
        KC_J: 0x24,
        KC_K: 0x25,
        KC_L: 0x26,
        KC_SEMICOLON: 0x27,
        KC_APOSTROPHE: 0x28,
        KC_GRAVE: 0x29, // accent
        KC_LSHIFT: 0x2A,
        KC_BACKSLASH: 0x2B,
        KC_Z: 0x2C,
        KC_X: 0x2D,
        KC_C: 0x2E,
        KC_V: 0x2F,
        KC_B: 0x30,
        KC_N: 0x31,
        KC_M: 0x32,
        KC_COMMA: 0x33,
        KC_PERIOD: 0x34, // . on main keyboard
        KC_SLASH: 0x35, // / on main keyboard
        KC_RSHIFT: 0x36,
        KC_MULTIPLY: 0x37, // * on numeric keypad
        KC_LMENU: 0x38, // left Alt
        KC_SPACE: 0x39,
        KC_CAPITAL: 0x3A,
        KC_F1: 0x3B,
        KC_F2: 0x3C,
        KC_F3: 0x3D,
        KC_F4: 0x3E,
        KC_F5: 0x3F,
        KC_F6: 0x40,
        KC_F7: 0x41,
        KC_F8: 0x42,
        KC_F9: 0x43,
        KC_F10: 0x44,
        KC_NUMLOCK: 0x45,
        KC_SCROLL: 0x46, // Scroll Lock
        KC_NUMPAD7: 0x47,
        KC_NUMPAD8: 0x48,
        KC_NUMPAD9: 0x49,
        KC_SUBTRACT: 0x4A, // - on numeric keypad
        KC_NUMPAD4: 0x4B,
        KC_NUMPAD5: 0x4C,
        KC_NUMPAD6: 0x4D,
        KC_ADD: 0x4E, // + on numeric keypad
        KC_NUMPAD1: 0x4F,
        KC_NUMPAD2: 0x50,
        KC_NUMPAD3: 0x51,
        KC_NUMPAD0: 0x52,
        KC_DECIMAL: 0x53, // . on numeric keypad
        KC_OEM_102: 0x56, // < > | on UK/Germany keyboards
        KC_F11: 0x57,
        KC_F12: 0x58,
        KC_F13: 0x64, //                     (NEC PC98)
        KC_F14: 0x65, //                     (NEC PC98)
        KC_F15: 0x66, //                     (NEC PC98)
        KC_KANA: 0x70, // (Japanese keyboard)
        KC_ABNT_C1: 0x73, // / ? on Portugese (Brazilian) keyboards
        KC_CONVERT: 0x79, // (Japanese keyboard)
        KC_NOCONVERT: 0x7B, // (Japanese keyboard)
        KC_YEN: 0x7D, // (Japanese keyboard)
        KC_ABNT_C2: 0x7E, // Numpad . on Portugese (Brazilian) keyboards
        KC_NUMPADEQUALS: 0x8D, //: on numeric keypad (NEC PC98)
        KC_PREVTRACK: 0x90, // Previous Track (KC_CIRCUMFLEX on Japanese keyboard)
        KC_AT: 0x91, //                     (NEC PC98)
        KC_COLON: 0x92, //                     (NEC PC98)
        KC_UNDERLINE: 0x93, //                     (NEC PC98)
        KC_KANJI: 0x94, // (Japanese keyboard)
        KC_STOP: 0x95, //                     (NEC PC98)
        KC_AX: 0x96, //                     (Japan AX)
        KC_UNLABELED: 0x97, //                        (J3100)
        KC_NEXTTRACK: 0x99, // Next Track
        KC_NUMPADENTER: 0x9C, // Enter on numeric keypad
        KC_RCONTROL: 0x9D,
        KC_MUTE: 0xA0, // Mute
        KC_CALCULATOR: 0xA1, // Calculator
        KC_PLAYPAUSE: 0xA2, // Play / Pause
        KC_MEDIASTOP: 0xA4, // Media Stop
        KC_VOLUMEDOWN: 0xAE, // Volume -
        KC_VOLUMEUP: 0xB0, // Volume +
        KC_WEBHOME: 0xB2, // Web home
        KC_NUMPADCOMMA: 0xB3, // , on numeric keypad (NEC PC98)
        KC_DIVIDE: 0xB5, // / on numeric keypad
        KC_SYSRQ: 0xB7,
        KC_RMENU: 0xB8, // right Alt
        KC_PAUSE: 0xC5, // Pause
        KC_HOME: 0xC7, // Home on arrow keypad
        KC_UP: 0xC8, // UpArrow on arrow keypad
        KC_PGUP: 0xC9, // PgUp on arrow keypad
        KC_LEFT: 0xCB, // LeftArrow on arrow keypad
        KC_RIGHT: 0xCD, // RightArrow on arrow keypad
        KC_END: 0xCF, // End on arrow keypad
        KC_DOWN: 0xD0, // DownArrow on arrow keypad
        KC_PGDOWN: 0xD1, // PgDn on arrow keypad
        KC_INSERT: 0xD2, // Insert on arrow keypad
        KC_DELETE: 0xD3, // Delete on arrow keypad
        KC_LWIN: 0xDB, // Left Windows key
        KC_RWIN: 0xDC, // Right Windows key
        KC_APPS: 0xDD, // AppMenu key
        KC_POWER: 0xDE, // System Power
        KC_SLEEP: 0xDF, // System Sleep
        KC_WAKE: 0xE3, // System Wake
        KC_WEBSEARCH: 0xE5, // Web Search
        KC_WEBFAVORITES: 0xE6, // Web Favorites
        KC_WEBREFRESH: 0xE7, // Web Refresh
        KC_WEBSTOP: 0xE8, // Web Stop
        KC_WEBFORWARD: 0xE9, // Web Forward
        KC_WEBBACK: 0xEA, // Web Back
        KC_MYCOMPUTER: 0xEB, // My Computer
        KC_MAIL: 0xEC, // Mail
        KC_MEDIASELECT: 0xED // Media Select
    };

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
        Keyboard: Keyboard,
        KeyCode: KeyCode
    };
});