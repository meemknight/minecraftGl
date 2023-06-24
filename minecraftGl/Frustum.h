#pragma once
#include <glm/glm.hpp>

void generateTangentSpace(glm::vec3 v, glm::vec3& outUp, glm::vec3& outRight);

//https://www.youtube.com/watch?v=lUo7s-i9Gy4&ab_channel=OREONENGINE
void computeFrustumDimensions(
	glm::vec3 position, glm::vec3 viewDirection,
	float fovRadians, float aspectRatio, float nearPlane, float farPlane,
	glm::vec2& nearDimensions, glm::vec2& farDimensions, glm::vec3& centerNear, glm::vec3& centerFar);

void computeFrustumSplitCorners(glm::vec3 directionVector,
	glm::vec2 nearDimensions, glm::vec2 farDimensions, glm::vec3 centerNear, glm::vec3 centerFar,
	glm::vec3& nearTopLeft, glm::vec3& nearTopRight, glm::vec3& nearBottomLeft, glm::vec3& nearBottomRight,
	glm::vec3& farTopLeft, glm::vec3& farTopRight, glm::vec3& farBottomLeft, glm::vec3& farBottomRight);