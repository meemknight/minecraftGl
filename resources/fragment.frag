#version 330

layout (location = 0) out vec4 color;

uniform sampler2D u_texture;
in vec2 v_texCoord;

uniform float u_ambience;
uniform samplerCube u_skybox;

in float v_fogFactor;
in vec3 v_skyBoxpoisition;

void main()
{
	color = texture(u_texture, v_texCoord);
	
	if(color.a < 0.9)
	discard;


	color.rgb *= u_ambience;

	color.rgb = mix(color.rgb, texture(u_skybox, v_skyBoxpoisition).rgb, v_fogFactor); 

}