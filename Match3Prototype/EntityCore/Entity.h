#pragma once

#include <cstdint>

#include "RenderTypes.h"
#include "EntityHandle.h"

class EntityManager;

class Entity
{
public:
	Entity(EntityHandle EntityHandleInstance, EntityManager* const EntityManagerRef, ERenderType RenderType, float X, float Y, uint8_t LayerZ);

	void SetEntityRenderType(ERenderType RenderType);
	void SetEntityPosition(float X, float Y);
	void SendEntityData(RenderData Data);
	void SetEntityLayerZ(uint8_t LayerZ);

	float ReadEntityX();
	float ReadEntityY();

private:

	void PropagateEntityPosition(float X, float Y);
	void PropagateEntityLayerZ(uint8_t LayerZ);

	const EntityHandle MyEntityHandle;
	EntityManager* const EntityManagerRef;
	ERenderType MyRenderType;

	float MyX, MyY;
	uint8_t MyLayerZ;
};