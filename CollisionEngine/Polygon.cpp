#include "Polygon.h"
#include <GL/glu.h>
#include "Maths.h"
#include "SceneManager.h"
#include "PhysicEngine.h"
#include <limits> 
#include <string>
#include <iostream>

CPolygon::CPolygon(size_t index)
	: m_vertexBufferId(0), m_index(index), density(0.1f), Mass(1.0f), Bouciness(1.0f)
{
	Mass = 1.0f;//static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
	Bouciness = 0.8f;//static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

CPolygon::~CPolygon()
{
	DestroyBuffers();
}

void CPolygon::Build()
{
	m_lines.clear();

	CreateBuffers();
	BuildLines();
}

void CPolygon::Draw()
{
	// Set transforms (qssuming model view mode is set)
	float transfMat[16] = { transform.GetRotation().X.x, transform.GetRotation().X.y, 0.0f, 0.0f,
						transform.GetRotation().Y.x, transform.GetRotation().Y.y, 0.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f,
						transform.GetPosition().x, transform.GetPosition().y, -1.0f, 1.0f };
	glPushMatrix();
	glMultMatrixf(transfMat);

	// Draw vertices
	BindBuffers();
	glDrawArrays(GL_LINE_LOOP, 0, points.size());
	glDisableClientState(GL_VERTEX_ARRAY);


	glPopMatrix();
}

size_t	CPolygon::GetIndex() const
{
	return m_index;
}

Vec2	CPolygon::TransformPoint(const Vec2& point) const
{
	return transform.GetPosition() + transform.GetRotation() * point;
}

Vec2	CPolygon::InverseTransformPoint(const Vec2& point) const
{
	return transform.GetRotation().GetInverse() * (point - transform.GetPosition());
}

bool	CPolygon::IsPointInside(const Vec2& point) const
{
	float maxDist = -FLT_MAX;

	for (const Line& line : m_lines)
	{
		Line globalLine = line.Transform(transform.GetRotation(), transform.GetPosition());
		float pointDist = globalLine.GetPointDist(point);
		maxDist = Max(maxDist, pointDist);
	}

	return maxDist <= 0.0f;
}

bool	CPolygon::CheckCollision(const CPolygon& poly, Vec2& colPoint, Vec2& colNormal, float& colDist) const
{
	Vec2 normal(0, 0);
	float minPen = FLT_MAX;

	if (!FindCollisionNormal(poly, normal, minPen, true))
	{
		return false;
	}
	if (!poly.FindCollisionNormal(*this, normal, minPen, false))
	{
		return false;
	}

	colPoint = FindCollisionPoint(poly, normal);
	colNormal = normal;
	colDist = minPen;
	return true;
}

Vec2 CPolygon::FindCollisionPoint(const CPolygon& poly, const Vec2& colNormal)const
{
	Vec2 colPoint;
	Line referenceEdge = GetIncidentLine(colNormal * -1);
	Line incidentLine = poly.GetIncidentLine(colNormal);

	Line SidePlane1(referenceEdge.point, colNormal, 25.0f);
	Line SidePlane2(referenceEdge.GetEndPoint(), colNormal * -1, 25.0f);

	if (gVars->bDebug && gVars->pSceneManager->GetCurrentSceneIndex() == 0)
	{
		gVars->pRenderer->DrawLine(referenceEdge.point + (referenceEdge.dir  * -1)  * referenceEdge.length, referenceEdge.GetEndPoint() + referenceEdge.dir * referenceEdge.length, 0.71f, 0.0f, 0.0f);
		gVars->pRenderer->DrawLine(incidentLine.point + (incidentLine.dir  * -1)  * incidentLine.length, incidentLine.GetEndPoint() + incidentLine.dir * incidentLine.length, 0.35f, 0.62f, 0.35f);
		gVars->pRenderer->DrawLine(SidePlane1.point + (SidePlane1.dir  * -1)  * SidePlane1.length, SidePlane1.GetEndPoint() + SidePlane1.dir * SidePlane1.length, 0.0f, 0.62f, 0.62f);
		gVars->pRenderer->DrawLine(SidePlane2.point + (SidePlane2.dir  * -1)  * SidePlane2.length, SidePlane2.GetEndPoint() + SidePlane2.dir * SidePlane2.length, 0.0f, 0.62f, 0.62f);
	}

	Vec2 incidentPoint1 = Line::Clip(SidePlane2, incidentLine);
	Vec2 incidentPoint2 = Line::Clip(SidePlane1, incidentLine.Inverse());

	float point1DistRef = referenceEdge.GetPointDist(incidentPoint1);
	float point2DistRef = referenceEdge.GetPointDist(incidentPoint2);

	//if(point1DistRef <= 0.0f  && point2DistRef <= 0.0f) both of them are collision point
	//to see what will we have to do in that case for collision response
	if (point1DistRef <= 0.0f && point1DistRef < point2DistRef)
	{
		colPoint = incidentPoint1;
	}
	else if (point2DistRef <= 0.0f)
	{
		colPoint = incidentPoint2;
	}
	else
	{
		float alpha = point1DistRef / (point1DistRef - point2DistRef);
		colPoint = incidentPoint1 + incidentLine.dir * alpha;
	}

	return colPoint;
}

bool CPolygon::FindCollisionNormal(const CPolygon& polyB, Vec2& colNormal, float& colDist, bool isA)const
{
	for (size_t index = 0; index < m_lines.size(); ++index)
	{
		float polyAMin = FLT_MAX;
		float polyAMax = -FLT_MAX;
		float polyBMin = FLT_MAX;
		float polyBMax = -FLT_MAX;

		Line referenceEdge = m_lines[index].Transform(transform.GetRotation(), transform.GetPosition());

		Vec2 tempNormal = referenceEdge.GetNormal();

		GetBestProjectedValue(tempNormal, polyAMin, polyAMax);
		polyB.GetBestProjectedValue(tempNormal, polyBMin, polyBMax);

		if (polyAMax < polyBMin || polyBMax < polyAMin)
		{
			return false;
		}

		float minDist = abs(polyAMax - polyBMin);

		if (minDist <= colDist)
		{
			colNormal = isA ? tempNormal : tempNormal * -1;
			colDist = minDist + FLT_EPSILON;
		}
	}
	return true;
}

void CPolygon::GetBestProjectedValue(const Vec2& normal, float& polyMin, float& polyMax)const
{
	for (size_t i = 0; i < points.size(); i++)
	{
		float projectedValue = normal | TransformPoint(points[i]);

		polyMin = Min(polyMin, projectedValue);
		polyMax = Max(polyMax, projectedValue);
	}
}

Line CPolygon::GetIncidentLine(const Vec2& normal)const
{
	float bestPene = FLT_MAX;
	Line incident;
	for (size_t i = 0; i < m_lines.size(); i++)
	{
		float tempPene = m_lines[i].Transform(transform.GetRotation(), transform.GetPosition()).GetNormal() | normal;
		if (tempPene < bestPene)
		{
			incident = m_lines[i].Transform(transform.GetRotation(), transform.GetPosition());
			bestPene = tempPene;
		}
	}
	return incident;
}

void CPolygon::CreateBuffers()
{
	DestroyBuffers();

	float* vertices = new float[3 * points.size()];
	for (size_t i = 0; i < points.size(); ++i)
	{
		vertices[3 * i] = points[i].x;
		vertices[3 * i + 1] = points[i].y;
		vertices[3 * i + 2] = 0.0f;
	}

	glGenBuffers(1, &m_vertexBufferId);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * points.size(), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete[] vertices;
}

void CPolygon::BindBuffers()
{
	if (m_vertexBufferId != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, (void*)0);
	}
}


void CPolygon::DestroyBuffers()
{
	if (m_vertexBufferId != 0)
	{
		glDeleteBuffers(1, &m_vertexBufferId);
		m_vertexBufferId = 0;
	}
}

void CPolygon::BuildLines()
{
	for (size_t index = 0; index < points.size(); ++index)
	{
		const Vec2& pointA = points[index];
		const Vec2& pointB = points[(index + 1) % points.size()];

		Vec2 lineDir = (pointA - pointB).Normalized();
		float lineLength = (pointA - pointB).GetLength();

		m_lines.push_back(Line(pointB, lineDir, lineLength));
	}
}

bool CPolygon::UpdateBound()
{
	return Bound.UpdateBound(transform, points);
}

void CPolygon::DrawBoundBox(const float& r, const float& g, const float& b)
{
	Bound.Draw(r, g, b);
}

float CPolygon::GetMassInvers() const
{
	return Mass == 0.0f	|| Mass == FLT_MAX ? 0.0f : 1 / Mass;
}
