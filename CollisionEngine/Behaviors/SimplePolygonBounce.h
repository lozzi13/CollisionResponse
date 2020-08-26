#ifndef _SIMPLE_COLLISION_BOUNCE_H_
#define _SIMPLE_COLLISION_BOUNCE_H_

#include "Behavior.h"
#include "PhysicEngine.h"
#include "GlobalVariables.h"
#include "Renderer.h"
#include "World.h"
#include <cmath>

class CSimplePolygonBounce : public CBehavior
{
private:
	virtual void Update(float frameTime) override
	{
		gVars->pPhysicEngine->ForEachCollision([&](const SCollision& collision)
		{
			float damping = 0.2f;
			float MassInverseA = collision.polyA->GetMassInvers();
			float MassInverseB = collision.polyB->GetMassInvers();

			if (MassInverseA + MassInverseB <= FLT_EPSILON)
				return;
		
			float correction = (collision.distance * damping) / (MassInverseA + MassInverseB);
			collision.polyA->transform.updatePosition((collision.normal * MassInverseA * correction) * -1);
			collision.polyB->transform.updatePosition(collision.normal * MassInverseB * correction);

			float vRel = (collision.polyB->speed - collision.polyA->speed) | collision.normal;
			if (vRel < 0.0f )
			{

				float J = (-(1 + collision.polyA->Bouciness) * vRel) / (MassInverseA + MassInverseB);
				collision.polyA->speed -= collision.normal * J * MassInverseA;
				collision.polyB->speed += collision.normal * J * MassInverseB;

				ApplyFriction(collision.polyA, collision.polyB, collision.normal, J, MassInverseA, MassInverseB);
			}
		});

		float hWidth = gVars->pRenderer->GetWorldWidth() * 0.5f;
		float hHeight = gVars->pRenderer->GetWorldHeight() * 0.5f;

		gVars->pWorld->ForEachPolygon([&](CPolygonPtr poly)
		{
			poly->transform.updatePosition(poly->speed * frameTime);

			if (poly->transform.GetPosition().x < -hWidth)
			{
				poly->transform.setPosition(Vec2(-hWidth, poly->transform.GetPosition().y));
				poly->speed.x *= -1.0f;
			}
			else if (poly->transform.GetPosition().x > hWidth)
			{
				poly->transform.setPosition(Vec2(hWidth, poly->transform.GetPosition().y));
				poly->speed.x *= -1.0f;
			}
			if (poly->transform.GetPosition().y < -hHeight)
			{
				poly->transform.setPosition(Vec2(poly->transform.GetPosition().x, -hHeight));

				poly->speed.y *= -1.0f;
			}
			else if (poly->transform.GetPosition().y > hHeight)
			{
				poly->transform.setPosition(Vec2(poly->transform.GetPosition().x, hHeight));
				poly->speed.y *= -1.0f;
			}
		});
	}

	void ApplyFriction(CPolygonPtr A, CPolygonPtr B, Vec2 normal, float colImpulse, float MassInverseA,	float MassInverseB)
	{
		float friction =.0f;
		Vec2 RelativeVelocity = B->speed - A->speed;

		Vec2 tangent = (RelativeVelocity - normal * (RelativeVelocity | normal));
		tangent.Normalize();

		float vTangent = RelativeVelocity | tangent;
		float J = -vTangent / (MassInverseA + MassInverseB);
		J = Clamp(J, -std::fabs(colImpulse) * friction, std::fabs(colImpulse) * friction);
		A->speed -= tangent * J * MassInverseA;
		B->speed += tangent * J * MassInverseB;
	}
};

#endif
