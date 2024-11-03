#pragma once
#include "natives.hpp"
#include "pointers.hpp"
#include "script.hpp"

struct world_model_bypass
{
	inline static memory::byte_patch* m_world_model_spawn_bypass;
};

namespace big::world_model
{
	inline Object spawn(Hash hash, Vector3 location = Vector3(), bool is_networked = true)
	{
		// Gunakan STREAMING::REQUEST_MODEL dan tunggu model dimuatkan
		STREAMING::REQUEST_MODEL(hash);
		while (!STREAMING::HAS_MODEL_LOADED(hash))
		{
			script::get_current()->yield(); // Tunggu hingga model selesai dimuatkan
		}

		// Bypass sekatan model spawn
		world_model_bypass::m_world_model_spawn_bypass->apply();

		// Cipta objek di lokasi yang ditentukan
		const auto object = OBJECT::CREATE_OBJECT(hash, location.x, location.y, location.z, is_networked, false, false);

		// Pulihkan bypass selepas penciptaan objek
		world_model_bypass::m_world_model_spawn_bypass->restore();
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash); // Bebaskan model daripada memori

		return object;
	}
}
