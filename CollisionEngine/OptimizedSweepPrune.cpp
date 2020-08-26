#include "OptimizedSweepPrune.h"
#include <algorithm> 
#include "GlobalVariables.h"
#include "World.h"

COptimizedSweepPrune::COptimizedSweepPrune()
{
}

COptimizedSweepPrune::~COptimizedSweepPrune()
{
	XEndPoints.clear();
	YEndPoints.clear();
	m_pairs.clear();
}

void COptimizedSweepPrune::AddPolygone(CPolygonPtr polygone)
{
	EndPoint maxEndPoint = EndPoint(polygone, polygone->Bound.Max()[0], EndPointType::MaxValue);
	EndPoint minEndPoint = EndPoint(polygone, polygone->Bound.Min()[0], EndPointType::MinValue);
	XEndPoints.emplace_back(minEndPoint);
	XEndPoints.emplace_back(maxEndPoint);
	Sort(XEndPoints, 0);

	maxEndPoint = EndPoint(polygone, polygone->Bound.Max()[1], EndPointType::MaxValue);
	minEndPoint = EndPoint(polygone, polygone->Bound.Min()[1], EndPointType::MinValue);
	YEndPoints.emplace_back(minEndPoint);
	YEndPoints.emplace_back(maxEndPoint);
	Sort(YEndPoints, 1);
}

void COptimizedSweepPrune::UpdatePolygone()
{
	size_t nbWorldPoly = gVars->pWorld->GetPolygonCount();
	for (size_t index = 0; index < nbWorldPoly; ++index)
	{
		CPolygonPtr polygone = gVars->pWorld->GetPolygon(index);
		polygone->UpdateBound();
	}
	Sort(XEndPoints, 0);
	Sort(YEndPoints, 1);
}

void COptimizedSweepPrune::RemovePolygone(CPolygonPtr polygone)
{
	for (std::unordered_set<SPolygonPair, SPolygonPair_Hash>::iterator pair = m_pairs.begin(); pair != m_pairs.end(); pair++)
	{
		if (polygone == pair->polyA || polygone == pair->polyB)
			m_pairs.erase(pair);
	}

	for (std::vector<EndPoint>::iterator endPoint = XEndPoints.begin(); endPoint != XEndPoints.end(); endPoint++)
	{
		if (polygone == endPoint->Box)
			XEndPoints.erase(endPoint);
	}

	for (std::vector<EndPoint>::iterator endPoint = YEndPoints.begin(); endPoint != YEndPoints.end(); endPoint++)
	{
		if (polygone == endPoint->Box)
			YEndPoints.erase(endPoint);
	}
}

void COptimizedSweepPrune::GetCollidingPairsToCheck(std::vector<SPolygonPair>& pairsToCheck)
{
	UpdatePolygone();

	for (std::unordered_set<SPolygonPair, SPolygonPair_Hash>::iterator pair = m_pairs.begin(); pair != m_pairs.end(); pair++)
	{
		pairsToCheck.emplace_back((*pair));
	}
}

void  COptimizedSweepPrune::AddToBroadPhase(CPolygonPtr polygone)
{
	AddPolygone(polygone);
}

void COptimizedSweepPrune::Sort(std::vector<EndPoint>& EndPoints, int axis)
{
	for (int i = 0; i < EndPoints.size(); i++)
	{
		CPolygonPtr polygone = gVars->pWorld->GetPolygon(EndPoints[i].Index);

		EndPoints[i].Value = EndPoints[i].IsEnding() ? polygone->Bound.Max()[axis] : polygone->Bound.Min()[axis];

		EndPoint& key = EndPoints[i];
		int j = i - 1;
		while (j >= 0 && key < EndPoints[j])
		{
			EndPoint& ep0 = EndPoints[j];
			EndPoint& ep1 = EndPoints[j + 1];

			if (ep0.IsBeginning())
			{
				if (ep1.IsEnding())
				{
					m_pairs.erase(SPolygonPair(ep0.Box, ep1.Box));
				}
			}
			else
			{
				if (ep1.IsBeginning())
				{
					if (ep0.Index != ep1.Index && ep1.Box->Bound.Overlap(ep0.Box->Bound))
					{
						m_pairs.emplace(SPolygonPair(ep0.Box, ep1.Box));
					}
				}
			}
			std::swap(EndPoints[j], EndPoints[j + 1]);
			--j;
		}
	}
}
