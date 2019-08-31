#version 330

in layout (location = 0) vec4 positions;
in layout (location = 1) vec2 texCoord;

out vec2 v_texCoord;
uniform mat4 u_mat;

void main()
{
	v_texCoord = texCoord;
	gl_Position = positions * u_mat;
}
