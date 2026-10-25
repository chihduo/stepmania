#ifndef GAMEPLAY_RATE_H
#define GAMEPLAY_RATE_H

#include <algorithm>
#include <cmath>

/** A temporary song rate. Saved modifiers and chart timing are never changed. */
class GameplayRate
{
public:
	bool Change(float currentRate, int direction)
	{
		if(direction != -1 && direction != 1)
			return false;
		const float previous = m_Active ? m_Rate : currentRate;
		if(!std::isfinite(previous) || previous <= 0)
			return false;
		// Keep three decimal places, including nonstandard starting rates.
		const float next = std::max(0.2f, std::min(3.0f,
			std::round((previous + direction * 0.1f) * 1000.0f) / 1000.0f));
		// A pre-existing out-of-range option must not reverse the shortcut.
		if(direction * (next - previous) < 0.0001f)
			return false;
		return Set(currentRate, next);
	}

	// Set an absolute live rate without losing the original rate used at cleanup.
	bool Set(float currentRate, float next)
	{
		const float previous = m_Active ? m_Rate : currentRate;
		if(!std::isfinite(previous) || previous <= 0 || !std::isfinite(next) ||
			next < 0.2f || next > 3.0f || std::fabs(next - previous) < 0.0001f)
			return false;
		if(!m_Active)
			m_OriginalRate = currentRate;
		m_Rate = next;
		m_Active = true;
		return true;
	}

	void Reset() { *this = GameplayRate(); }
	bool IsActive() const { return m_Active; }
	float GetRate() const { return m_Rate; }
	// CMod/MMod retain their original spacing while the song clock accelerates.
	float GetScrollSpeedRate(float normalRate) const
	{
		return m_Active ? m_OriginalRate : normalRate;
	}

private:
	bool m_Active = false;
	float m_OriginalRate = 1.0f;
	float m_Rate = 1.0f;
};

#endif
