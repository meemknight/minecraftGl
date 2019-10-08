#version 330 core

out layout(location = 0) vec4 color;

in vec4 v_color;

void main()
{
	color = v_color;
}