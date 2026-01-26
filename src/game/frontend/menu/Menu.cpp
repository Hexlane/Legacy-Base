#include "Menu.hpp"
#include "game/gta/invoker/natives.h"
namespace NewBase
{
	void Menu::Main()
	{
		if (!GUI::IsOpen())
			return;

		if (ImGui::Begin("Test"))
		{
			if (ImGui::Button("Test Native"))
			{
				ENTITY::SET_ENTITY_HEALTH(PLAYER::PLAYER_PED_ID(), 0, 0,0);
			}
		}
		ImGui::End();
	}
}
