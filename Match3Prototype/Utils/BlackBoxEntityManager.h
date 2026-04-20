#pragma once

#include <print>
#include <string>
#include <format>
#include <source_location>

class EntityManager;
struct EntityHandle;

struct TestContext
{
	bool TestFailed = false;

	void Fail(std::string_view Message, const std::source_location& Location = std::source_location::current())
	{
		std::println("[TEST CASE FAILED] {}({}:{}) {} - {}", Location.file_name(), Location.line(), Location.column(), Location.function_name(), Message);
		TestFailed = true;
	}
};

class BlackBoxEntityManager
{
public:
	BlackBoxEntityManager(EntityManager& EntityManagerRefIn);
	void RunAllTests();
private:
	void PrintEntity(EntityHandle EntityHandleInstance);

	void TestVersionControl0();
	void TestVersionControl1();
	void TestVersionControl2();
	void TestVersionControl3();

	void TestRemoveEntity0();
	void TestRemoveEntity1();
	void TestRemoveEntity2();

	void TestEntityChangeRenderData0();
	void TestEntityChangeRenderData1();
	void TestEntityChangeRenderData2();

	void TestChangeRenderData0();

	EntityManager& EntityManagerRef;
};
