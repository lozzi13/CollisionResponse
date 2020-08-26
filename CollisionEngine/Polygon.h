#ifndef _POLYGON_H_
#define _POLYGON_H_

#include <GL/glew.h>
#include <vector>
#include <memory>


#include "Maths.h"
#include "BoundBox.h"


struct collisionPoint
{
	Vec2 colPoint;
	Vec2 colNormal;
	float colDist;
	collisionPoint(Vec2 _colPoint, Vec2 _colNormal, float _colDist) : colPoint(_colPoint), colNormal(_colNormal), colDist(_colDist)
	{}
};

class CPolygon
{
private:
	friend class CWorld;

	CPolygon(size_t index);
public:
	~CPolygon();

	Transform			transform;
	std::vector<Vec2>	points;

	void				Build();
	void				Draw();
	size_t				GetIndex() const;

	float				GetArea() const;

	float				GetMassInvers() const;


	Vec2				TransformPoint(const Vec2& point) const;
	Vec2				InverseTransformPoint(const Vec2& point) const;

	// if point is outside then returned distance is negative (and doesn't make sense)
	bool				IsPointInside(const Vec2& point) const;
	bool				CheckCollision(const CPolygon& poly, Vec2& colPoint, Vec2& colNormal, float& colDist) const;
	// Physics
	float				density;
	Vec2				speed;
	CBoundBox			Bound;
	void				DrawBoundBox(const float& r, const float& g, const float& b);
	bool				UpdateBound();
	float				Mass;
	float				Bouciness;

private:

	bool				FindCollisionNormal(const CPolygon& polyB, Vec2& colNormal, float& colDist, bool isA)const;
	Vec2				FindCollisionPoint(const CPolygon& poly, const Vec2& colNormal)const;
	void				GetBestProjectedValue(const Vec2& normal, float& polyMin, float& polyMax)const;
	Line				GetIncidentLine(const Vec2& normal)const;

	void				CreateBuffers();
	void				BindBuffers();
	void				DestroyBuffers();

	void				BuildLines();


	GLuint				m_vertexBufferId;
	size_t				m_index;

	std::vector<Line>	m_lines;
};

typedef std::shared_ptr<CPolygon>	CPolygonPtr;

#endif
