#version 330

layout (location = 0) in vec4 model;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 translation;
layout (location = 3) in vec2 texIndex;

out vec2 v_texCoord;
uniform mat4 u_mat;

uniform float u_magnifier;

void main()
{
	v_texCoord = (texCoord + texIndex) * u_magnifier;
	gl_Position = u_mat * (model + vec4(translation,0));
}
