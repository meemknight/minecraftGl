#version 330

layout (location = 0) in vec4 model;
layout (location = 1) in vec3 translation;
layout (location = 2) in vec4 color;

out vec4 v_color;
uniform mat4 u_mat;

void main()
{
	v_color = color;
	gl_Position = u_mat * (vec4(model.xyz,1) + vec4(translation,0));
}