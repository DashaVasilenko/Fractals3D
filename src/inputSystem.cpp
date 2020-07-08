#include "inputSystem.h"

bool InputSystem::keys[1024] = {false};
bool InputSystem::mouse[8] = {false};
bool InputSystem::scroll = false;
bool InputSystem::firstMouseMove = true;  
double InputSystem::lastCursPosX;
double InputSystem::lastCursPosY;
double InputSystem::deltaCursPosX;
double InputSystem::deltaCursPosY;
double InputSystem::scrollOffsetX;
double InputSystem::scrollOffsetY;
//float InputSystem::fieldOfView = 45.0f;