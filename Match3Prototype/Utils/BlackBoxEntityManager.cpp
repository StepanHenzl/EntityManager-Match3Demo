#include "BlackBoxEntityManager.h"

#include "EntityHandle.h"
#include "EntityManager.h"
#include "EntityBatchData.h"
#include "RenderTypes.h"

BlackBoxEntityManager::BlackBoxEntityManager(EntityManager& EntityManagerRefIn) : EntityManagerRef(EntityManagerRefIn)
{

}

void BlackBoxEntityManager::RunAllTests()
{
	TestVersionControl0();
	TestVersionControl1();
	TestVersionControl2();
	TestVersionControl3();

	TestRemoveEntity0();
	TestRemoveEntity1();
	TestRemoveEntity2();

	TestEntityChangeRenderData0();
	TestEntityChangeRenderData1();
	TestEntityChangeRenderData2();

	TestChangeRenderData0();
}

std::string PrintBatchData(Rectangle RectangleData)
{
	return std::format("Rectangle: X={} Y={} Width={} Height={} Color={}", RectangleData.X, RectangleData.Y, RectangleData.Width, RectangleData.Height, RectangleData.Color);
}

std::string PrintBatchData(Image ImageData)
{
	return std::format("Image: X={} Y={}", ImageData.X, ImageData.Y);
}

void BlackBoxEntityManager::PrintEntity(EntityHandle EntityHandleInstance)
{
	auto& EntityLocations = EntityManagerRef.ReadEntityLocations();
	auto EntityLocation = EntityLocations[EntityHandleInstance.ID];

	std::println("Entity: ID={} Version={} Layer={} RenderType={} RenderOrder={}", EntityHandleInstance.ID, EntityHandleInstance.Version, EntityLocation.LayerZ, std::to_underlying(EntityLocation.RenderType), EntityLocation.RenderOrder);

	auto& RenderBatchData = EntityManagerRef.ReadEntityBatchData();
	auto EntityBatchData = RenderBatchData.DataPerLayerZ[EntityLocation.LayerZ].RectanglesData[EntityLocation.RenderOrder];

	std::println("{}", PrintBatchData(EntityBatchData));
}

void BlackBoxEntityManager::TestVersionControl0()
{
	TestContext TestContextInstance;

	auto EntityHandle = EntityManagerRef.AddEntity();
	uint32_t ExpectedVersion = 0;

	if (EntityHandle->Version != ExpectedVersion)
	{
		TestContextInstance.Fail(std::format("Entity 0 version should be 0 but is {}", EntityHandle->Version));
	}

	if (! TestContextInstance.TestFailed)
	{
		std::println("[PASS] TestVersionControl0");
	}

}

void BlackBoxEntityManager::TestVersionControl1()
{
	TestContext TestContextInstance;

	auto EntityHandle = EntityManagerRef.AddEntity();
	EntityManagerRef.RemoveEntityByID(*EntityHandle);
	EntityHandle = EntityManagerRef.AddEntity();
	uint32_t ExpectedVersion = 1;

	if (EntityHandle->Version != ExpectedVersion)
	{
		TestContextInstance.Fail(std::format("Entity 0 version should be 1 but is {}", EntityHandle->Version));
	}

	if (!TestContextInstance.TestFailed)
	{
		std::println("[PASS] TestVersionControl1");
	}

}

void BlackBoxEntityManager::TestVersionControl2()
{
	TestContext TestContextInstance;

	auto EntityHandle0 = EntityManagerRef.AddEntity();
	auto EntityHandle1 = EntityManagerRef.AddEntity();
	EntityManagerRef.RemoveEntityByID(*EntityHandle0);
	uint32_t ExpectedVersion = 0;

	if (EntityHandle1->Version != ExpectedVersion)
	{
		TestContextInstance.Fail(std::format("Entity 1 version should be 0 but is {}", EntityHandle1->Version));
	}

	//Consume recycled ID so future tests are not affected by reuse
	EntityHandle0 = EntityManagerRef.AddEntity();

	if (!TestContextInstance.TestFailed)
	{
		std::println("[PASS] TestVersionControl2");
	}

}

