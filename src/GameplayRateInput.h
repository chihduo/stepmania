#ifndef GAMEPLAY_RATE_INPUT_H
#define GAMEPLAY_RATE_INPUT_H

/** Captures a shortcut for its whole key lifetime, even if Shift is released first. */
class GameplayRateInput
{
public:
	enum Arrow { Up, Right, Down, Left, NumArrows, NotArrow };
	enum Event { Press, Repeat, Release };
	enum Action { NoChange, Faster, Slower, ResetToNormal };
	struct Result
	{
		bool handled;
		Action action;
	};

	Result Handle(Arrow arrow, Event event, bool shift, bool enabled,
		bool oppositeDown = false)
	{
		if(arrow < Up || arrow >= NumArrows)
			return {false, NoChange};
		if(event == Press)
		{
			// A fresh press also recovers from a release lost during focus changes.
			m_Captured[arrow] = enabled && shift;
			if(!m_Captured[arrow])
				return {false, NoChange};
			// Use the event's physical snapshot, not a possibly stale capture flag.
			if(oppositeDown)
				return {true, ResetToNormal};
			return {true, arrow == Up || arrow == Right ? Faster : Slower};
		}
		const bool captured = m_Captured[arrow];
		if(event == Release)
			m_Captured[arrow] = false;
		return {captured, NoChange};
	}

	bool IsCaptured(Arrow arrow) const
	{
		return arrow >= Up && arrow < NumArrows && m_Captured[arrow];
	}

private:
	bool m_Captured[NumArrows] = {};
};

#endif
