#pragma once
#include <vector>
#include "Polygon.h"
#include "BroadPhase.h"
#include  <unordered_set>
#include <list>
#include <unordered_map>

enum EndPointType
{
	MinValue,
	MaxValue,
};

struct EndPoint
{
public:
	CPolygonPtr Box;
	float Value;
	EndPointType Type;
	int Index;

	EndPoint(CPolygonPtr box, float Value, EndPointType Type) : Box{ box }, Value{ Value }, Type{ Type }
	{
		Index = Box->GetIndex();
	}
	bool operator <(EndPoint const& b)const
	{
		return Value < b.Value;
	}
	bool IsBeginning()
	{
		return Type == EndPointType::MinValue;
	}
	bool IsEnding()
	{
		return Type == EndPointType::MaxValue;
	}
};

struct EndPointTuple
{
	std::list<EndPoint>::iterator Min;
	std::list<EndPoint>::iterator Max;
};

class CSweepPrune : public IBroadPhase
{

	std::list<EndPoint> XEndPoints;
	std::unordered_map<CPolygonPtr, EndPointTuple> XEndPointsTuple;
public:
	CSweepPrune();
	~CSweepPrune();
	void AddPolygone(CPolygonPtr polygone);
	void UpdatePolygone();
	void RemovePolygone(CPolygonPtr polygone);
	virtual void GetCollidingPairsToCheck(std::vector<SPolygonPair>& pairsToCheck) override;
	virtual void  AddToBroadPhase(CPolygonPtr polygone)override;
};

/*std::vector<CPolygonPtr> polygons;
static int gSortAxis; // Specifies axis (0/1/2) to sort on (here arbitrarily initialized)

public:
	CSweepPrune();
	~CSweepPrune();
	void AddPolygone(CPolygonPtr polygone);
	void UpdateAxis(int value);
	virtual void GetCollidingPairsToCheck(std::vector<SPolygonPair>& pairsToCheck) override;
	virtual void  AddToBroadPhase(CPolygonPtr polygone) override
	{};
	void Sort();*/
