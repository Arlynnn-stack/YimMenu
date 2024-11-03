#include "gui.hpp"

#include "lua/lua_manager.hpp"
#include "natives.hpp"
#include "renderer/renderer.hpp"
#include "script.hpp"
#include "views/view.hpp"

#include <imgui.h>

namespace big
{
	/**
	 * @brief The later an entry comes in this enum to higher up it comes in the z-index.
	 */
	enum eRenderPriority
	{
		// low priority
		ESP,
		CONTEXT_MENU,

		// medium priority
		MENU = 0x1000,
		VEHICLE_CONTROL,
		LUA,

		// high priority
		INFO_OVERLAY = 0x2000,
		CMD_EXECUTOR,

		GTA_DATA_CACHE = 0x3000,
		ONBOARDING,

		// should remain in a league of its own
		NOTIFICATIONS = 0x4000,
	};

	gui::gui() :
	    m_is_open(false),
	    m_override_mouse(false)
	{
		g_renderer.add_dx_callback(view::notifications, eRenderPriority::NOTIFICATIONS);
		g_renderer.add_dx_callback(view::onboarding, eRenderPriority::ONBOARDING);
		g_renderer.add_dx_callback(view::gta_data, eRenderPriority::GTA_DATA_CACHE);
		g_renderer.add_dx_callback(view::cmd_executor, eRenderPriority::CMD_EXECUTOR);
		g_renderer.add_dx_callback(view::overlay, eRenderPriority::INFO_OVERLAY);

		g_renderer.add_dx_callback(view::vehicle_control, eRenderPriority::VEHICLE_CONTROL);
		g_renderer.add_dx_callback(esp::draw, eRenderPriority::ESP); // TODO: move to ESP service
		g_renderer.add_dx_callback(view::context_menu, eRenderPriority::CONTEXT_MENU);

		g_renderer.add_dx_callback(
		    [this] {
			    dx_on_tick();
		    },
		    eRenderPriority::MENU);

		g_renderer.add_dx_callback(
		    [] {
			    g_lua_manager->draw_always_draw_gui();
		    },
		    eRenderPriority::LUA);

		g_renderer.add_wndproc_callback([](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
			g_lua_manager->trigger_event<menu_event::Wndproc>(hwnd, msg, wparam, lparam);
		});
		g_renderer.add_wndproc_callback([this](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
			wndproc(hwnd, msg, wparam, lparam);
		});
		g_renderer.add_wndproc_callback([](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
			if (g.cmd_executor.enabled && msg == WM_KEYUP && wparam == VK_ESCAPE)
			{
				g.cmd_executor.enabled = false;
			}
		});


		dx_init();

		g_gui = this;
		g_renderer.rescale(g.window.gui_scale);
	}

	gui::~gui()
	{
		g_gui = nullptr;
	}

	bool gui::is_open()
	{
		return m_is_open;
	}

	void gui::toggle(bool toggle)
	{
		m_is_open = toggle;

		toggle_mouse();
	}

	void gui::override_mouse(bool override)
	{
		m_override_mouse = override;

		toggle_mouse();
	}

