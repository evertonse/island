#pragma once

namespace cyx {

    enum Key {
		INVALID,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		ARROWUP, ARROWDOWN, ARROWLEFT, ARROWRIGHT,
		SPACE, TAB, SHIFT, CTRL,
		ESCAPE, RETURN, ENTER,
        KEY_COUNT
	};	

    enum MouseButton {
        LEFT = 0,		
        RIGHT = 1,		
        MIDDLE = 2	
    };

    struct MouseWheel {
        int dir = 0;
    };

    struct Mouse {
        MouseButton button;
        MouseWheel wheel;
        int x = 0, y = 0 ;
    };

    enum EventType {
        QUIT,
        RESHAPE,
        MOUSEUP,
        MOUSEDOWN,
        MOUSEMOVE,
        MOUSEWHEEL,
        KEYUP,
        KEYDOWN,
    };

    struct Event {
        EventType   type;
        Mouse       mouse;
        Key         key;
    };

};