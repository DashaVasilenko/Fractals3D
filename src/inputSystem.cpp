#include "inputSystem.h"

bool InputSystem::keys[1024];
bool InputSystem::firstMouseMove = true;  
double InputSystem::lastCursPosX;
double InputSystem::lastCursPosY;
double InputSystem::deltaCursPosX;
double InputSystem::deltaCursPosY;