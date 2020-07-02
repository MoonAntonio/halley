#ifdef WINDOWS_STORE

#include "winrt_input.h"
#include "winrt_gamepad.h"
#include "winrt_keyboard.h"
#include "winrt_mouse.h"

using namespace Halley;

void WinRTInput::init()
{
	for (int i = 0; i < 4; ++i) {
		gamepads.push_back(std::make_shared<WinRTGamepad>(i));
	}
	keyboard = std::make_shared<WinRTKeyboard>();
	mouse = std::make_shared<WinRTMouse>();
}

void WinRTInput::deInit()
{
	
}

void WinRTInput::beginEvents(Time t)
{
	for (auto& g: gamepads) {
		g->clearPresses();
		g->update(t);
	}
	keyboard->update();
	mouse->update();
}

size_t WinRTInput::getNumberOfKeyboards() const
{
	return 1;
}

std::shared_ptr<InputKeyboard> WinRTInput::getKeyboard(int id) const
{
	return keyboard;
}

size_t WinRTInput::getNumberOfJoysticks() const
{
	return 4;
}

std::shared_ptr<InputJoystick> WinRTInput::getJoystick(int id) const
{
	return gamepads.at(id);
}

size_t WinRTInput::getNumberOfMice() const
{
	return 1;
}

std::shared_ptr<InputDevice> WinRTInput::getMouse(int id) const
{
	return mouse;
}

void WinRTInput::setMouseRemapping(std::function<Vector2f(Vector2i)> remapFunction)
{
	mouse->setRemap(remapFunction);
}

Vector<std::shared_ptr<InputTouch>> WinRTInput::getNewTouchEvents()
{
	return {};
}

Vector<std::shared_ptr<InputTouch>> WinRTInput::getTouchEvents()
{
	return {};
}

#endif
