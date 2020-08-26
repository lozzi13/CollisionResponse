#pragma once

#include <limits.h>
#include "Renderer.h"
#include "GlobalVariables.h"

class CBoundBox
{
private:
	Vec2 center = Vec2();
	Vec2 Diagonal = Vec2();

	float angle = 0.0f;
public:
	static int Axis;

	CBoundBox() = default;

	bool	UpdateBound(Transform& trans, const std::vector<Vec2>& points);

	Vec2 GetDiagonal()const;

	void Draw(const float& r, const float& g, const float& b);

	Vec2 GetCenter()const;

	Vec2 Max()const;

	Vec2 Min()const;

	bool operator <(CBoundBox const& b)const;

	bool Overlap(CBoundBox b);
	bool OverlapY(CBoundBox b);
	bool OverlapX(CBoundBox b);
};
