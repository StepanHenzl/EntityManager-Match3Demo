#include "Board.h"

#include <memory>
#include <algorithm>

#include "EntityManager.h"
#include "Entity.h"
#include "ErrorHandler.h"
#include "RenderTypes.h"

Board::Board(EntityManager* EntityManagerRefIn) : EntityManagerRef(EntityManagerRefIn)
{
	GemMovementRef = std::make_unique<GemMovement>();
}

void Board::Tick(double Delta)
{
	GemMovementRef->TriggerMovementsTick(Delta);
}

void Board::InitializeBoard()
{
	for (uint8_t Y = 0; Y < 8; Y++)
	{
		for (uint8_t X = 0; X < 8; X++)
		{
			CreateTile({ X, Y }, RGBAToColor(150, (1 - ((X + Y) % 2)) * 150, 70, 255));
		}
	}

	for (uint8_t Y = 0; Y < 8; Y++)
	{
		for (uint8_t X = 0; X < 8; X++)
		{
			RequestGemDrop({ X, Y });
		}
	}
}

void Board::CreateTile(TileCoordinate TileCoordinateInstance, uint32_t TileColor)
{
	auto EntityHandleRef = EntityManagerRef->AddEntityWithParameters(50, ERenderType::Rectangle);
	auto EntityRef = EntityManagerRef->GetEntityRefByID(*EntityHandleRef);

	if (!EntityRef.has_value())
	{
		Error_Abort("Entity of tile could not be created!!");
		return;
	}

	float TileRenderLocationX = OffsetX + TileCoordinateInstance.X * TileWidth;
	float TileRenderLocationY = OffsetY + TileCoordinateInstance.Y * TileHeight;

	if (TilesContainer.find(TileCoordinateInstance) != TilesContainer.end())
	{
		return;
	}

	TilesContainer.emplace(TileCoordinateInstance, std::make_unique<Tile>(TileRenderLocationX, TileRenderLocationY, TileWidth, TileHeight, TileColor, std::move(EntityHandleRef), *EntityRef, EntityManagerRef, GemOffset));
}

void Board::RequestGemDrop(TileCoordinate TileCoordinateInstance)
{
	TileCoordinate TargetTile = TileCoordinateInstance;
	bool WasAvaiableGemFound = false;

	while (!WasAvaiableGemFound)
	{

		AboveTileResult Result = GetNextTileLocationAbove(TargetTile);

		if (Result.Access == AboveTileResult::AccessType::SpawnFromColumn)
		{
			Tile::SetGemTypeFromSpawn(TilesContainer[TargetTile].get(), 1);
			StateSpawnedGemsAtCounter[TargetTile] += 1;
			for (int i = 0; i <= StateSpawnedGemsAtCounter[TargetTile]; i++)
			{
				GemMovementRef->AddMovement
				(
					TilesContainer[TargetTile]->GemEntityHandleRef.get(),
					TilesContainer[TargetTile]->GemEntityRef,
					GetGemPositionAtTile({ Result.SpawnX, 0 - (i + 1) }),
					GetGemPositionAtTile({ Result.SpawnX, 0 - i })
				);
			}

			return;
		}

		if (!TilesContainer[Result.Tile]->IsEmpty())
		{
			Tile::SwapTileObjects(TilesContainer[TargetTile].get(), TilesContainer[Result.Tile].get());
			GemMovementRef->AddMovement
			(
				TilesContainer[TargetTile]->GemEntityHandleRef.get(),
				TilesContainer[TargetTile]->GemEntityRef,
				GetGemPositionAtTile({ Result.Tile.X, Result.Tile.Y }),
				GetGemPositionAtTile({ TargetTile.X, TargetTile.Y })
			);
			RequestGemDrop(Result.Tile);
			WasAvaiableGemFound = true;
			return;
		}

		TargetTile = Result.Tile;
	}
}

Board::AboveTileResult Board::GetNextTileLocationAbove(TileCoordinate TileCoordinateInstance)
{
	//If tile is at top, give it new gem.
	if (TileCoordinateInstance.Y == 0)
		return { AboveTileResult::AccessType::SpawnFromColumn, {}, TileCoordinateInstance.X };

	const TileCoordinate Above { TileCoordinateInstance.X, static_cast<uint16_t>(TileCoordinateInstance.Y - 1 )};
	const TileCoordinate AboveLeft { static_cast<uint16_t>(TileCoordinateInstance.X - 1), static_cast<uint16_t>(TileCoordinateInstance.Y - 1)};
	const TileCoordinate AboveRight { static_cast<uint16_t>(TileCoordinateInstance.X + 1), static_cast<uint16_t>(TileCoordinateInstance.Y - 1)};

	//If There is tile above, give it its gem instead.
	if(HasTileAt(Above))
		return { AboveTileResult::AccessType::ExistingTile, Above, {} };

	if(HasTileAt(AboveLeft))
		return { AboveTileResult::AccessType::ExistingTile, AboveLeft, {} };

	if(HasTileAt(AboveRight))
		return { AboveTileResult::AccessType::ExistingTile, AboveRight, {} };

	//Spawning gems at other heights than 0 is NOT valid.
	Error_Abort("Incorrect board shape");
}