	void gui::dx_init()
	{
		// Definisi warna-warna yang digunakan dalam GUI menggunakan ImVec4
		// Format ImVec4 adalah (red, green, blue, alpha) dengan nilai antara 0.0 hingga 1.0
		static auto bgColor     = ImVec4(0.09f, 0.094f, 0.129f, .9f);  // Warna latar belakang (gelap)
		static auto primary     = ImVec4(0.172f, 0.380f, 0.909f, 1.f); // Warna utama (biru terang)
		static auto secondary   = ImVec4(0.443f, 0.654f, 0.819f, 1.f); // Warna sekunder (biru kelabu)
		static auto whiteBroken = ImVec4(0.792f, 0.784f, 0.827f, 1.f); // Warna hampir putih (kelabu terang)

		// Mendapatkan dan mengubah gaya ImGui
		auto& style = ImGui::GetStyle();

		// Menetapkan padding dan rounding untuk window
		style.WindowPadding     = ImVec2(15, 15); // Padding untuk window
		style.WindowRounding    = 10.f;           // Rounding untuk sudut window
		style.WindowBorderSize  = 0.f;            // Tiada border pada window
		style.FramePadding      = ImVec2(5, 5);   // Padding untuk frame
		style.FrameRounding     = 4.0f;           // Rounding untuk sudut frame
		style.ItemSpacing       = ImVec2(12, 8);  // Spacing antara item
		style.ItemInnerSpacing  = ImVec2(8, 6);   // Spacing dalam item
		style.IndentSpacing     = 25.0f;          // Spacing untuk indentasi
		style.ScrollbarSize     = 15.0f;          // Saiz scrollbar
		style.ScrollbarRounding = 9.0f;           // Rounding untuk scrollbar
		style.GrabMinSize       = 5.0f;           // Saiz minimum untuk grab
		style.GrabRounding      = 3.0f;           // Rounding untuk grab
		style.ChildRounding     = 4.0f;           // Rounding untuk child windows

		// Mendapatkan dan menetapkan warna-warna untuk komponen GUI
		auto& colors                  = style.Colors;
		colors[ImGuiCol_Text]         = ImGui::ColorConvertU32ToFloat4(g.window.text_color); // Warna teks
		colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f); // Warna teks yang dinonaktifkan
		colors[ImGuiCol_WindowBg] = ImGui::ColorConvertU32ToFloat4(g.window.background_color); // Warna latar belakang window
		colors[ImGuiCol_ChildBg] = ImGui::ColorConvertU32ToFloat4(g.window.background_color); // Warna latar belakang child window
		colors[ImGuiCol_PopupBg]      = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);   // Warna latar belakang popup
		colors[ImGuiCol_Border]       = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);   // Warna border
		colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);   // Bayangan border
		colors[ImGuiCol_FrameBg]      = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);   // Warna latar belakang frame
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f); // Warna latar belakang frame ketika hovered
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f); // Warna latar belakang frame ketika aktif
		colors[ImGuiCol_TitleBg]       = ImVec4(0.10f, 0.09f, 0.12f, 1.00f); // Warna latar belakang tajuk
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f); // Warna latar belakang tajuk yang dilipat
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f); // Warna latar belakang tajuk ketika aktif
		colors[ImGuiCol_MenuBarBg]     = ImVec4(0.10f, 0.09f, 0.12f, 1.00f); // Warna latar belakang menu bar
		colors[ImGuiCol_ScrollbarBg]   = ImVec4(0.10f, 0.09f, 0.12f, 1.00f); // Warna latar belakang scrollbar
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f); // Warna grab scrollbar
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f); // Warna grab scrollbar ketika hovered
		colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.06f, 0.05f, 0.07f, 1.00f); // Warna grab scrollbar ketika aktif
		colors[ImGuiCol_CheckMark]            = ImVec4(1.00f, 0.98f, 0.95f, 0.61f); // Warna tanda semak
		colors[ImGuiCol_SliderGrab]           = ImVec4(0.80f, 0.80f, 0.83f, 0.31f); // Warna grab slider
		colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.06f, 0.05f, 0.07f, 1.00f); // Warna grab slider ketika aktif
		colors[ImGuiCol_Button]               = ImVec4(0.24f, 0.23f, 0.29f, 1.00f); // Warna butang
		colors[ImGuiCol_ButtonHovered]        = ImVec4(0.24f, 0.23f, 0.29f, 1.00f); // Warna butang ketika hovered
		colors[ImGuiCol_ButtonActive]         = ImVec4(0.56f, 0.56f, 0.58f, 1.00f); // Warna butang ketika aktif
		colors[ImGuiCol_Header]               = ImVec4(0.30f, 0.29f, 0.32f, 1.00f); // Warna header
		colors[ImGuiCol_HeaderHovered]        = ImVec4(0.56f, 0.56f, 0.58f, 1.00f); // Warna header ketika hovered
		colors[ImGuiCol_HeaderActive]         = ImVec4(0.06f, 0.05f, 0.07f, 1.00f); // Warna header ketika aktif
		colors[ImGuiCol_ResizeGrip]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f); // Warna grip untuk resize
		colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.56f, 0.56f, 0.58f, 1.00f); // Warna grip ketika hovered
		colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.06f, 0.05f, 0.07f, 1.00f); // Warna grip ketika aktif
		colors[ImGuiCol_PlotLines]            = ImVec4(0.40f, 0.39f, 0.38f, 0.63f); // Warna garis plot
		colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.25f, 1.00f, 0.00f, 1.00f); // Warna garis plot ketika hovered
		colors[ImGuiCol_PlotHistogram]        = ImVec4(0.40f, 0.39f, 0.38f, 0.63f); // Warna histogram plot
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f); // Warna histogram ketika hovered
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f); // Warna latar belakang teks yang dipilih

		// Menyimpan gaya default untuk pemulihan kemudian
		save_default_style();
	}

	void gui::dx_on_tick()
	{
		// Memeriksa jika GUI dibuka
		if (m_is_open)
		{
			push_theme_colors(); // Menghantar warna tema
			view::root();        // Menggambar latar belakang bingkai
			pop_theme_colors();  // Mengeluarkan warna tema
		}
	}

	void gui::save_default_style()
	{
		// Menyimpan gaya default untuk pemulihan
		memcpy(&m_default_config, &ImGui::GetStyle(), sizeof(ImGuiStyle));
	}

	void gui::restore_default_style()
	{
		// Memulihkan gaya default yang disimpan sebelumnya
		memcpy(&ImGui::GetStyle(), &m_default_config, sizeof(ImGuiStyle));
	}

	void gui::push_theme_colors()
	{
		// Mendapatkan warna butang dari konfigurasi global
		auto button_color = ImGui::ColorConvertU32ToFloat4(g.window.button_color);

		// Menetapkan warna untuk butang ketika hovered dan aktif
		auto button_active_color =
		    ImVec4(button_color.x + 0.33f, button_color.y + 0.33f, button_color.z + 0.33f, button_color.w);
		auto button_hovered_color =
		    ImVec4(button_color.x + 0.15f, button_color.y + 0.15f, button_color.z + 0.15f, button_color.w);

		// Mendapatkan warna frame dari konfigurasi global
		auto frame_color = ImGui::ColorConvertU32ToFloat4(g.window.frame_color);

		// Menetapkan warna untuk frame ketika hovered dan aktif
		auto frame_hovered_color =
		    ImVec4(frame_color.x + 0.14f, frame_color.y + 0.14f, frame_color.z + 0.14f, button_color.w);
		auto frame_active_color =
		    ImVec4(frame_color.x + 0.30f, frame_color.y + 0.30f, frame_color.z + 0.30f, button_color.w);

		// Menghantar warna tema ke ImGui
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::ColorConvertU32ToFloat4(g.window.background_color)); // Warna latar belakang window
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertU32ToFloat4(g.window.text_color)); // Warna teks
		ImGui::PushStyleColor(ImGuiCol_Button, button_color);                                      // Warna butang
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_hovered_color); // Warna butang ketika hovered
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, button_active_color);   // Warna butang ketika aktif
		ImGui::PushStyleColor(ImGuiCol_FrameBg, frame_color);                // Warna latar belakang frame
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frame_hovered_color); // Warna latar belakang frame ketika hovered
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, frame_active_color);   // Warna latar belakang frame ketika aktif
	}






	void gui::pop_theme_colors()
	{
		ImGui::PopStyleColor(8);
	}

	void gui::script_on_tick()
	{
		if (g_gui->m_is_open || g_gui->m_override_mouse)
		{
			for (uint8_t i = 0; i <= 6; i++)
				PAD::DISABLE_CONTROL_ACTION(2, i, true);
			PAD::DISABLE_CONTROL_ACTION(2, 106, true);
			PAD::DISABLE_CONTROL_ACTION(2, 329, true);
			PAD::DISABLE_CONTROL_ACTION(2, 330, true);

			PAD::DISABLE_CONTROL_ACTION(2, 14, true);
			PAD::DISABLE_CONTROL_ACTION(2, 15, true);
			PAD::DISABLE_CONTROL_ACTION(2, 16, true);
			PAD::DISABLE_CONTROL_ACTION(2, 17, true);
			PAD::DISABLE_CONTROL_ACTION(2, 24, true);
			PAD::DISABLE_CONTROL_ACTION(2, 69, true);
			PAD::DISABLE_CONTROL_ACTION(2, 70, true);
			PAD::DISABLE_CONTROL_ACTION(2, 84, true);
			PAD::DISABLE_CONTROL_ACTION(2, 85, true);
			PAD::DISABLE_CONTROL_ACTION(2, 99, true);
			PAD::DISABLE_CONTROL_ACTION(2, 92, true);
			PAD::DISABLE_CONTROL_ACTION(2, 100, true);
			PAD::DISABLE_CONTROL_ACTION(2, 114, true);
			PAD::DISABLE_CONTROL_ACTION(2, 115, true);
			PAD::DISABLE_CONTROL_ACTION(2, 121, true);
			PAD::DISABLE_CONTROL_ACTION(2, 142, true);
			PAD::DISABLE_CONTROL_ACTION(2, 241, true);
			PAD::DISABLE_CONTROL_ACTION(2, 261, true);
			PAD::DISABLE_CONTROL_ACTION(2, 257, true);
			PAD::DISABLE_CONTROL_ACTION(2, 262, true);
			PAD::DISABLE_CONTROL_ACTION(2, 331, true);
		}
	}

	void gui::script_func()
	{
		while (true)
		{
			g_gui->script_on_tick();
			script::get_current()->yield();
		}
	}

	void gui::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (msg == WM_KEYUP && wparam == g.settings.hotkeys.menu_toggle)
		{
			//Persist and restore the cursor position between menu instances.
			static POINT cursor_coords{};
			if (g_gui->m_is_open)
			{
				GetCursorPos(&cursor_coords);
			}
			else if (cursor_coords.x + cursor_coords.y != 0)
			{
				SetCursorPos(cursor_coords.x, cursor_coords.y);
			}

			toggle(g.settings.hotkeys.editing_menu_toggle || !m_is_open);
			if (g.settings.hotkeys.editing_menu_toggle)
				g.settings.hotkeys.editing_menu_toggle = false;
		}
	}

	void gui::toggle_mouse()
	{
		if (m_is_open || g_gui->m_override_mouse)
		{
			ImGui::GetIO().MouseDrawCursor = true;
			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		}
		else
		{
			ImGui::GetIO().MouseDrawCursor = false;
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}
	}
}
