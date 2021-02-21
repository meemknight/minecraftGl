#version 330

layout (location = 0) in vec4 model;			//actually vec3
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 translation;
layout (location = 3) in vec2 texIndex;

out vec2 v_texCoord;
out float v_fogFactor;
out vec3 v_skyBoxpoisition;

uniform mat4 u_mat;
uniform mat4 u_skyBoxMat;

uniform vec3 u_cameraPosition;

uniform float u_magnifier;

uniform float u_distance;


void calculateFogFactor()
{

	float distanceFromCamera = length(u_cameraPosition - (vec3(model) + translation));

	v_fogFactor = distanceFromCamera / u_distance;
	v_fogFactor = clamp(v_fogFactor, 0, 1);

}

void main()
{
	
	calculateFogFactor();

	const float density = 2;
	const float gradient = 64;
	
	v_fogFactor = 1 - v_fogFactor;
	v_fogFactor = exp(-pow(v_fogFactor*density, gradient));
	//v_fogFactor = clamp(v_fogFactor, 0, 1);

	v_skyBoxpoisition = vec3
	(
	//u_skyBoxMat * 
		-(
			vec4(u_cameraPosition, 0) - (vec4(model) + vec4(translation,0))
		)
	);

	v_texCoord = (texCoord + texIndex) * u_magnifier;
	gl_Position = u_mat * (model + vec4(translation,0));

}
