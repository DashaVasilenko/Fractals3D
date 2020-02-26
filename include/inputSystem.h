#ifndef __INPUT_SYSTEM__
#define __INPUT_SYSTEM__

class InputSystem {
public:
    static bool keys[1024];
    static bool firstMouseMove;  
    static double lastCursPosX;
    static double lastCursPosY;
    static double deltaCursPosX;
    static double deltaCursPosY;
private:
};

#endif /* End of __INPUT_SYSTEM__ */