#version 330

in layout (location = 0) vec4 model;
in layout (location = 1) vec2 texCoord;
in layout (location = 2) vec3 translation;
in layout (location = 3) vec2 texIndex;

out vec2 v_texCoord;
uniform mat4 u_mat;

uniform float u_magnifier;

void main()
{
	v_texCoord = (texCoord + texIndex) * u_magnifier;
	gl_Position = u_mat * (model + vec4(translation,0));
}
