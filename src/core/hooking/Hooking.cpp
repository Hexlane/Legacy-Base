#include "Hooking.hpp"
#include "game/Script/script.h"
#include "BaseHook.hpp"
#include "DetourHook.hpp"
#include "VMTHook.hpp"
#include "game/hooks/Hooks.hpp"
#include "game/pointers/Pointers.hpp"
#include "game/Script/script_mgr.h"

namespace NewBase
{
	
bool run_script_threads(std::uint32_t ops_to_execute)
{
    TRY_CLAUSE
    {
        if (g_Running)
        {
            g_script_mgr.tick();
        }
        return BaseHook::Get<run_script_threads, DetourHook<functions::run_script_threads_t>>()->Original()(ops_to_execute);
    }
     EXCEPT_CLAUSE
    return false;
}






	Hooking::Hooking()
	{
		BaseHook::Add<Window::WndProc>(new DetourHook("WndProc", Pointers.WndProc, Window::WndProc));

		auto swapchain = new VMTHook<SwapChain::VMTSize>("SwapChain", *Pointers.SwapChain);
		swapchain->Hook(SwapChain::VMTPresentIdx, SwapChain::Present);
		swapchain->Hook(SwapChain::VMTResizeBuffersIdx, SwapChain::ResizeBuffers);
		BaseHook::Add<SwapChain::Present>(swapchain);
       
		BaseHook::Add<run_script_threads>(new DetourHook("Script hook", Pointers.m_run_script_threads, run_script_threads));

	}

	Hooking::~Hooking()
	{
		DestroyImpl();
	}

	bool Hooking::Init()
	{
		return GetInstance().InitImpl();
	}

	void Hooking::Destroy()
	{
		GetInstance().DestroyImpl();
	}

	bool Hooking::InitImpl()
	{
		BaseHook::EnableAll();
		m_MinHook.ApplyQueued();

		return true;
	}

	void Hooking::DestroyImpl()
	{
		BaseHook::DisableAll();
		m_MinHook.ApplyQueued();

		for (auto it : BaseHook::Hooks())
		{
			delete it;
		}
		BaseHook::Hooks().clear();
	}
}
