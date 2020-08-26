#include "PhysicEngine.h"

#include <iostream>
#include <string>
#include "GlobalVariables.h"
#include "World.h"
#include "Renderer.h" // for debugging only
#include "Timer.h"

#include "BroadPhase.h"
#include "BroadPhaseBrut.h"
#include "SweepPrune.h"
#include "OptimizedSweepPrune.h"


void	CPhysicEngine::Reset()
{
	m_pairsToCheck.clear();
	m_collidingPairs.clear();

	m_active = true;

	if (m_broadPhase != nullptr)
		delete m_broadPhase;
	//m_broadPhase = new CBroadPhaseBrut();
	//m_broadPhase = new CSweepPrune();
	m_broadPhase = new COptimizedSweepPrune();
}

void	CPhysicEngine::Activate(bool active)
{
	m_active = active;
}

void	CPhysicEngine::DetectCollisions()
{
	CTimer timer;
	timer.Start();
	CollisionBroadPhase();
	timer.Stop();
	if (gVars->bDebug)
	{
		gVars->pRenderer->DisplayText("Collision broadphase duration " + std::to_string(timer.GetDuration() * 1000.0f) + " ms");
		for (size_t index = 0; index < m_pairsToCheck.size(); ++index)
		{
			m_pairsToCheck[index].polyA->DrawBoundBox(0.71f, 0.0f, 0.0f);
			m_pairsToCheck[index].polyB->DrawBoundBox(0.71f, 0.0f, 0.0f);
		}
	}

	timer.Start();
	CollisionNarrowPhase();
	timer.Stop();
	if (gVars->bDebug)
	{
		gVars->pRenderer->DisplayText("Collision narrowphase duration " + std::to_string(timer.GetDuration() * 1000.0f) + " ms, collisions : " + std::to_string(m_collidingPairs.size()));
		for (size_t index = 0; index < m_collidingPairs.size(); ++index)
		{
			m_collidingPairs[index].polyA->DrawBoundBox(0.0f, 0.0f, 0.62f);
			m_collidingPairs[index].polyB->DrawBoundBox(0.0f, 0.0f, 0.62f);
		}
	}
}

void	CPhysicEngine::Step(float deltaTime)
{
	if (!m_active)
	{
		return;
	}

	DetectCollisions();
}

void	CPhysicEngine::CollisionBroadPhase()
{
	m_pairsToCheck.clear();
	m_broadPhase->GetCollidingPairsToCheck(m_pairsToCheck);
}

void	CPhysicEngine::CollisionNarrowPhase()
{
	m_collidingPairs.clear();
	for (const SPolygonPair& pair : m_pairsToCheck)
	{
		SCollision collision;
		collision.polyA = pair.polyA;
		collision.polyB = pair.polyB;
		if (pair.polyA->CheckCollision(*(pair.polyB), collision.point, collision.normal, collision.distance))
		{
			m_collidingPairs.push_back(collision);
		}
	}
}

void CPhysicEngine::AddToBroadPhase(CPolygonPtr polygone)
{
	m_broadPhase->AddToBroadPhase(polygone);
}
