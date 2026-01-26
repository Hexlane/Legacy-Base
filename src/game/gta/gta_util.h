#pragma once
#include "common.hpp"
#include "array.h"
#include "ped_factory.h"
#include "player.h"
#include "scrThreadContext.h"
#include "tls_context.h"
#include "game/pointers/Pointers.hpp"
#include "GtaThread.h"


namespace NewBase::gta_util
{
	inline CPed *get_local_ped()
	{
		if (auto ped_factory = *Pointers.m_ped_factory)
		{
			return ped_factory->m_local_ped;
		}

		return nullptr;
	}

	inline CPlayerInfo *get_local_playerinfo()
	{
		if (auto ped_factory = *Pointers.m_ped_factory)
		{
			if (auto ped = ped_factory->m_local_ped)
			{
				return ped->m_playerinfo;
			}
		}

		return nullptr;
	}

	template<typename F, typename... Args>
	void execute_as_script(rage::scrThread* thread, F&& callback, Args&&... args)
	{
		auto tls_ctx   = rage::tlsContext::get();
		auto og_thread = tls_ctx->m_script_thread;

		tls_ctx->m_script_thread           = thread;
		tls_ctx->m_is_script_thread_active = true;

		std::invoke(std::forward<F>(callback), std::forward<Args>(args)...);

		tls_ctx->m_script_thread           = og_thread;
		tls_ctx->m_is_script_thread_active = og_thread != nullptr;
	}

	template<typename F, typename... Args>
	void execute_as_script(rage::joaat_t script_hash, F&& callback, Args&&... args)
	{
		for (auto thread : *Pointers.m_script_threads)
		{
			if (!thread || !thread->m_context.m_thread_id || thread->m_context.m_script_hash != script_hash)
				continue;

			execute_as_script(thread, callback, args...);

			return;
		}
	}
}
