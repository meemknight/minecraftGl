#include "Frustum.h"
#include "tools.h"

void generateTangentSpace(glm::vec3 v, glm::vec3& outUp, glm::vec3& outRight)
{
	glm::vec3 up(0, 1, 0);

	if (v == up)
	{
		outRight = glm::vec3(1, 0, 0);
	}
	else
	{
		outRight = normalize(glm::cross(v, up));
	}

	outUp = normalize(cross(outRight, v));

}

void computeFrustumDimensions(
	glm::vec3 position, glm::vec3 viewDirection,
	float fovRadians, float aspectRatio, float nearPlane, float farPlane,
	glm::vec2& nearDimensions, glm::vec2& farDimensions, glm::vec3& centerNear, glm::vec3& centerFar)
{
	float tanFov2 = tan(fovRadians) * 2;

	nearDimensions.y = tanFov2 * nearPlane;			//hNear
	nearDimensions.x = nearDimensions.y * aspectRatio;	//wNear

	farDimensions.y = tanFov2 * farPlane;			//hNear
	farDimensions.x = farDimensions.y * aspectRatio;	//wNear

	centerNear = position + viewDirection * farPlane;
	centerFar = position + viewDirection * farPlane;

}

//https://www.youtube.com/watch?v=lUo7s-i9Gy4&ab_channel=OREONENGINE
void computeFrustumSplitCorners(glm::vec3 directionVector,
	glm::vec2 nearDimensions, glm::vec2 farDimensions, glm::vec3 centerNear, glm::vec3 centerFar,
	glm::vec3& nearTopLeft, glm::vec3& nearTopRight, glm::vec3& nearBottomLeft, glm::vec3& nearBottomRight,
	glm::vec3& farTopLeft, glm::vec3& farTopRight, glm::vec3& farBottomLeft, glm::vec3& farBottomRight)
{

	glm::vec3 rVector = {};
	glm::vec3 upVectpr = {};

	generateTangentSpace(directionVector, upVectpr, rVector);

	nearTopLeft = centerNear + upVectpr * nearDimensions.y / 2.f - rVector * nearDimensions.x / 2.f;
	nearTopRight = centerNear + upVectpr * nearDimensions.y / 2.f + rVector * nearDimensions.x / 2.f;
	nearBottomLeft = centerNear - upVectpr * nearDimensions.y / 2.f - rVector * nearDimensions.x / 2.f;
	nearBottomRight = centerNear - upVectpr * nearDimensions.y / 2.f + rVector * nearDimensions.x / 2.f;

	farTopLeft = centerNear + upVectpr * farDimensions.y / 2.f - rVector * farDimensions.x / 2.f;
	farTopRight = centerNear + upVectpr * farDimensions.y / 2.f + rVector * farDimensions.x / 2.f;
	farBottomLeft = centerNear - upVectpr * farDimensions.y / 2.f - rVector * farDimensions.x / 2.f;
	farBottomRight = centerNear - upVectpr * farDimensions.y / 2.f + rVector * farDimensions.x / 2.f;

}