#pragma once

#include "keys.h"

class ICallbacks
{
public:

	virtual void KeyboardCB(PCDEV_KEY PcdevKey, PCDEV_KEY_STATE PcdevKeyState = PCDEV_KEY_STATE_PRESS)
	{
	}

	virtual void PassiveMouseCB(int x, int y)
	{
	}

	virtual void RenderSceneCB()
	{
	}

	virtual void MouseCB(PCDEV_MOUSE Button, PCDEV_KEY_STATE State, int x, int y)
	{
	}
};
