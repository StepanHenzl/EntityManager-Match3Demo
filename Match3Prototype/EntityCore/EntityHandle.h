#pragma once

#include <cstdint>

struct EntityHandle
{
	uint32_t ID;
	uint32_t Version;

	bool operator==(const EntityHandle& Other) const
	{
		return ID == Other.ID && Version == Other.Version;
	}
};

template <>
struct std::hash<EntityHandle>
{
	size_t operator()(const EntityHandle EntityHandleInstance) const
	{
		return static_cast<size_t>(EntityHandleInstance.ID) << 32 | static_cast<size_t>(EntityHandleInstance.Version);
	}
};