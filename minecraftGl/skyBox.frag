#version 330

out vec4 a_outColor;

in vec3 v_texCoords;

uniform samplerCube u_skybox;


void main()
{    
	a_outColor = texture(u_skybox, v_texCoords);
}