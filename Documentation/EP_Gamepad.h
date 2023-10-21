#pragma once

#include "EP_Numerics.h"
#include "EP_Platform.h"

#include EP_PLATFORM_HEADER(EP_Gamepad)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct EP_GamepadButton
{
	uint8 pressed : 1;
	uint8 pressedFresh : 1;
	uint8 releasedFresh : 1;
	uint8 rem : 5;
};

struct EP_GamepadStick
{
	float32 minBound = -1.f;		// minimum value this can reach (used in normalization)
	float32 maxBound = 1.f;			// maximum value this can reach	(used in normalization)

	float32 valRaw = 0.f;			// the raw readable value 
	float32 valNormalized = 0.f;	// the normalized readable value
	float32 valNoDead = 0.f;		// the raw value ignoring dead zoning (equal to valRaw if > deadZone)

	float32 deadZonePos = 0.1f;		// dead zone in the negative direction
	float32 deadZoneNeg = -0.1f;	// dead zone in the positive direction
};

struct EP_GamepadTrigger
{
	float32 maxBound = 1.f;			// maximum value for normalization
	float32 valRaw = 0.f;			// the raw readable input
	float32 valNormalized = 0.f;	// the [0,1] normalized input
	float32 valNoDead = 0.f;		// the actual raw input
	float32 deadZone = 0.1f;		// the minimum value before writing to val raw
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class EP_GamepadButtons : uint32
{
	// Common
	A = 0,
	B = 1,
	X = 2,
	Y = 3,

	Start = 4,
	Select = 5,

	Middle = 6,

	BumpRight = 7,
	BumpLeft = 8,

	Left = 9,
	Up = 10,
	Right = 11,
	Down = 12,

	LStick = 13,
	RStick = 14,

	Count = 15
};

enum class EP_GamepadSticks {
	LeftX = 0,
	LeftY = 1,

	RightX = 2,
	RightY = 3,
	Count = 4
};

enum class EP_GamepadTriggers
{
	Left = 0,
	Right = 1,
	Count = 2
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EP_Gamepad
{
public:
	EP_Gamepad();

	void Refresh();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SetButtonState(EP_GamepadButtons, bool);
	void ButtonPressed(EP_GamepadButtons);
	void ButtonReleased(EP_GamepadButtons);
	
	bool GetButtonPressed(EP_GamepadButtons) const;
	bool GetButtonPressedFresh(EP_GamepadButtons) const;
	bool GetButtonReleasedFresh(EP_GamepadButtons) const;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SetStickBounds(EP_GamepadSticks, float32 lowBound, float32 highBound);
	void SetStickLowBound(EP_GamepadSticks, float32 lowBound);
	void SetStickHighBound(EP_GamepadSticks, float32 highBound);
	
	void SetStickDeadZone(EP_GamepadSticks, float32 negDeadZone, float32 posDeadZone);
	void SetStickNegDeadZone(EP_GamepadSticks, float32 negDeadZone);
	void SetStickPosDeadZone(EP_GamepadSticks, float32 posDeadZone);

	void SetStickVal(EP_GamepadSticks, float32 val);

	float32 GetStickValRaw(EP_GamepadSticks) const;
	float32 GetStickValNorm(EP_GamepadSticks) const;
	float32 GetStickValNoDead(EP_GamepadSticks) const;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SetTriggerBound(EP_GamepadTriggers, float32 bound);
	void SetTriggerDeadZone(EP_GamepadTriggers, float32 deadZone);

	void SetTriggerVal(EP_GamepadTriggers, float32 val);

	float32 GetTriggerValRaw(EP_GamepadTriggers) const;
	float32 GetTriggerValNorm(EP_GamepadTriggers) const;
	float32 GetTriggerValNoDead(EP_GamepadTriggers) const;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SetConnectionStatus(bool);
	bool IsConnected() const;

private:
	EP_GamepadButton buttons[(uint32)EP_GamepadButtons::Count];
	EP_GamepadStick sticks[(uint32)EP_GamepadSticks::Count];
	EP_GamepadTrigger triggers[(uint32)EP_GamepadSticks::Count];

	bool connected = false;

	EP_Gamepad_Platform platform;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
