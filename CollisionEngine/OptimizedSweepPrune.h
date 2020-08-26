#pragma once
#include <vector>
#include "Polygon.h"
#include "BroadPhase.h"
#include  <unordered_set>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include "SweepPrune.h"

class COptimizedSweepPrune : public IBroadPhase
{
	std::vector<EndPoint> XEndPoints;
	std::vector<EndPoint> YEndPoints;
	std::unordered_set<SPolygonPair, SPolygonPair_Hash> m_pairs;
public:
	COptimizedSweepPrune();
	~COptimizedSweepPrune();
	void AddPolygone(CPolygonPtr polygone);
	void UpdatePolygone();
	void RemovePolygone(CPolygonPtr polygone);
	virtual void GetCollidingPairsToCheck(std::vector<SPolygonPair>& pairsToCheck) override;
	virtual void  AddToBroadPhase(CPolygonPtr polygone)override;
	void Sort(std::vector<EndPoint>& EndPoints, int axis);

};
