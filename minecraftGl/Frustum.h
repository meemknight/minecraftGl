#pragma once

/*
	I don't own this code
	https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
*/

#include <array>
#include <glm/glm.hpp>


struct Plane
{
	union 
	{
		struct 
		{
			glm::vec3 vect;
			float d2;
		};
		struct 
		{
			float a, b, c, d;
		};
	};
};

struct Matrix4x4
{
	// The elements of the 4x4 matrix are stored in
	// column-major order (see "OpenGL Programming Guide",
	// 3rd edition, pp 106, glLoadMatrix).
	float _11, _21, _31, _41;
	float _12, _22, _32, _42;
	float _13, _23, _33, _43;
	float _14, _24, _34, _44;
	
	Matrix4x4() {};
	Matrix4x4(glm::mat4 m)
	{
		_11 = m[0][0];
		_21 = m[1][0];
		_31 = m[2][0];
		_41 = m[3][0];

		_12 = m[0][1];
		_22 = m[1][1];
		_32 = m[2][1];
		_42 = m[3][1];

		_13 = m[0][2];
		_23 = m[1][2];
		_33 = m[2][2];
		_43 = m[3][2];

		_14 = m[0][3];
		_24 = m[1][3];
		_34 = m[2][3];
		_44 = m[3][3];
	}
};

void NormalizePlane(Plane & plane);
void ExtractPlanesGL(
	const Matrix4x4 & comboMatrix,
	glm::vec2 &min, glm::vec2 &max);