void BlackBoxEntityManager::TestVersionControl3()
{
	TestContext TestContextInstance;

	auto EntityHandle0 = EntityManagerRef.AddEntity();
	EntityManagerRef.RemoveEntityByID(*EntityHandle0);

	EntityHandle0 = EntityManagerRef.AddEntity();
	auto EntityHandle1 = EntityManagerRef.AddEntity();
	auto EntityHandle2 = EntityManagerRef.AddEntity();
	auto EntityHandle3 = EntityManagerRef.AddEntity();

	EntityManagerRef.RemoveEntityByID(*EntityHandle1);
	EntityManagerRef.RemoveEntityByID(*EntityHandle0);
	EntityManagerRef.RemoveEntityByID(*EntityHandle2);

	//Order must match the removal order to ensure correct handle reuse.
	EntityHandle1 = EntityManagerRef.AddEntity();
	EntityHandle0 = EntityManagerRef.AddEntity();
	EntityHandle2 = EntityManagerRef.AddEntity();

	uint32_t ExpectedVersion0 = 2;
	uint32_t ExpectedVersion1 = 1;
	uint32_t ExpectedVersion2 = 1;
	uint32_t ExpectedVersion3 = 0;

	if (EntityHandle0->Version != ExpectedVersion0)
	{
		TestContextInstance.Fail(std::format("Entity 0 version should be 2 but is {}", EntityHandle0->Version));
	}

	if (EntityHandle1->Version != ExpectedVersion1)
	{
		TestContextInstance.Fail(std::format("Entity 1 version should be 1 but is {}", EntityHandle1->Version));
	}

	if (EntityHandle2->Version != ExpectedVersion2)
	{
		TestContextInstance.Fail(std::format("Entity 2 version should be 1 but is {}", EntityHandle2->Version));
	}

	if (EntityHandle3->Version != ExpectedVersion3)
	{
		TestContextInstance.Fail(std::format("Entity 3 version should be 0 but is {}", EntityHandle3->Version));
	}

	if (!TestContextInstance.TestFailed)
	{
		std::println("[PASS] TestVersionControl3");
	}

}

void BlackBoxEntityManager::TestRemoveEntity0()
{
	TestContext TestContextInstance;

	auto EntityHandle = EntityManagerRef.AddEntity();
	auto WasRemoveSucesfull = EntityManagerRef.RemoveEntityByID(*EntityHandle);

	if (!WasRemoveSucesfull)
	{
		TestContextInstance.Fail("Remove of entity 0 failed, but should not.");
	}

	//Consume recycled ID so future tests are not affected by reuse
	EntityHandle = EntityManagerRef.AddEntity();

	if (!TestContextInstance.TestFailed)
	{
		std::println("[PASS] TestRemoveEntity0");
	}

}

void BlackBoxEntityManager::TestRemoveEntity1()
{
	TestContext TestContextInstance;

	auto EntityHandle = EntityManagerRef.AddEntity();
	auto WasRemoveSucesfull = EntityManagerRef.RemoveEntityByID(*EntityHandle);
	auto WasRepeatedRemoveSucesfull = EntityManagerRef.RemoveEntityByID(*EntityHandle);

	if (WasRepeatedRemoveSucesfull)
	{
		TestContextInstance.Fail("Repeated remove of entity 0 succeed, but should not.");
	}

	//Consume recycled ID so future tests are not affected by reuse
	EntityHandle = EntityManagerRef.AddEntity();

	if (!TestContextInstance.TestFailed)
	{
		std::println("[PASS] TestRemoveEntity1");
	}

}

void BlackBoxEntityManager::TestRemoveEntity2()
{
	TestContext TestContextInstance;

	auto EntityHandle0 = EntityManagerRef.AddEntity();
	auto EntityHandle1 = EntityManagerRef.AddEntity();
	auto WasRemoveSucesfull1 = EntityManagerRef.RemoveEntityByID(*EntityHandle1);
	auto WasRemoveSucesfull0 = EntityManagerRef.RemoveEntityByID(*EntityHandle0);

	if (!WasRemoveSucesfull0)
	{
		TestContextInstance.Fail("Remove of entity 0 failed, but should not.");
	}

	if (!WasRemoveSucesfull1)
	{
		TestContextInstance.Fail("Remove of entity 1 failed, but should not.");
	}

	//Consume recycled IDs so future tests are not affected by reuse
	EntityHandle1 = EntityManagerRef.AddEntity();
	EntityHandle0 = EntityManagerRef.AddEntity();

	if (!TestContextInstance.TestFailed)
	{
		std::println("[PASS] TestRemoveEntity2");
	}

}

