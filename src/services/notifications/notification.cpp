#include "notification.hpp"

namespace big
{
	notification::notification(const std::string title, const std::string message, NotificationType type, const std::chrono::high_resolution_clock::duration lifetime) :
	    m_title(title),
	    m_message(message),
	    m_identifier(std::hash<std::string>{}(title + message)),
	    m_type(type),
	    m_lifetime(lifetime),
	    m_destroy_time(std::chrono::high_resolution_clock::now() + lifetime),
	    m_counter(1)
	{
		switch (type)
		{
		case NotificationType::DANGER:
			m_color = ImVec4(0.9f, 0.2f, 0.2f, 1.00f); // Merah cerah
			break;
		case NotificationType::WARNING:
			m_color = ImVec4(0.9f, 0.7f, 0.2f, 1.00f); // Kuning keemasan
			break;
		case NotificationType::SUCCESS:
			m_color = ImVec4(0.2f, 0.9f, 0.2f, 1.00f); // Hijau cerah
			break;
		default:
		case NotificationType::INFO:
			m_color = ImVec4(0.6f, 0.2f, 0.8f, 1.00f); // Ungu
			break;
		}
	}

	// linear fade out in the last 600ms
	const float notification::alpha() const
	{
		const auto remaining_time = std::chrono::duration_cast<std::chrono::milliseconds>(m_destroy_time - std::chrono::high_resolution_clock::now());
		if (remaining_time < 300ms)
		{
			return (float)remaining_time.count() / 300.f;
		}
		return 1.0f;
	}

	void notification::reset()
	{
		++m_counter;

		m_destroy_time = std::chrono::high_resolution_clock::now() + m_lifetime;
	}

	bool notification::should_be_destroyed() const
	{
		return m_destroy_time < std::chrono::high_resolution_clock::now();
	}
}
