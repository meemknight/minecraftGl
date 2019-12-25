#include "Frustum.h"
#include "tools.h"

/*
	I don't own this code
	https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
*/

glm::vec3 intersectPlane(const Plane & p1, const Plane & p2, const Plane & p3)
{
	glm::mat3 m = { p1.vect, p2.vect, p3.vect };
	glm::vec3 p = { p1.d, p2.d, p3.d};

	m = glm::inverse(m);

	return m * p;
}

void NormalizePlane(Plane & plane)
{
	float mag;
	mag = sqrt(plane.a * plane.a + plane.b * plane.b + plane.c * plane.c);
	plane.a = plane.a / mag;
	plane.b = plane.b / mag;
	plane.c = plane.c / mag;
	plane.d = plane.d / mag;
}

enum Planes
{
	Left,
	Right,
	Top,
	Bottom,
	Near,
	Far,
};


void ExtractPlanesGL(
	const Matrix4x4 & comboMatrix,
	glm::vec2 &min, glm::vec2 &max)
{
	Plane p_planes[6];

	// Left clipping plane
	p_planes[0].a = comboMatrix._41 + comboMatrix._11;
	p_planes[0].b = comboMatrix._42 + comboMatrix._12;
	p_planes[0].c = comboMatrix._43 + comboMatrix._13;
	p_planes[0].d = comboMatrix._44 + comboMatrix._14;
	// Right clipping plane
	p_planes[1].a = comboMatrix._41 - comboMatrix._11;
	p_planes[1].b = comboMatrix._42 - comboMatrix._12;
	p_planes[1].c = comboMatrix._43 - comboMatrix._13;
	p_planes[1].d = comboMatrix._44 - comboMatrix._14;
	// Top clipping plane
	p_planes[2].a = comboMatrix._41 - comboMatrix._21;
	p_planes[2].b = comboMatrix._42 - comboMatrix._22;
	p_planes[2].c = comboMatrix._43 - comboMatrix._23;
	p_planes[2].d = comboMatrix._44 - comboMatrix._24;
	// Bottom clipping plane
	p_planes[3].a = comboMatrix._41 + comboMatrix._21;
	p_planes[3].b = comboMatrix._42 + comboMatrix._22;
	p_planes[3].c = comboMatrix._43 + comboMatrix._23;
	p_planes[3].d = comboMatrix._44 + comboMatrix._24;
	// Near clipping plane
	p_planes[4].a = comboMatrix._41 + comboMatrix._31;
	p_planes[4].b = comboMatrix._42 + comboMatrix._32;
	p_planes[4].c = comboMatrix._43 + comboMatrix._33;
	p_planes[4].d = comboMatrix._44 + comboMatrix._34;
	// Far clipping plane
	p_planes[5].a = comboMatrix._41 - comboMatrix._31;
	p_planes[5].b = comboMatrix._42 - comboMatrix._32;
	p_planes[5].c = comboMatrix._43 - comboMatrix._33;
	p_planes[5].d = comboMatrix._44 - comboMatrix._34;
	// Normalize the plane equations, if requested
	
	NormalizePlane(p_planes[0]);
	NormalizePlane(p_planes[1]);
	NormalizePlane(p_planes[2]);
	NormalizePlane(p_planes[3]);
	NormalizePlane(p_planes[4]);
	NormalizePlane(p_planes[5]);

	glm::vec3 points[8];

	points[0] = intersectPlane(p_planes[Planes::Far], p_planes[Planes::Top],    p_planes[Planes::Left]);
	points[1] = intersectPlane(p_planes[Planes::Far], p_planes[Planes::Bottom], p_planes[Planes::Right]);
	points[2] = intersectPlane(p_planes[Planes::Far], p_planes[Planes::Top],	p_planes[Planes::Right]);
	points[3] = intersectPlane(p_planes[Planes::Far], p_planes[Planes::Bottom], p_planes[Planes::Left]);
	points[4] = intersectPlane(p_planes[Planes::Near], p_planes[Planes::Top],	p_planes[Planes::Left]);
	points[5] = intersectPlane(p_planes[Planes::Near], p_planes[Planes::Bottom],p_planes[Planes::Right]);
	points[6] = intersectPlane(p_planes[Planes::Near], p_planes[Planes::Top],	p_planes[Planes::Right]);
	points[7] = intersectPlane(p_planes[Planes::Near], p_planes[Planes::Bottom],p_planes[Planes::Left]);

	min.x = points[0].x;
	min.y = points[0].z;
	max.x = points[0].x;
	max.y = points[0].z;

	for (int i = 1; i < 8; i++)
	{
		if (points[i].x < min.x)
		{
			min.x = points[i].x;
		}

		if (points[i].z < min.y)
		{
			min.y = points[i].z;
		}

		if (points[i].x > max.x)
		{
			max.x = points[i].x;
		}

		if (points[i].z > max.y)
		{
			max.y = points[i].z;
		}
	}
	
}