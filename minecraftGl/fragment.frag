#version 330

layout (location = 0) out vec4 color;

uniform sampler2D u_texture;
in vec2 v_texCoord;

uniform float u_ambience;

void main()
{
	color = texture(u_texture, v_texCoord);
	
	if(color.a < 0.9)
	discard;

	color.rgb *= u_ambience;
}