#version 410

in vec3 vertex_position;
in vec2 vt;

uniform mat4 mvp;

out vec2 texture_coordinates;

void main() 
{
	texture_coordinates = vt;
    gl_Position = mvp * vertex_position;
} 
