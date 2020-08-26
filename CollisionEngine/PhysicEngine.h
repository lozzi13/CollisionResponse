#ifndef _PHYSIC_ENGINE_H_
#define _PHYSIC_ENGINE_H_

#include <vector>
#include <unordered_map>
#include "Maths.h"
#include "Polygon.h"
#include <functional>

class IBroadPhase;

struct SPolygonPair
{
	SPolygonPair(CPolygonPtr _polyA, CPolygonPtr _polyB) : polyA(_polyA), polyB(_polyB) {}

	CPolygonPtr	polyA;
	CPolygonPtr	polyB;
	bool operator==(const SPolygonPair &other) const
	{
		return (polyA == other.polyA && polyB == other.polyB) || (polyA == other.polyB && polyB == other.polyA);
	}
};

class SPolygonPair_Hash {
public:

	size_t operator()(const SPolygonPair& p) const
	{
		return (std::hash<CPolygonPtr>()(p.polyA)) ^
			(std::hash<CPolygonPtr>()(p.polyB));
	}
};

struct SCollision
{
	SCollision() = default;
	SCollision(CPolygonPtr _polyA, CPolygonPtr _polyB, Vec2	_point, Vec2 _normal, float _distance)
		: polyA(_polyA), polyB(_polyB), point(_point), normal(_normal), distance(_distance) {}

	CPolygonPtr	polyA, polyB;

	Vec2	point;
	Vec2	normal;
	float	distance;
};

class CPhysicEngine
{
public:
	void	Reset();
	void	Activate(bool active);

	void	DetectCollisions();

	void	Step(float deltaTime);

	template<typename TFunctor>
	void	ForEachCollision(TFunctor functor)
	{
		for (const SCollision& collision : m_collidingPairs)
		{
			functor(collision);
		}
	}
	void AddToBroadPhase(CPolygonPtr polygone);


private:
	friend class CPenetrationVelocitySolver;

	void						CollisionBroadPhase();
	void						CollisionNarrowPhase();

	bool						m_active = true;

	// Collision detection
	IBroadPhase*				m_broadPhase = nullptr;
	std::vector<SPolygonPair>	m_pairsToCheck;
	std::vector<SCollision>		m_collidingPairs;
};

#endif
