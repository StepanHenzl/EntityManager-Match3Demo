#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>

#include "ErrorHandler.h"

#include "EntityHandle.h"
#include "EntityBatchData.h"
#include "RenderTypes.h"

class Entity;

class EntityManager
{
public:
	friend class Entity;
	friend class BlackBoxEntityManager;

	EntityManager();
	~EntityManager();

	std::shared_ptr<EntityHandle> AddEntity();

	std::shared_ptr<EntityHandle> AddEntityWithParameters(uint8_t LayerZ, ERenderType RenderType, float EntityX = DefaultEntityX, float EntityY = DefaultEntityY);

	const EntityBatchData& ReadEntityBatchData() const;

	const std::vector<EntityLocation>& ReadEntityLocations() const;

	std::optional<Entity*> GetEntityRefByID(EntityHandle EntityHandleInstance);

private:

	void PushBackEntityRenderData(EntityHandle EntityHandleInstance, uint8_t RenderLayer, ERenderType RenderType, float EntityX = DefaultEntityX, float EntityY = DefaultEntityY);

	bool RemoveEntityRenderDataByID(EntityHandle EntityHandleInstance);

	bool ChangeEntityRenderDataByID(EntityHandle EntityHandleInstance, RenderData Data);

	bool SetEntityRenderDataByID(EntityHandle EntityHandleInstance, ERenderType RenderType);

	bool ChangeEntityRenderPositionByID(EntityHandle EntityHandleInstance, ERenderType RenderType, float X, float Y);

	bool ChangeEntityRenderLayerZ(EntityHandle EntityHandleInstance, uint8_t LayerZ);

	EntityHandle GenerateNewEntityHandle();

	bool RemoveEntityByID(EntityHandle EntityHandleInstance);

	inline bool ValidateEntityHandler(EntityHandle EntityHandleInstance) const noexcept
	{
		return EntityHandleInstance.Version == EntityVersions[EntityHandleInstance.ID];
	}

	std::unordered_map<uint32_t, std::unique_ptr<Entity>> EntityContainer;
	std::vector<EntityLocation> EntitiesLocationsContainer;

	uint32_t NextUnusedEntityID = 0;

	std::vector<uint32_t> ReusableEntityIDs;
	std::vector<uint32_t> EntityVersions;

	EntityBatchData EntityBatchDataInstance;


	static constexpr uint8_t DefaultLayerZ = 0;
	static constexpr ERenderType DefaultRenderType = ERenderType::Rectangle;
	static constexpr float DefaultEntityX = 0;
	static constexpr float DefaultEntityY = 0;

	inline static bool IsAlive;
	
};
