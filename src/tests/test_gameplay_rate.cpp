// Standalone regression checks: g++ -std=c++11 -Isrc src/tests/test_gameplay_rate.cpp -o /tmp/test_gameplay_rate
#include "GameplayRate.h"
#include "GameplayRateInput.h"
#include <cassert>
#include <iostream>
#include <limits>

static void Equal(float actual, float expected)
{
	assert(std::fabs(actual - expected) < 0.0001f);
}

int main()
{
	GameplayRate rate;
	assert(!rate.IsActive());
	assert(rate.Change(1.0f, 1)); Equal(rate.GetRate(), 1.1f);
	assert(rate.Change(1.0f, -1)); Equal(rate.GetRate(), 1.0f);
	assert(rate.Change(1.0f, -1)); Equal(rate.GetRate(), 0.9f);
	// Options may be rebuilt, but the manual rate remains authoritative.
	assert(rate.Change(1.5f, 1)); Equal(rate.GetRate(), 1.0f);
	assert(rate.IsActive()); // Returning to 1x does not end practice.
	for(int i = 0; i < 10000; ++i)
	{
		assert(rate.Change(1.0f, 1));
		assert(rate.Change(1.0f, -1));
	}
	Equal(rate.GetRate(), 1.0f);
	for(int i = 0; i < 40; ++i) rate.Change(1.0f, -1);
	Equal(rate.GetRate(), 0.2f);
	assert(!rate.Change(1.0f, -1));
	for(int i = 0; i < 40; ++i) rate.Change(1.0f, 1);
	Equal(rate.GetRate(), 3.0f);
	assert(!rate.Change(1.0f, 1));
	rate.Reset();
	assert(!rate.IsActive());
	assert(!rate.Change(3.0f, 1));
	assert(!rate.IsActive()); // A bound-only shortcut is not a rate change.
	assert(!rate.Change(0.1f, -1));
	assert(!rate.Change(3.2f, 1));
	assert(!rate.IsActive());
	assert(!rate.Change(0, 1));
	assert(!rate.Change(std::numeric_limits<float>::infinity(), 1));
	assert(!rate.Change(std::numeric_limits<float>::quiet_NaN(), 1));
	assert(!rate.Change(1.0f, 0));
	assert(rate.Change(1.25f, 1)); Equal(rate.GetRate(), 1.35f);
	assert(rate.Change(1.25f, -1)); Equal(rate.GetRate(), 1.25f);
	Equal(rate.GetScrollSpeedRate(1.35f), 1.25f);
	rate.Reset();
	Equal(rate.GetScrollSpeedRate(0.8f), 0.8f);
	assert(rate.Change(0.8f, 1)); Equal(rate.GetRate(), 0.9f);
	Equal(rate.GetScrollSpeedRate(0.9f), 0.8f);

	// CMod 300 at a starting 1.25x: a note one song-second away has unchanged
	// spacing at the change boundary, then travels 1.35/1.25 times as fast.
	rate.Reset();
	const float originalSpacing = 300.0f / 60.0f / rate.GetScrollSpeedRate(1.25f);
	assert(rate.Change(1.25f, 1));
	const float changedSpacing = 300.0f / 60.0f / rate.GetScrollSpeedRate(1.35f);
	Equal(changedSpacing, originalSpacing);
	Equal(changedSpacing * rate.GetRate(), 5.4f);
	// MMod uses the same denominator; XMod follows song time directly.
	Equal(300.0f / (150.0f * rate.GetScrollSpeedRate(1.35f)), 1.6f);
	rate.Reset();
	Equal(rate.GetScrollSpeedRate(1.25f), 1.25f);

	// Reset-to-1x is a live override, distinct from cleaning up after the song.
	assert(!rate.Set(1.0f, 1.0f));
	assert(!rate.IsActive());
	assert(!rate.Set(0.0f, 1.0f));
	assert(!rate.Set(std::numeric_limits<float>::quiet_NaN(), 1.0f));
	assert(!rate.Set(1.0f, std::numeric_limits<float>::infinity()));
	assert(!rate.Set(1.0f, std::numeric_limits<float>::quiet_NaN()));
	assert(!rate.Set(1.0f, 0.1f));
	assert(!rate.Set(1.0f, 3.1f));
	assert(!rate.IsActive());
	assert(rate.Change(1.25f, 1));
	assert(rate.Set(1.5f, 1.0f));
	Equal(rate.GetRate(), 1.0f);
	Equal(rate.GetScrollSpeedRate(1.0f), 1.25f);
	assert(rate.IsActive());
	assert(!rate.Set(1.5f, 1.0f));
	assert(rate.Change(1.5f, 1)); Equal(rate.GetRate(), 1.1f);
	assert(rate.Set(1.5f, 1.0f));
	rate.Reset();
	for(float initial : {0.1f, 0.2f, 0.8f, 1.25f, 3.0f, 3.2f})
	{
		assert(rate.Set(initial, 1.0f));
		Equal(rate.GetRate(), 1.0f);
		Equal(rate.GetScrollSpeedRate(1.0f), initial);
		assert(rate.IsActive());
		rate.Reset();
		assert(!rate.IsActive());
	}

	using Keys = GameplayRateInput;
	Keys keys;
	for(int i = Keys::Up; i < Keys::NumArrows; ++i)
	{
		const auto arrow = static_cast<Keys::Arrow>(i);
		assert(!keys.Handle(arrow, Keys::Press, false, true).handled);
		// Pressing Shift after an ordinary arrow never steals an existing hold.
		assert(!keys.Handle(arrow, Keys::Repeat, true, true).handled);
		assert(!keys.IsCaptured(arrow));
		assert(!keys.Handle(arrow, Keys::Release, true, true).handled);
		// The global release filter precedes the screen shortcut handler.
		assert(!keys.Handle(arrow, Keys::Press, true, false).handled);
		const auto press = keys.Handle(arrow, Keys::Press, true, true);
		assert(press.handled);
		assert(press.action == (arrow == Keys::Up || arrow == Keys::Right ? Keys::Faster : Keys::Slower));
		assert(keys.IsCaptured(arrow));
		const auto repeat = keys.Handle(arrow, Keys::Repeat, true, false);
		assert(repeat.handled && repeat.action == Keys::NoChange);
		// Shift can be released first, or gameplay can end while the key is down.
		const auto shifted = keys.Handle(arrow, Keys::Repeat, false, false);
		assert(shifted.handled && shifted.action == Keys::NoChange);
		assert(keys.IsCaptured(arrow));
		const auto release = keys.Handle(arrow, Keys::Release, false, false);
		assert(release.handled && release.action == Keys::NoChange);
		assert(!keys.IsCaptured(arrow));
		assert(!keys.Handle(arrow, Keys::Press, false, true).handled);
		keys.Handle(arrow, Keys::Release, false, true);
	}
	assert(!keys.Handle(Keys::NotArrow, Keys::Press, true, true).handled);
	assert(!keys.Handle(Keys::Up, Keys::Press, true, false).handled);
	assert(!keys.IsCaptured(Keys::Up));
	keys.Handle(Keys::Up, Keys::Press, true, true);
	keys.Handle(Keys::Right, Keys::Press, true, true);
	assert(keys.IsCaptured(Keys::Up) && keys.IsCaptured(Keys::Right));
	keys.Handle(Keys::Up, Keys::Release, true, true);
	assert(!keys.IsCaptured(Keys::Up) && keys.IsCaptured(Keys::Right));
	// Recover from a missed key release (for example, losing application focus).
	assert(!keys.Handle(Keys::Right, Keys::Press, false, false).handled);
	assert(!keys.IsCaptured(Keys::Right));

	// Both reset pairs work in either press order and either release order.
	for(int i = Keys::Up; i < Keys::NumArrows; ++i)
	for(int reverseRelease = 0; reverseRelease < 2; ++reverseRelease)
	{
		Keys chord;
		const auto first = static_cast<Keys::Arrow>(i);
		const auto second = static_cast<Keys::Arrow>((i + 2) % Keys::NumArrows);
		assert(!chord.Handle(first, Keys::Press, true, false).handled);
		assert(chord.Handle(first, Keys::Press, true, true).action ==
			(first == Keys::Up || first == Keys::Right ? Keys::Faster : Keys::Slower));
		assert(!chord.Handle(second, Keys::Press, true, false, true).handled);
		const auto reset = chord.Handle(second, Keys::Press, true, true, true);
		assert(reset.handled && reset.action == Keys::ResetToNormal);
		assert(chord.IsCaptured(first) && chord.IsCaptured(second));
		for(auto arrow : {first, second})
		{
			const auto held = chord.Handle(arrow, Keys::Repeat, true, true, true);
			assert(held.handled && held.action == Keys::NoChange);
			const auto noShift = chord.Handle(arrow, Keys::Repeat, false, false, true);
			assert(noShift.handled && noShift.action == Keys::NoChange);
		}
		const auto releaseFirst = reverseRelease ? second : first;
		const auto releaseLast = reverseRelease ? first : second;
		const auto release = chord.Handle(releaseFirst, Keys::Release, true, false, true);
		assert(release.handled && release.action == Keys::NoChange);
		assert(!chord.IsCaptured(releaseFirst) && chord.IsCaptured(releaseLast));
		assert(chord.Handle(releaseLast, Keys::Release, false, false).handled);
		assert(!chord.IsCaptured(releaseLast));
		assert(chord.Handle(first, Keys::Press, true, true).action ==
			(first == Keys::Up || first == Keys::Right ? Keys::Faster : Keys::Slower));
	}
	// A stale capture cannot stand in for a physically held opposite arrow.
	Keys stale;
	stale.Handle(Keys::Up, Keys::Press, true, true);
	assert(stale.Handle(Keys::Down, Keys::Press, true, true, false).action == Keys::Slower);
	// A previously held gameplay arrow remains a gameplay hold during a reset.
	Keys heldNote;
	heldNote.Handle(Keys::Up, Keys::Press, false, true);
	assert(heldNote.Handle(Keys::Down, Keys::Press, true, true, true).action == Keys::ResetToNormal);
	assert(!heldNote.IsCaptured(Keys::Up) && heldNote.IsCaptured(Keys::Down));
	assert(!heldNote.Handle(Keys::Up, Keys::Repeat, true, true, true).handled);
	assert(!heldNote.Handle(Keys::Up, Keys::Release, true, true, true).handled);
	// Shift and active gameplay remain mandatory even when both arrows are down.
	Keys disabled;
	assert(!disabled.Handle(Keys::Up, Keys::Press, false, true, true).handled);
	assert(!disabled.Handle(Keys::Up, Keys::Press, true, false, true).handled);

	std::cout << "Gameplay rate and shortcut regression checks passed.\n";
}
