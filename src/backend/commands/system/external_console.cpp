#include "backend/bool_command.hpp"

namespace big
{
	class external_console : bool_command
	{
		using bool_command::bool_command;

		virtual void refresh() override
		{
			g_log.toggle_external_console(m_toggle);
		}
	};

	external_console
	    g_external_console("external_console", "BACKEND_EXTERNAL_CONSOLE", "Toggle LynnnMenu Developer Console", g.debug.external_console);
}
