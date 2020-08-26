#ifndef _DISPLAY_COLLISION_H_
#define _DISPLAY_COLLISION_H_

#include "Behavior.h"
#include "PhysicEngine.h"
#include "GlobalVariables.h"
#include "Renderer.h"
#include "RenderWindow.h"
#include "World.h"

#include <string>
#include <iostream>

class CDisplayCollision : public CBehavior
{
public:
	CPolygonPtr polyA;
	CPolygonPtr polyB;

private:
	virtual void Update(float frameTime) override
	{
		gVars->pPhysicEngine->Activate(true);

		Vec2 point, normal;
		float dist;
		if (polyA->CheckCollision(*polyB, point, normal, dist))
		{
			gVars->pRenderer->DisplayTextWorld("collision point", point);
			gVars->pRenderer->DisplayText("Collision distance : " + std::to_string(dist), 50, 50);

			gVars->pRenderer->DrawLine(point, point + normal * dist, 0.0f, 1.0f, 0.0f);

			if (gVars->bDebug)
			{
				polyA->UpdateBound();
				polyB->UpdateBound();
				polyA->DrawBoundBox(0.0f, 0.0f, 0.62f);
				polyB->DrawBoundBox(0.0f, 0.0f, 0.62f);
			}
		}
	}
};


#endif
