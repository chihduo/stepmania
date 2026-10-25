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
	assert(rate.Change(1.0f, 1)); Equal(rate.GetRate(), 1.2f);
	assert(rate.Change(1.0f, -1)); Equal(rate.GetRate(), 1.0f);
	assert(rate.Change(1.0f, -1)); Equal(rate.GetRate(), 0.8f);
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
	assert(rate.Change(1.25f, 1)); Equal(rate.GetRate(), 1.45f);
	assert(rate.Change(1.25f, -1)); Equal(rate.GetRate(), 1.25f);
	Equal(rate.GetScrollSpeedRate(1.45f), 1.25f);
	rate.Reset();
	Equal(rate.GetScrollSpeedRate(0.8f), 0.8f);
	assert(rate.Change(0.8f, 1)); Equal(rate.GetRate(), 1.0f);
	Equal(rate.GetScrollSpeedRate(1.0f), 0.8f);

	// CMod 300 at a starting 1.25x: a note one song-second away has unchanged
	// spacing at the change boundary, then travels 1.45/1.25 times as fast.
	rate.Reset();
	const float originalSpacing = 300.0f / 60.0f / rate.GetScrollSpeedRate(1.25f);
	assert(rate.Change(1.25f, 1));
	const float changedSpacing = 300.0f / 60.0f / rate.GetScrollSpeedRate(1.45f);
	Equal(changedSpacing, originalSpacing);
	Equal(changedSpacing * rate.GetRate(), 5.8f);
	// MMod uses the same denominator; XMod follows song time directly.
	Equal(300.0f / (150.0f * rate.GetScrollSpeedRate(1.45f)), 1.6f);
	rate.Reset();
	Equal(rate.GetScrollSpeedRate(1.25f), 1.25f);

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
		assert(press.direction == (arrow == Keys::Up || arrow == Keys::Right ? 1 : -1));
		assert(keys.IsCaptured(arrow));
		const auto repeat = keys.Handle(arrow, Keys::Repeat, true, false);
		assert(repeat.handled && repeat.direction == 0);
		// Shift can be released first, or gameplay can end while the key is down.
		const auto shifted = keys.Handle(arrow, Keys::Repeat, false, false);
		assert(shifted.handled && shifted.direction == 0);
		assert(keys.IsCaptured(arrow));
		const auto release = keys.Handle(arrow, Keys::Release, false, false);
		assert(release.handled && release.direction == 0);
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

	std::cout << "Gameplay rate and shortcut regression checks passed.\n";
}
