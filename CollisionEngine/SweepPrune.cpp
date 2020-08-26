#include "SweepPrune.h"
#include <algorithm> 
#include "GlobalVariables.h"
#include "World.h"

CSweepPrune::CSweepPrune()
{
}

CSweepPrune::~CSweepPrune()
{
	XEndPoints.clear();
	XEndPointsTuple.clear();
}

void CSweepPrune::AddPolygone(CPolygonPtr polygone)
{
	EndPoint maxEndPoint = EndPoint(polygone, polygone->Bound.Max()[0], EndPointType::MaxValue);
	EndPoint minEndPoint = EndPoint(polygone, polygone->Bound.Min()[0], EndPointType::MinValue);
	std::list<EndPoint>::iterator max = XEndPoints.insert(std::upper_bound(XEndPoints.begin(), XEndPoints.end(), maxEndPoint), maxEndPoint);
	std::list<EndPoint>::iterator min = XEndPoints.insert(std::lower_bound(XEndPoints.begin(), XEndPoints.end(), minEndPoint), minEndPoint);

	EndPointTuple tuple;
	tuple.Max = max;
	tuple.Min = min;
	XEndPointsTuple.emplace(polygone, tuple);
}

void CSweepPrune::UpdatePolygone()
{
	size_t nbWorldPoly = gVars->pWorld->GetPolygonCount();
	for (size_t index = 0; index < nbWorldPoly; ++index)
	{
		CPolygonPtr polygone = gVars->pWorld->GetPolygon(index);
		if (polygone->UpdateBound())
		{
			EndPointTuple tuple = XEndPointsTuple[polygone];
			(*tuple.Max).Value = polygone->Bound.Max().x;
			(*tuple.Min).Value = polygone->Bound.Min().x;
		}
	}
	XEndPoints.sort();
}

void CSweepPrune::RemovePolygone(CPolygonPtr polygone)
{
	EndPointTuple RemoveTuple = XEndPointsTuple[polygone];
	XEndPoints.erase(RemoveTuple.Min);
	XEndPoints.erase(RemoveTuple.Max);
	XEndPointsTuple.erase(polygone);
}

void CSweepPrune::GetCollidingPairsToCheck(std::vector<SPolygonPair>& pairsToCheck)
{
	UpdatePolygone();
	std::unordered_set<CPolygonPtr> currentPolygons;

	for (std::list<EndPoint>::iterator endPoint = XEndPoints.begin(); endPoint != XEndPoints.end(); endPoint++)
	{
		if ((*endPoint).Type == EndPointType::MinValue)
		{
			for each (CPolygonPtr currentPolygon in currentPolygons)
			{
				if (currentPolygon->Bound.OverlapY((*endPoint).Box->Bound))
				{
					pairsToCheck.push_back(SPolygonPair(currentPolygon, (*endPoint).Box));
				}
			}
			currentPolygons.emplace((*endPoint).Box);
		}
		else
		{
			currentPolygons.erase((*endPoint).Box);
		}
	}
}

void  CSweepPrune::AddToBroadPhase(CPolygonPtr polygone)
{
	AddPolygone(polygone);
}


/*
int CSweepPrune::gSortAxis = 0;

CSweepPrune::CSweepPrune()
{
}

CSweepPrune::~CSweepPrune()
{
	polygons.clear();
}

void CSweepPrune::AddPolygone(CPolygonPtr polygone)
{
	polygons.emplace_back(polygone);
}


void CSweepPrune::UpdateAxis(int value)
{
	polygons[0]->Bound.Axis = value;
}

void CSweepPrune::GetCollidingPairsToCheck(std::vector<SPolygonPair>& pairsToCheck)
{
	polygons.clear();
	for (size_t index = 0; index < gVars->pWorld->GetPolygonCount(); ++index)
	{
		AddPolygone(gVars->pWorld->GetPolygon(index));
	}
	Sort();

	Vec2 s = Vec2(), s2 = Vec2(), v = Vec2();
	for (int i = 0; i < polygons.size(); i++)
	{
		Vec2 center = polygons[i]->Bound.GetCenter();
		for (int c = 0; c < 2; c++)
		{
			s[c] += center[c];
			s2[c] += center[c] * center[c];
		}

		for (int j = i + 1; j < polygons.size(); j++)
		{
			if (polygons[j]->Bound.Min()[gSortAxis] > polygons[i]->Bound.Max()[gSortAxis])
				break;
			if (polygons[i]->Bound.Overlap(polygons[j]->Bound))
			{
				pairsToCheck.push_back(SPolygonPair(polygons[i], polygons[j]));
			}
		}
	}
	for (int c = 0; c < 2; c++)
	{
		v[c] = s2[c] - s[c] * s[c] / polygons.size();
	}

	gSortAxis = 0;
	if (v[1] > v[0])
	{
		gSortAxis = 1;
		polygons[0]->Bound.Axis = 1;
	}
}

void CSweepPrune::Sort()
{
	if (polygons.size() > 1)
	{
		for (size_t i = 0; i < polygons.size(); ++i)
		{
			for (size_t j = i; j > 0; --j)
			{
				if (polygons[j]->Bound.Min().x < polygons[j - 1]->Bound.Min().x)
				{
					std::iter_swap(polygons.begin() + j, polygons.begin() + j - 1);
				}
				else
				{
					break;
				}
			}
		}
	}
}
*/
