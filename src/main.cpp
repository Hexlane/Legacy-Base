#include "common.hpp"
#include "core/filemgr/FileMgr.hpp"
#include "core/hooking/Hooking.hpp"
#include "core/memory/ModuleMgr.hpp"
#include "game/frontend/GUI.hpp"
#include "game/pointers/Pointers.hpp"
#include "game/Script/script_mgr.h"
#include "game/gta/invoker/natives.h"
#include "game/Script/fiber_pool.h"
namespace NewBase
{

	bool superjumpbool = true;
	void Superjump(bool toggle)
	{
		if (toggle)
		{
			MISC::SET_SUPER_JUMP_THIS_FRAME(PLAYER::PLAYER_ID());
		}
	}
	void run_tick()
	{
		superjumpbool ? Superjump(true) : Superjump(false);
	}

	void script_func()
	{
		while (true)
		{
			TRY_CLAUSE
			{
				run_tick();
			}
			EXCEPT_CLAUSE
			script::get_current()->yield();
		}
	}

	DWORD Main(void*)
	{
		const auto documents = std::filesystem::path(std::getenv("USERPROFILE")) / "Documents";
		FileMgr::Init(documents / "HellBase");

		g_log.attach();

		

		if (!ModuleMgr.LoadModules());
		if (!Pointers.Init());
			
			
		auto fiber_pool_instance = std::make_unique<fiber_pool>(10);


		GUI::Init();
		Hooking::Init();

		g_script_mgr.add_script(std::make_unique<script>(&script_func));

		while (g_Running)
		{
			std::this_thread::sleep_for(100ms);
		}

	
		Hooking::Destroy();
		fiber_pool_instance.reset();
		g_log.detach();

		CloseHandle(g_MainThread);
		FreeLibraryAndExitThread(g_DllInstance, EXIT_SUCCESS);

		return EXIT_SUCCESS;
	}
}

BOOL WINAPI DllMain(HINSTANCE dllInstance, DWORD reason, void*)
{
	using namespace NewBase;

	DisableThreadLibraryCalls(dllInstance);

	if (reason == DLL_PROCESS_ATTACH)
	{
		g_DllInstance = dllInstance;

		g_MainThread = CreateThread(nullptr, 0, Main, nullptr, 0, &g_MainThreadId);
	}
	return true;
}
