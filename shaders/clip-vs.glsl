#version 330 core

in vec4 inPosition;
in vec4 inColor;
out vec4 exColor;

uniform mat4 Matrix;
uniform vec4 objectColor; 

void main(void) {
    gl_Position = Matrix * inPosition;
    exColor = objectColor;
}
