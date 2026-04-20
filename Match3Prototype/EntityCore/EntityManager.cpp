#include "EntityManager.h"

#include "Entity.h"

EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
	//Signal to all entity handles, that entity manager is not alive anymore.
	//Entity handle destruction might trigger callbacks otherwise.
	EntityManager::IsAlive = false;
}

std::shared_ptr<EntityHandle> EntityManager::AddEntity()
{
	return AddEntityWithParameters(DefaultLayerZ, DefaultRenderType);
}

std::shared_ptr<EntityHandle> EntityManager::AddEntityWithParameters(uint8_t LayerZ, ERenderType RenderType, float EntityX, float EntityY)
{
	auto EntityHandleInstance = GenerateNewEntityHandle();

	PushBackEntityRenderData(EntityHandleInstance, LayerZ, RenderType, EntityX, EntityY);

	EntityContainer.try_emplace(EntityHandleInstance.ID, std::make_unique<Entity>(EntityHandleInstance, this, RenderType, EntityX, EntityY, LayerZ));

	return std::shared_ptr<EntityHandle>(
		new EntityHandle(EntityHandleInstance),
		[this](EntityHandle* EntityHandleRef)
		{
			//Entity removal from manager is coupled with entity lifespan.
			//Ownership of handle belongs to classes that request for it, therefore entity handle might outlive entity manager.
			if(EntityManager::IsAlive)
				this->RemoveEntityByID(*EntityHandleRef); delete EntityHandleRef; 
		});
}

EntityHandle EntityManager::GenerateNewEntityHandle()
{
	if (ReusableEntityIDs.size() == 0)
	{
		uint32_t EntityID = NextUnusedEntityID;
		NextUnusedEntityID++;

		//Version is always 0 when entityID is created first time.
		//Serve as invalidator of old IDs.
		EntityVersions.push_back(0);
		return {EntityID, 0};
	}
	else
	{
		uint32_t EntityID = ReusableEntityIDs.back();
		ReusableEntityIDs.pop_back();

		return {EntityID, EntityVersions[EntityID]};
	}
}

bool EntityManager::RemoveEntityByID(EntityHandle EntityHandleInstance)
{

	if (! ValidateEntityHandler(EntityHandleInstance))
		return false;

	uint32_t EntityID = EntityHandleInstance.ID;

	if (EntityContainer.erase(EntityID) == 0)
	{
		return false;
	}

	if (! RemoveEntityRenderDataByID(EntityHandleInstance))
		return false;

	ReusableEntityIDs.push_back(EntityHandleInstance.ID);
	EntityVersions[EntityID] += 1;

	return true;
}

void EntityManager::PushBackEntityRenderData(EntityHandle EntityHandleInstance, uint8_t RenderLayer, ERenderType RenderType, float EntityX, float EntityY)
{
	if (! ValidateEntityHandler(EntityHandleInstance))
		return;

	size_t IndexOfEntityInRenderData = EntityBatchDataInstance.DataPerLayerZ[RenderLayer].PushBackEmptyByType(RenderType, EntityX, EntityY, EntityHandleInstance.ID) - 1;

	//Check if entity already have its location data.
	if (EntitiesLocationsContainer.size() <= EntityHandleInstance.ID)
	{
		EntitiesLocationsContainer.resize(EntityHandleInstance.ID + 1);
	}

	EntitiesLocationsContainer[EntityHandleInstance.ID] = EntityLocation{RenderLayer, RenderType, static_cast<uint32_t>(IndexOfEntityInRenderData)};
}

const EntityBatchData& EntityManager::ReadEntityBatchData() const
{
	return EntityBatchDataInstance;
}

const std::vector<EntityLocation>& EntityManager::ReadEntityLocations() const
{
	return EntitiesLocationsContainer;
}

std::optional<Entity*> EntityManager::GetEntityRefByID(EntityHandle EntityHandleInstance)
{
	if (!ValidateEntityHandler(EntityHandleInstance))
		return std::nullopt;

	auto Result = EntityContainer.find(EntityHandleInstance.ID);
	if (Result == EntityContainer.end())
		return std::nullopt;

	return Result->second.get();
}

bool EntityManager::RemoveEntityRenderDataByID(EntityHandle EntityHandleInstance)
{
	if (! ValidateEntityHandler(EntityHandleInstance))
		return false;

	auto& EntityLocation = EntitiesLocationsContainer[EntityHandleInstance.ID];
	auto SwappedEntityID = EntityBatchDataInstance.DataPerLayerZ[EntityLocation.LayerZ].SwapPop(EntityLocation.RenderType, EntityLocation.RenderOrder);

	if (!SwappedEntityID.has_value())
		return false;

	//Swap pop change entíty location of swapped entity and therefore have to be updated.
	EntitiesLocationsContainer[SwappedEntityID.value()] = EntityLocation;

	return true;
}

bool EntityManager::ChangeEntityRenderDataByID(EntityHandle EntityHandleInstance, RenderData Data)
{
	if (! ValidateEntityHandler(EntityHandleInstance))
		return false;

	auto& EntityLocation = EntitiesLocationsContainer[EntityHandleInstance.ID];

	if (Data.index() != static_cast<size_t>(EntityLocation.RenderType))
		return false;
	
	EntityBatchDataInstance.DataPerLayerZ[EntityLocation.LayerZ].SetDataAtIndex(EntityLocation.RenderOrder, Data);

	return true;
}

bool EntityManager::SetEntityRenderDataByID(EntityHandle EntityHandleInstance, ERenderType RenderType)
{
	if (! ValidateEntityHandler(EntityHandleInstance))
		return false;

	if (! RemoveEntityRenderDataByID(EntityHandleInstance))
		return false;

	PushBackEntityRenderData(EntityHandleInstance, EntitiesLocationsContainer[EntityHandleInstance.ID].LayerZ, RenderType);

	return true;
}

bool EntityManager::ChangeEntityRenderPositionByID(EntityHandle EntityHandleInstance, ERenderType RenderType, float X, float Y)
{
	if (! ValidateEntityHandler(EntityHandleInstance))
		return false;

	auto& EntityLocation = EntitiesLocationsContainer[EntityHandleInstance.ID];

	EntityBatchDataInstance.DataPerLayerZ[EntityLocation.LayerZ].SetRenderLocation(RenderType, EntityLocation.RenderOrder, X, Y);

	return true;
}

bool EntityManager::ChangeEntityRenderLayerZ(EntityHandle EntityHandleInstance, uint8_t LayerZ)
{
	if (!ValidateEntityHandler(EntityHandleInstance))
		return false;

	//Remove previous render data of entity
	if (! RemoveEntityRenderDataByID(EntityHandleInstance))
		return false;

	//Pushback new render data to new layerz. Render data have to be set again after change of render layer. 
	//Render type persist for optimisation purposes, but it should not be relied on.
	size_t IndexOfEntityInRenderData = EntityBatchDataInstance.DataPerLayerZ[LayerZ].PushBackEmptyByType
	(
		EntitiesLocationsContainer[EntityHandleInstance.ID].RenderType, 
		DefaultEntityX, 
		DefaultEntityY, 
		EntityHandleInstance.ID
	) - 1;

	EntitiesLocationsContainer[EntityHandleInstance.ID].LayerZ = LayerZ;
	EntitiesLocationsContainer[EntityHandleInstance.ID].RenderOrder = static_cast<uint32_t>(IndexOfEntityInRenderData);

	return true;
}



