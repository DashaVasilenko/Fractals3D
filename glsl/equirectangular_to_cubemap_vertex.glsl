//#version 330 core

layout (location = 0) in vec3 position;

out vec3 localPosition;

uniform mat4 projection;
uniform mat4 view;

void main() {
    localPosition = position;  
    gl_Position =  projection * view * vec4(localPosition, 1.0);
}