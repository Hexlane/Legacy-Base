#pragma once
#include <d3d11.h>
#include <windows.h>
#include "game/gta/fwddec.h"
#include "game/gta/natives.h"

namespace NewBase
{
namespace functions
{
	using get_native_handler_t = rage::scrNativeHandler(*)(rage::scrNativeRegistrationTable*, rage::scrNativeHash);
	using fix_vectors_t = void(*)(rage::scrNativeCallContext*);
	using run_script_threads_t = bool(*)(std::uint32_t ops_to_execute);
}


	struct PointerData
	{
		HWND Hwnd;
		CPedFactory **m_ped_factory{};
		PVOID m_init_native_tables;
		rage::scrNativeRegistrationTable *m_native_registration_table{};
		functions::get_native_handler_t m_get_native_handler{};
		functions::fix_vectors_t m_fix_vectors{};
		functions::run_script_threads_t m_run_script_threads{};
		rage::atArray<GtaThread*> *m_script_threads{};
		std::int64_t** m_script_globals;
	};

	struct Pointers : PointerData
	{
		bool Init();
	};

	inline NewBase::Pointers Pointers;
}
