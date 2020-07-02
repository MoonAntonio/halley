#include "behaviours/ui_transition_slide_behaviour.h"
#include "ui_widget.h"
#include "halley/support/logger.h"
using namespace Halley;

UITransitionSlideBehaviour::UITransitionSlideBehaviour(Time length, UIAnchor base, UIAnchor startPos, UIAnchor endPos, TransitionCurve curve)
	: length(length)
	, time(0.0)
	, mode(Mode::FadeIn)
	, curve(std::move(curve))
	, base(std::move(base))
	, startPos(std::move(startPos))
	, endPos(std::move(endPos))
	, finished(false)
{
}

void UITransitionSlideBehaviour::updateAnchors(UIAnchor newBase, UIAnchor newStartPos, UIAnchor newEndPos) {
	base = newBase;
	startPos = newStartPos;
	endPos = newEndPos;

	const float x = clamp(mode == Mode::FadeIn ? float(time / length) : 1.0f - float(time / length), 0.0f, 1.0f);
	const float t = curve(x);

	if (mode == Mode::FadeIn) {
		getWidget()->setAnchor(lerp(isReversed() ? endPos : startPos, base, t));
	}
	else if (mode == Mode::FadeOut) {
		getWidget()->setAnchor(lerp(base, isReversed() ? startPos : endPos, 1.0f - t));
	}
	else {
		getWidget()->setAnchor(base);
	}
}

bool UITransitionSlideBehaviour::isFinished() const {
	return finished;
}

void UITransitionSlideBehaviour::init()
{
	getWidget()->setEnabled(false);
	getWidget()->setAnchor(startPos);
}

void UITransitionSlideBehaviour::deInit()
{
}

void UITransitionSlideBehaviour::update(Time dt)
{
	if (mode != Mode::Normal) {
		time += dt;
		
		const float x = clamp(mode == Mode::FadeIn ? float(time / length) : 1.0f - float(time / length), 0.0f, 1.0f);
		const float t = curve(x);

		if (mode == Mode::FadeIn) {
			if (time >= length) {
				mode = Mode::Normal;
				getWidget()->setEnabled(true);
			}
			getWidget()->setAnchor(lerp(isReversed() ? endPos : startPos, base, t));
		} else if (mode == Mode::FadeOut) {
			getWidget()->setAnchor(lerp(base, isReversed() ? startPos : endPos, 1.0f - t));
			if (time >= length) {
				getWidget()->forceDestroy();
				finished = true;
			}
		}
	}
}

bool UITransitionSlideBehaviour::isAlive() const
{
	return true;
}

bool UITransitionSlideBehaviour::onParentDestroyed()
{
	if (mode == Mode::FadeOut) {
		return time >= length;
	} else {
		mode = Mode::FadeOut;
		time = 0;
		getWidget()->setEnabled(false);
		return false;
	}
}
