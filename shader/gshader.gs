#version 330 core
layout (points) in;
layout (line_strip, max_vertices = 4) out;
// layout (points, max_vertices = 1) out;
uniform mat4 g_model;
uniform mat4 g_view;
uniform mat4 g_projection;
uniform float g_t;

void main() {    
    
    
    vec4 pos = gl_in[0].gl_Position;
    vec4 p0 = g_projection * g_view * g_model * (pos + vec4(0,0,0.1 * sin(g_t + pos.x * 10) + 0.1 * sin(g_t + pos.y * 10),1));
    vec4 p1 = g_projection * g_view * g_model * (pos + vec4(0.01,0,0.1 * sin(g_t + (pos.x + 0.01) * 10) + 0.1 * sin(g_t + pos.y * 10),1));
    vec4 p2 = g_projection * g_view * g_model * (pos + vec4(0,0.01,0.1 * sin(g_t + pos.x * 10) + 0.1 * sin(g_t + (pos.y + 0.01) * 10),1));
    gl_Position = p0;
    EmitVertex();
    gl_Position = p1;
    EmitVertex();
    gl_Position = p0;
    EmitVertex();
    gl_Position = p2;
    EmitVertex();
    EndPrimitive();
}   