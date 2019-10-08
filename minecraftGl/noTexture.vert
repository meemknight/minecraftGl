#version 330 core

in layout (location = 0) vec4 model;
in layout (location = 1) vec3 translation;
in layout (location = 2) vec4 color;

out vec4 v_color;
uniform mat4 u_mat;

void main()
{
	v_color = color;
	gl_Position = u_mat * (vec4(model.xyz,1) + vec4(translation,0));
}