bool Board::HasTileAt(TileCoordinate TileCoordinateInstance)
{
	return TilesContainer.find(TileCoordinateInstance) != TilesContainer.end();
}

void Tile::SwapTileObjects(Tile* TileRef, Tile* OtherTileRef)
{
	std::swap(TileRef->GemType, OtherTileRef->GemType);
	std::swap(TileRef->GemEntityHandleRef, OtherTileRef->GemEntityHandleRef);
	std::swap(TileRef->GemEntityRef, OtherTileRef->GemEntityRef);
}

void Tile::SetGemTypeFromSpawn(Tile* TileRef, uint8_t GemType)
{
	TileRef->GemType = GemType;
	TileRef->RedrawGemType();
}

void Tile::RedrawGemType()
{
	auto EntityX = GemEntityRef->ReadEntityX();
	auto EntityY = GemEntityRef->ReadEntityY();
	GemEntityRef->SendEntityData(Rectangle{ EntityX, EntityY, 5, 5, GemColors[GemType] });
}

void Tile::RedrawGem()
{
	GemEntityRef->SendEntityData(Rectangle{ TileX + GemLeftOffset, TileY + GemTopOffset, 5, 5, GemColors[GemType] });
}

void Tile::DrawTile()
{
	MyEntityRef->SendEntityData(Rectangle{ TileX, TileY, TileWidth, TileHeight, TileColor });
}

bool Tile::IsEmpty()
{
	return GemType == EmptyGemType;
}

VectorF Board::GetGemPositionAtTile(TileCoordinate TileCoordinateInstance) const
{
	float CoordinateX = OffsetX + TileCoordinateInstance.X * TileWidth + GemOffset.X;
	float CoordinateY = OffsetY + TileCoordinateInstance.Y * TileHeight + GemOffset.Y;
	return VectorF(CoordinateX, CoordinateY);
}

void Tile::SetUpGemSprite()
{
	GemEntityHandleRef = EntityManagerRef->AddEntityWithParameters(60, ERenderType::Rectangle);
	auto GemEntityRefOpt = EntityManagerRef->GetEntityRefByID(*GemEntityHandleRef);

	if (!GemEntityRefOpt.has_value())
	{
		Error_Abort("Entity of gem could not be created!!");
		return;
	}

	GemEntityRef = *GemEntityRefOpt;
}

void GemMovement::AddMovement(EntityHandle* EntityHandleInstance, Entity* EntityRef, VectorF InitialPositionIn, VectorF FinalPositionIn)
{
	//Sort gems by Y value, so that they replay in order.
	GemsMovementList[*EntityHandleInstance].emplace_back(InitialPositionIn, FinalPositionIn, EntityRef);
	std::sort(GemsMovementList[*EntityHandleInstance].begin(), GemsMovementList[*EntityHandleInstance].end());
}

void GemMovement::TriggerMovementsTick(double Delta)
{
	for (auto GemMovements = GemsMovementList.begin(); GemMovements != GemsMovementList.end();)
	{
		auto& Movement = GemMovements->second;

		//Convert delta from ms to seconds.
		Movement[0].DurationPassed += Delta / 1000;
		Movement[0].EntityRef->SetEntityPosition
		(
			Lerp(Movement[0].InitialPosition.X, Movement[0].FinalPosition.X, static_cast<double>(Movement[0].DurationPassed / Movement[0].MaxDuration)),
			Lerp(Movement[0].InitialPosition.Y, Movement[0].FinalPosition.Y, static_cast<double>(Movement[0].DurationPassed / Movement[0].MaxDuration))
		);

		//Once movement is done, check if we need to keep gem in list of gems that should move.
		if (Movement[0].DurationPassed >= Movement[0].MaxDuration)
		{
			Movement.erase(Movement.begin());
			if (Movement.size() == 0)
			{
				GemMovements = GemsMovementList.erase(GemMovements);
				continue;
			}
		}
		GemMovements++;
	}
}
