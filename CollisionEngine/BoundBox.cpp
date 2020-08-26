#pragma once

#include <limits.h>
#include "Renderer.h"
#include "GlobalVariables.h"
#include "BoundBox.h"
#include  <cmath>
int  CBoundBox::Axis = 0;

bool	CBoundBox::UpdateBound(Transform& trans, const std::vector<Vec2>& points)
{
	if (trans.HasChanged())
	{
		Vec2 Min(FLT_MAX, FLT_MAX);
		Vec2 Max(-FLT_MAX, -FLT_MAX);
		for (size_t index = 0; index < points.size(); ++index)
		{
			Vec2 point = trans.GetRotation() * points[index];

			if (Min.y > point.y)
				Min.y = point.y;
			if (Max.y < point.y)
				Max.y = point.y;

			if (Min.x > point.x)
				Min.x = point.x;
			if (Max.x < point.x)
				Max.x = point.x;
		}
		Diagonal = Vec2(Max - Min) * 0.5f;

		center = Vec2(Max + Min) * 0.5f + trans.GetPosition();
		trans.HasChanged(false);
		return true;
	}
	return false;
}

Vec2 CBoundBox::GetDiagonal()const
{
	return Diagonal;
}

void CBoundBox::Draw(const float& r, const float& g, const float& b)
{
	gVars->pRenderer->DrawLine(Vec2(Diagonal.x, Diagonal.y) + center, Vec2(-Diagonal.x, Diagonal.y) + center, r, g, b);
	gVars->pRenderer->DrawLine(Vec2(-Diagonal.x, Diagonal.y) + center, Vec2(-Diagonal.x, -Diagonal.y) + center, r, g, b);
	gVars->pRenderer->DrawLine(Vec2(-Diagonal.x, -Diagonal.y) + center, Vec2(Diagonal.x, -Diagonal.y) + center, r, g, b);
	gVars->pRenderer->DrawLine(Vec2(Diagonal.x, -Diagonal.y) + center, Vec2(Diagonal.x, Diagonal.y) + center, r, g, b);
	gVars->pRenderer->DrawLine(Diagonal + center, center, r, g, b);
}

Vec2 CBoundBox::GetCenter()const
{
	return center;
}

Vec2 CBoundBox::Max()const
{
	return (Diagonal + center);
}

Vec2 CBoundBox::Min()const
{
	return (center - Diagonal);
}

bool CBoundBox::operator <(CBoundBox const& b)const
{
	switch (Axis)
	{
	case 0:
		return Min().x < b.Min().x;
		break;
	case 1:
		return Min().y < b.Min().y;
		break;
	default:
		return false;
		break;
	}
}

bool CBoundBox::Overlap(CBoundBox box2)
{
	bool overlapedX{ std::abs(center.x - box2.center.x) < (Diagonal.x + box2.Diagonal.x) };
	bool overlapedY{ std::abs(center.y - box2.center.y) < (Diagonal.y + box2.Diagonal.y) };
	return overlapedX && overlapedY;
}


bool CBoundBox::OverlapY(CBoundBox box2)
{
	return std::abs(center.y - box2.center.y) < (Diagonal.y + box2.Diagonal.y);
}


bool CBoundBox::OverlapX(CBoundBox box2)
{
	return  std::abs(center.x - box2.center.x) < (Diagonal.x + box2.Diagonal.x);
}
