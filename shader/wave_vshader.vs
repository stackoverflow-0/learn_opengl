#version 330 core
layout (location = 0) in vec3 aPos;

out vec4 pos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float t;

void main()
{
    float x = gl_VertexID / 100;
    float y = gl_VertexID % 100;
    vec3 tpos = aPos + vec3(0,0,0.1 * sin(t + x/10.0) + 0.1 * sin(t + y/10.0));
    gl_Position =  projection * view * model * vec4(tpos, 1.0);
    pos = gl_Position;
}