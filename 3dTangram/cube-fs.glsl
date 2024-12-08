#version 330 core

in vec3 exPosition;
in vec2 exTexcoord;
in vec3 exNormal;

out vec4 FragmentColor;

uniform vec3 baseColor;

void main(void)
{
    vec3 N = normalize(exNormal);
    
    vec3 colorTint = 0.1 * N;
    
    vec3 modifiedColor = baseColor + colorTint;

    modifiedColor = clamp(modifiedColor, 0.0, 1.0);

    FragmentColor = vec4(modifiedColor, 1.0);
}
