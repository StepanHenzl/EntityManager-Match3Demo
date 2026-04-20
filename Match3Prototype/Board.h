#pragma once

#include <optional>
#include <memory>
#include <cstdint>
#include <unordered_map>
#include <queue>

#include "EntityHandle.h"

#include "RGBAToColor.h"
#include "CoreMath.h"

class EntityManager;

class Entity;

struct TileCoordinate
{
	int32_t X;
	int32_t Y;

	bool operator==(const TileCoordinate& Other) const
	{
		return X == Other.X && Y == Other.Y;
	}

	TileCoordinate operator+(const TileCoordinate& Other) const
	{
		return { X + Other.X, Y + Other.Y };
	}
};

template <>
struct std::hash<TileCoordinate>
{
	size_t operator()(const TileCoordinate TileCoordinateInstance) const
	{
		return static_cast<size_t>(TileCoordinateInstance.X) << 32 | static_cast<size_t>(TileCoordinateInstance.Y);
	}
};

class Tile
{
public:
	Tile(float X, float Y, float TileWidthIn, float TileHeightIn, uint32_t TileColorIn, std::shared_ptr<EntityHandle> EntityHandleRefIn, Entity* EntityRefIn, EntityManager* EntityManagerRefIn, VectorF GemOffset) :
		TileX(X),
		TileY(Y),
		TileHeight(TileHeightIn),
		TileWidth(TileWidthIn),
		TileColor(TileColorIn),
		MyEntityHandleRef(std::move(EntityHandleRefIn)),
		MyEntityRef(EntityRefIn),
		EntityManagerRef(EntityManagerRefIn),
		GemTopOffset(GemOffset.X),
		GemLeftOffset(GemOffset.Y)
	{
		DrawTile();
		SetUpGemSprite();
		RedrawGemType();
	};

	static void SwapTileObjects(Tile* TileRef, Tile* OtherTileRef);

	static void SetGemTypeFromSpawn(Tile* TileRef, uint8_t GemType);

	void RedrawGemType();
	void RedrawGem();

	bool IsEmpty();

	const uint8_t EmptyGemType = 0;

private:

	void DrawTile();
	void SetUpGemSprite();

	float TileX;
	float TileY;

	float TileWidth;
	float TileHeight;

	uint32_t TileColor;

	uint8_t GemType = EmptyGemType;
	uint32_t GemColors[3] = { 0, RGBAToColor(180,60,60,255), RGBAToColor(60,180,60,255) };

	std::shared_ptr<EntityHandle> MyEntityHandleRef;
	Entity* MyEntityRef;

	//TODO
public:
	std::shared_ptr<EntityHandle> GemEntityHandleRef;
	Entity* GemEntityRef;
private:

	EntityManager* EntityManagerRef;

	const float GemTopOffset;
	const float GemLeftOffset;

};

class GemMovement
{
public:
	GemMovement() = default;

	void AddMovement(EntityHandle* EntityHandleInstance, Entity* EntityRef, VectorF InitialPositionIn, VectorF DirectionIn);
	void TriggerMovementsTick(double Delta);

private:
	struct LerpMovementData
	{
		VectorF InitialPosition;
		VectorF FinalPosition;
		Entity* EntityRef;
		double MaxDuration = 0.6;
		double DurationPassed = 0;

		bool operator<(const LerpMovementData& Other) const
		{
			return InitialPosition.Y < Other.InitialPosition.Y;
		}
	};

	std::unordered_map<EntityHandle, std::vector<LerpMovementData>> GemsMovementList;
};

class Board
{
public:
	Board(EntityManager* EntityManagerRefIn);
	void Tick(double Delta);

	void InitializeBoard();

	VectorF GetGemPositionAtTile(TileCoordinate TileCoordinateInstance) const;

private:
	struct AboveTileResult
	{
		enum class AccessType
		{
			ExistingTile,
			SpawnFromColumn
		};

		AccessType Access;
		TileCoordinate Tile;
		int32_t SpawnX;
	};

	void CreateTile(TileCoordinate TileCoordinateInstance, uint32_t TileColor);

	void RequestGemDrop(TileCoordinate TileCoordinateInstance);
	AboveTileResult GetNextTileLocationAbove(TileCoordinate TileCoordinateInstance);
	bool HasTileAt(TileCoordinate TileCoordinateInstance);

	EntityManager* EntityManagerRef;

	const float OffsetX = 200;
	const float OffsetY = 100;

	const float TileWidth = 80;
	const float TileHeight = 80;

	std::unordered_map<TileCoordinate, std::unique_ptr<Tile>> TilesContainer;

	std::unique_ptr<GemMovement> GemMovementRef;
	std::unordered_map<TileCoordinate, uint16_t> StateSpawnedGemsAtCounter;

	const int MinAmountOfGemMovementToDisallowSwapping = 2;

	const VectorF GemOffset = { 40, 40 };
};