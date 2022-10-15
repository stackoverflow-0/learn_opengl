#version 330 core
out vec4 FragColor;

in vec4 pos;

uniform float sphere_index;
void main()
{
    FragColor = vec4( sphere_index / 3.0, 1.0 ,sphere_index / 3.0,1.0);
}