void BlackBoxEntityManager::TestEntityChangeRenderData0()
{
	TestContext TestContextInstance;

	auto EntityHandle0 = EntityManagerRef.AddEntity();
	EntityManagerRef.SetEntityRenderDataByID(*EntityHandle0, ERenderType::Image);

	auto& EntityLocations = EntityManagerRef.ReadEntityLocations();
	auto EntityLocation0 = EntityLocations[EntityHandle0->ID];

	if (EntityLocation0.RenderType != ERenderType::Image)
	{
		TestContextInstance.Fail(std::format("Render type of entity 0 is type id {}, but should be Image.", static_cast<int>(EntityLocation0.RenderType)));
	}

	if (!TestContextInstance.TestFailed)
	{
		std::println("[PASS] TestEntityChangeRenderData0");
	}

}

void BlackBoxEntityManager::TestEntityChangeRenderData1()
{
	TestContext TestContextInstance;

	auto EntityHandle0 = EntityManagerRef.AddEntity();
	//Incorrect handler
	bool WasSettingRenderDataSuccesfull = EntityManagerRef.SetEntityRenderDataByID(EntityHandle{EntityHandle0->ID, EntityHandle0->Version + 1}, ERenderType::Image);

	if (WasSettingRenderDataSuccesfull)
	{
		TestContextInstance.Fail("Setting data with incorrect handler was succesfull, but should not be.");
	}

	if (!TestContextInstance.TestFailed)
	{
		std::println("[PASS] TestEntityChangeRenderData1");
	}

}

void BlackBoxEntityManager::TestEntityChangeRenderData2()
{
	TestContext TestContextInstance;

	auto EntityHandle0 = EntityManagerRef.AddEntity();
	auto EntityHandle1 = EntityManagerRef.AddEntity();
	EntityManagerRef.SetEntityRenderDataByID(*EntityHandle0, ERenderType::Image);
	auto EntityHandle2 = EntityManagerRef.AddEntity();
	EntityManagerRef.RemoveEntityByID(*EntityHandle1);

	auto& EntityLocations = EntityManagerRef.ReadEntityLocations();
	auto EntityLocation0 = EntityLocations[EntityHandle0->ID];

	if (EntityLocation0.RenderType != ERenderType::Image)
	{
		TestContextInstance.Fail(std::format("Render type of entity 0 is type id {}, but should be Image.", static_cast<int>(EntityLocation0.RenderType)));
	}

	//Consume recycled ID so future tests are not affected by reuse
	EntityHandle1 = EntityManagerRef.AddEntity();

	if (!TestContextInstance.TestFailed)
	{
		std::println("[PASS] TestEntityChangeRenderData2");
	}
}

void BlackBoxEntityManager::TestChangeRenderData0()
{
	TestContext TestContextInstance;

	Rectangle RectangleData = Rectangle{56, 32, 48, 12, 53};

	auto EntityHandle0 = EntityManagerRef.AddEntity();
	EntityManagerRef.ChangeEntityRenderDataByID(*EntityHandle0, RectangleData);

	auto& EntityLocations = EntityManagerRef.ReadEntityLocations();
	auto EntityLocation0 = EntityLocations[EntityHandle0->ID];

	auto& RenderBatchData = EntityManagerRef.ReadEntityBatchData();
	auto EntityBatchData = RenderBatchData.DataPerLayerZ[EntityLocation0.LayerZ].RectanglesData[EntityLocation0.RenderOrder];

	if (EntityBatchData.X != RectangleData.X || EntityBatchData.Y != RectangleData.Y || EntityBatchData.Width != RectangleData.Width || EntityBatchData.Height != RectangleData.Height || EntityBatchData.Color != RectangleData.Color)
	{
		TestContextInstance.Fail(std::format("Render data of entity 0 should be {}, but instead it is {}.", PrintBatchData(RectangleData), PrintBatchData(EntityBatchData)));
	}

	if (!TestContextInstance.TestFailed)
	{
		std::println("[PASS] TestChangeRenderData0");
	}
}

