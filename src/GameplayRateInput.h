#ifndef GAMEPLAY_RATE_INPUT_H
#define GAMEPLAY_RATE_INPUT_H

/** Captures a shortcut for its whole key lifetime, even if Shift is released first. */
class GameplayRateInput
{
public:
	enum Arrow { Up, Right, Down, Left, NumArrows, NotArrow };
	enum Event { Press, Repeat, Release };
	struct Result
	{
		bool handled;
		int direction;
	};

	Result Handle(Arrow arrow, Event event, bool shift, bool enabled)
	{
		if(arrow < Up || arrow >= NumArrows)
			return {false, 0};
		if(event == Press)
		{
			// A fresh press also recovers from a release lost during focus changes.
			m_Captured[arrow] = enabled && shift;
			return {m_Captured[arrow], m_Captured[arrow] ?
				(arrow == Up || arrow == Right ? 1 : -1) : 0};
		}
		const bool captured = m_Captured[arrow];
		if(event == Release)
			m_Captured[arrow] = false;
		return {captured, 0};
	}

	bool IsCaptured(Arrow arrow) const
	{
		return arrow >= Up && arrow < NumArrows && m_Captured[arrow];
	}

private:
	bool m_Captured[NumArrows] = {};
};

#endif
