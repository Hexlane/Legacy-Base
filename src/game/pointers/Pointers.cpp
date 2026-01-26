#include "Pointers.hpp"
#include "game/frontend/xorstr.h"
#include "core/memory/BytePatch.hpp"
#include "core/memory/ModuleMgr.hpp"
#include "core/memory/PatternScanner.hpp"
#include "game/gta/joaat.h"

namespace NewBase
{
	bool Pointers::Init()
	{
		const auto gta5 = ModuleMgr.Get("GTA5.exe"_J);
		if (!gta5)
		{
			g_log.send("FATAL", "Could not find [{}], is this GTA5?", gta5->Name());
			return false;
		}

		auto scanner = PatternScanner(gta5);


		constexpr auto PedfPTR = Pattern<("48 8B 05 ? ? ? ? 48 8B 48 08 48 85 C9 74 52 8B 81")>("PF");
		scanner.Add(PedfPTR, [this](PointerCalculator ptr) {
			m_ped_factory = ptr.Add(3).Rip().As<CPedFactory**>();
		});

		constexpr auto NHPtr = Pattern<"48 8D 0D ? ? ? ? 48 8B 14 FA E8 ? ? ? ? 48 85 C0 75 0A">("NH");
		scanner.Add(NHPtr, [this](PointerCalculator ptr) {
			    m_init_native_tables = ptr.Sub(37).As<PVOID>();
				m_native_registration_table = ptr.Add(3).Rip().As<rage::scrNativeRegistrationTable*>();
				m_get_native_handler = ptr.Add(12).Rip().As<functions::get_native_handler_t>();
		});

		constexpr auto FVPtr = Pattern<"83 79 18 00 48 8B D1 74 4A FF 4A 18 48 63 4A 18 48 8D 41 04 48 8B 4C CA">("FV");
		scanner.Add(FVPtr, [this](PointerCalculator ptr) {
			   m_fix_vectors = ptr.As<functions::fix_vectors_t>();
		});

		constexpr auto STptr = Pattern<"45 33 F6 8B E9 85 C9 B8">("ST");
		scanner.Add(STptr, [this](PointerCalculator ptr) {
			    m_script_threads = ptr.Sub(4).Rip().Sub(8).As<decltype(m_script_threads)>();
				m_run_script_threads = ptr.Sub(0x1F).As<functions::run_script_threads_t>();
		});

		constexpr auto SGptr = Pattern<"48 8D 15 ? ? ? ? 4C 8B C0 E8 ? ? ? ? 48 85 FF 48 89 1D">("SG");
		scanner.Add(STptr, [this](PointerCalculator ptr) {
			   m_script_globals = ptr.Add(3).Rip().As<std::int64_t**>();
		});


	

		if (!scanner.Scan())
		{
			g_log.send("FATAL", "Some patterns could not be found, unloading.");

			return false;
		}

		if (Hwnd = FindWindow("grcWindow", nullptr); !Hwnd)
		{
			g_log.send("FATAL", "Failed to grab game window, unloading.");

			return false;
		}

		return true;
	}
}
