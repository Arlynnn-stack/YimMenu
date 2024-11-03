#include "backend/player_command.hpp"
#include "core/enums.hpp"
#include "core/scr_globals.hpp"
#include "gta_util.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "util/world_model.hpp"


namespace big
{
	class wood_crash : player_command
	{
		using player_command::player_command;

		virtual CommandAccessLevel get_access_level() override
		{
			return CommandAccessLevel::TOXIC;
		}

		virtual void execute(player_ptr player, const command_arguments& args, const std::shared_ptr<command_context> ctx) override
		{
			// Hash objek yang akan digunakan dalam crash
			Hash loghash = 1581872401; // prop_log_ac

			// Meminta model untuk objek
			STREAMING::REQUEST_MODEL(loghash);
			while (!STREAMING::HAS_MODEL_LOADED(loghash))
			{
				script::get_current()->yield(); // Tunggu hingga model selesai dimuatkan
			}

			// Mendapatkan koordinat pemain sasaran
			Vector3 targetCoords =
			    ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_player_service->get_selected()->id()), false);

			// Mengulang untuk mencipta dan memadamkan objek
			for (int i = 0; i < 100; ++i) // Ulang 100 kali
			{
				// Cipta objek di sekitar pemain sasaran menggunakan fungsi spawn
				Object ac = big::world_model::spawn(loghash, targetCoords, true);

				// Tetapkan ciri objek: godmode, kelihatan, dan tiada collision
				if (ENTITY::DOES_ENTITY_EXIST(ac))
				{
					ENTITY::SET_ENTITY_INVINCIBLE(ac, true);        // Godmode
					ENTITY::SET_ENTITY_VISIBLE(ac, true, false);    // Visible
					ENTITY::SET_ENTITY_COLLISION(ac, false, false); // No collision

					// Biarkan objek kekal seketika sebelum dipadamkan
					script::get_current()->yield(500ms);

					// Padamkan objek selepas tempoh yang ditetapkan
					ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&ac);
					ENTITY::DELETE_ENTITY(&ac);

					script::get_current()->yield(300ms);
				}
			}

			// Nyahmuat model selepas selesai digunakan
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(loghash);

			g_notification_service.push_warning("Crash", "Crash has been sent to targeted player :3");
		}

		virtual void execute(const command_arguments& args, const std::shared_ptr<command_context> ctx = std::make_shared<default_command_context>()) override
		{
			// Jika perlu, urus argumen dan konteks di sini
		}
	};

	wood_crash g_wood_crash("woodcrash", "Jungle Crash", "Will crash other player close to target\nCommand : woodcrash", 0);
}
