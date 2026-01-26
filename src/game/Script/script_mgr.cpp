#include "common.hpp"
#include "game/gta/array.h"
#include "game/gta/scrThreadContext.h"
#include "game/gta/tls_context.h"
#include "game/gta/gta_util.h"
#include "game/gta/invoker/invoker.h"
#include "game/pointers/Pointers.hpp"
#include "script_mgr.h"



namespace NewBase
{
	void script_mgr::add_script(std::unique_ptr<script> script)
	{
		std::lock_guard lock(m_mutex);
		m_scripts.push_back(std::move(script));
	}

	void script_mgr::remove_all_scripts()
	{
		std::lock_guard lock(m_mutex);
		m_scripts.clear();
	}

	void script_mgr::tick()
	{
		gta_util::execute_as_script("main_persistent"_J, std::mem_fn(&script_mgr::tick_internal), this);
	}

	void script_mgr::tick_internal()
	{
		static bool ensure_main_fiber = (ConvertThreadToFiber(nullptr), true);
		static bool ensure_native_handlers = (g_native_invoker.cache_handlers(), true);

		std::lock_guard lock(m_mutex);
		for (auto const &script : m_scripts)
		{
			script->tick();
		}
	}
}
