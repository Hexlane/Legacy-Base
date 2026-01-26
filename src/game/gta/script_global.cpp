#include "script_global.h"
#include "game/pointers/Pointers.hpp"

namespace NewBase
{
	void* script_global::get() const
	{
		return Pointers.m_script_globals[m_index >> 0x12 & 0x3F] + (m_index & 0x3FFFF);
	}
}
