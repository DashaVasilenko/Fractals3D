#ifndef __INPUT_SYSTEM__
#define __INPUT_SYSTEM__

enum MouseButton {
    LeftButton = 0,    
    RightButton = 1,
    MiddleButton = 2,
    Button4 = 3,
    Button5 = 4,
    Button6 = 5,
    Button7 = 6,
    Button8 = 7
};

class InputSystem {
public:
    static bool keys[1024];
    static bool mouse[8];
    static bool scroll;
    static bool firstMouseMove;  
    static double lastCursPosX;
    static double lastCursPosY;
    static double deltaCursPosX;
    static double deltaCursPosY;
    static double scrollOffsetX;
    static double scrollOffsetY;
    //static float fieldOfView;
private:
};

#endif /* End of __INPUT_SYSTEM__ */