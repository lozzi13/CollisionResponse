#ifndef _SPHERE_SIMULATION_H_
#define _SPHERE_SIMULATION_H_

#include "Behavior.h"
#include "PhysicEngine.h"
#include "GlobalVariables.h"
#include "Renderer.h"
#include "World.h"

#define RADIUS 2.0f
#define DISTANCE 5.0f

struct SConstraint
{
	SConstraint(CPolygonPtr _A, CPolygonPtr _B)
		: A(_A), B(_B) {}

	CPolygonPtr A, B;
	float distance = DISTANCE;
};

class CSphereSimulation : public CBehavior
{
private:

	void InitChain(size_t count, const Vec2& start)
	{
		for (size_t i = 0; i < count; ++i)
		{
			m_chain.push_back(AddCircle(start + Vec2(0.0f, -(float)i * DISTANCE)));
		}
	}

	void SolveChainConstraints()
	{
		//m_chain[0]->speed = Vec2();
	}

	virtual void Start() override
	{
		for (float x = -12.0f; x < 12.0f; x += 5.0f)
		{
			for (float y = -22.0f; y < 22.0f; y += 5.0f)
			{
				AddCircle(Vec2(x + Random(-0.1f, 0.1f), y + Random(-0.1f, 0.1f)))->speed.x = 50.0f;
			}
		}

		InitChain(8, Vec2(20.0f, 20.0f));

		gVars->pPhysicEngine->Activate(false);
	}

	virtual void Update(float frameTime) override
	{
		for (CPolygonPtr& circle : m_circles)
		{
			circle->speed.y -= 20.0f * frameTime;
			circle->speed -= circle->speed * 0.3f * frameTime;
		}

		for (size_t i = 0; i < m_circles.size(); ++i)
		{
			for (size_t j = i + 1; j < m_circles.size(); ++j)
			{
				CPolygonPtr c1 = m_circles[i];
				CPolygonPtr c2 = m_circles[j];

				Vec2 diffPos = c2->transform.GetPosition() - c1->transform.GetPosition();
				Vec2 diffSpeed = c2->speed - c1->speed;
				if (diffPos.GetSqrLength() < 4.0f * RADIUS * RADIUS && ((diffSpeed | diffPos) < 0.0f))
				{
					float damping = 1.0f;
					float MassInverseA = c1->GetMassInvers();
					float MassInverseB = c2->GetMassInvers();

					if (MassInverseA + MassInverseB <= FLT_EPSILON)
						return;

					float vRel = diffSpeed | diffPos.Normalized();
					if (vRel < 0.0f)
					{

						float J = (-(1 + c1->Bouciness) * vRel) / (MassInverseA + MassInverseB);
						c1->speed -= diffPos.Normalized() * J * MassInverseA;
						c2->speed += diffPos.Normalized() * J * MassInverseB;

						ApplyFriction(c1, c2, diffPos.Normalized(), J, MassInverseA, MassInverseB);
					}
				}
			}
		}

		float hWidth = gVars->pRenderer->GetWorldWidth() * 0.5f;
		float hHeight = gVars->pRenderer->GetWorldHeight() * 0.5f;

		for (CPolygonPtr& circle : m_circles)
		{
			if (circle->transform.GetPosition().x < -hWidth + RADIUS && circle->speed.x < 0)
			{
				circle->speed.x *= -1.0f;
			}
			else if (circle->transform.GetPosition().x > hWidth - RADIUS && circle->speed.x > 0)
			{
				circle->speed.x *= -1.0f;
			}
			if (circle->transform.GetPosition().y < -hHeight + RADIUS && circle->speed.y < 0)
			{
				circle->speed.y *= -1.0f;
			}
			else if (circle->transform.GetPosition().y > hHeight - RADIUS && circle->speed.y > 0)
			{
				circle->speed.y *= -1.0f;
			}
		}

		SolveChainConstraints();

		for (CPolygonPtr& circle : m_circles)
		{
			circle->transform.updatePosition(circle->speed * frameTime);
		}
	}

	CPolygonPtr AddCircle(const Vec2& pos, float radius = RADIUS)
	{
		CPolygonPtr circle = gVars->pWorld->AddSymetricPolygon(radius, 50);
		circle->density = 0.0f;
		circle->transform.setPosition(pos);
		circle->Build();
		gVars->pPhysicEngine->AddToBroadPhase(circle);
		m_circles.push_back(circle);
		return circle;
	}

	void ApplyFriction(CPolygonPtr A, CPolygonPtr B, Vec2 normal, float colImpulse, float MassInverseA, float MassInverseB)
	{
		float friction = .01f;
		Vec2 RelativeVelocity = B->speed - A->speed;

		Vec2 tangent = (RelativeVelocity - normal * (RelativeVelocity | normal));
		tangent.Normalize();

		float vTangent = RelativeVelocity | tangent;
		float J = -vTangent / (MassInverseA + MassInverseB);
		J = Clamp(J, -std::fabs(colImpulse) * friction, std::fabs(colImpulse) * friction);
		A->speed -= tangent * J * MassInverseA;
		B->speed += tangent * J * MassInverseB;
	}

	std::vector<CPolygonPtr>	m_circles;
	std::vector<CPolygonPtr>	m_chain;
};

#endif
