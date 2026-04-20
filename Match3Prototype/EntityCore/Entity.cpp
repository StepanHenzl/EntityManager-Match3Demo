#include "Entity.h"

#include "EntityManager.h"

Entity::Entity(EntityHandle EntityHandleInstance, EntityManager* const EntityManagerRefIn, ERenderType RenderType, float X, float Y, uint8_t LayerZ) :
	MyEntityHandle(EntityHandleInstance),
	EntityManagerRef(EntityManagerRefIn),
	MyRenderType(RenderType),
	MyX(X),
	MyY(Y),
	MyLayerZ(LayerZ)
{
}

void Entity::SetEntityRenderType(ERenderType RenderType)
{
	if (MyRenderType == RenderType)
		return;

	MyRenderType = RenderType;

	EntityManagerRef->SetEntityRenderDataByID(MyEntityHandle, RenderType);

}

void Entity::SetEntityPosition(float X, float Y)
{
	if (MyX == X && MyY == Y)
		return;

	MyX = X;
	MyY = Y;

	PropagateEntityPosition(X, Y);
}

void Entity::SendEntityData(RenderData Data)
{
	EntityManagerRef->ChangeEntityRenderDataByID(MyEntityHandle, Data);
}

void Entity::SetEntityLayerZ(uint8_t LayerZ)
{
	if (LayerZ != MyLayerZ)
	{
		MyLayerZ = LayerZ;
		PropagateEntityLayerZ(LayerZ);
	}
}

float Entity::ReadEntityX()
{
	return MyX;
}

float Entity::ReadEntityY()
{
	return MyY;
}

void Entity::PropagateEntityPosition(float X, float Y)
{
	EntityManagerRef->ChangeEntityRenderPositionByID(MyEntityHandle, MyRenderType, X, Y);
}

void Entity::PropagateEntityLayerZ(uint8_t LayerZ)
{
	EntityManagerRef->ChangeEntityRenderLayerZ(MyEntityHandle, LayerZ);
}
