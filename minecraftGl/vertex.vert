#version 330

in layout (location = 0) vec4 positions;
in layout (location = 1) vec2 texCoord;

out vec2 v_texCoord;
uniform mat4 u_mat;

uniform vec2 u_index;
uniform float u_magnifier;

void main()
{
	v_texCoord = (texCoord + u_index) * u_magnifier;
	gl_Position = u_mat * positions;
}
