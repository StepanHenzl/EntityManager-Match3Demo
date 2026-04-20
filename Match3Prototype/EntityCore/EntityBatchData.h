#pragma once

#include <cstdint>
#include <vector>
#include <optional>
#include <array>

#include "ErrorHandler.h"

#include "RenderTypes.h"

struct EntityDataPerLayerZ
{
	//Parallel storage for cache efficiency.
	//Index in data vectors must correspond to index in entity location corresponding to ID.
	//Handled manually for efficiency.
	std::vector<Rectangle> RectanglesData;
	std::vector<uint32_t> RectanglesEntityIDs;
	std::vector<Image> ImagesData;
	std::vector<uint32_t> ImagesEntityIDs;

	std::vector<Rectangle>& GetRectangleData() { return RectanglesData; };
	std::vector<uint32_t>& GetRectangleIDs() { return RectanglesEntityIDs; };
	std::vector<Image>& GetImageData() { return ImagesData; };
	std::vector<uint32_t>& GetImageIDs() { return ImagesEntityIDs; };

	size_t PushBackEmptyByType(ERenderType RenderType, float X, float Y, uint32_t EntityID)
	{
		switch (RenderType)
		{
		case ERenderType::Rectangle:
			GetRectangleData().emplace_back(X, Y);
			GetRectangleIDs().push_back(EntityID);
			return GetRectangleData().size();
		case ERenderType::Image:
			GetImageData().emplace_back(X, Y);
			GetImageIDs().push_back(EntityID);
			return GetImageData().size();
		}

		Error_Abort("Pushing to non-existing render type!\n");
	}

	void SetDataAtIndex(size_t DataIndex, RenderData Data)
	{
		switch (Data.index())
		{
		case 0:
			GetRectangleData()[DataIndex] = std::get<Rectangle>(Data);
			break;

		case 1:
			GetImageData()[DataIndex] = std::get<Image>(Data);
			break;
		}
	}

	void SetRenderLocation(ERenderType RenderType, size_t DataIndex, float X, float Y)
	{
		switch (static_cast<int>(RenderType))
		{
		case 0:
			GetRectangleData()[DataIndex].SetRenderLocation(X, Y);
			break;

		case 1:
			GetImageData()[DataIndex].SetRenderLocation(X, Y);
			break;
		}
	}


private:
	template<typename T>
	std::optional<uint32_t> SwapPopVector(std::vector<T>& TargetVector, std::vector<uint32_t>& EntitiesIDs, size_t RemovedIndex)
	{
		if (TargetVector.size() <= RemovedIndex)
			return std::nullopt;

		//Swap with last element for O(1) removal.
		TargetVector[RemovedIndex] = TargetVector[TargetVector.size() - 1];
		TargetVector.pop_back();


		EntitiesIDs[RemovedIndex] = EntitiesIDs[EntitiesIDs.size() - 1];
		EntitiesIDs.pop_back();

		//Return ID that was moved to removed slot.
		return std::optional<uint32_t>(EntitiesIDs[RemovedIndex]);
	}

public:
	std::optional<uint32_t> SwapPop(ERenderType RenderType, size_t RemovedIndex)
	{
		switch (RenderType)
		{
		case ERenderType::Rectangle:
			return SwapPopVector(RectanglesData, RectanglesEntityIDs, RemovedIndex);

		case ERenderType::Image:
			return SwapPopVector(ImagesData, ImagesEntityIDs, RemovedIndex);
		}

		Error_Abort("SwapPoping from non-existing render type!\n");
	}
};

struct EntityLocation
{
	uint8_t LayerZ;
	ERenderType RenderType;
	uint32_t RenderOrder;
};

struct EntityBatchData
{
	std::array<EntityDataPerLayerZ, 256> DataPerLayerZ;
};
