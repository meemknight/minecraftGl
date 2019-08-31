#version 330 core

out layout (location = 0) vec4 color;

uniform sampler2D u_texture;
in vec2 v_texCoord;

void main()
{
	color = texture(u_texture, v_texCoord);
}