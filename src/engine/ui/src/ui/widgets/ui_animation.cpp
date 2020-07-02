#include "widgets/ui_animation.h"
using namespace Halley;

UIAnimation::UIAnimation(const String& id, Vector2f size, Vector2f animationOffset, AnimationPlayer animation)
	: UIWidget(id, size)
	, offset(animationOffset)
	, animation(animation)
{}

AnimationPlayer& UIAnimation::getPlayer()
{
	return animation;
}

const AnimationPlayer& UIAnimation::getPlayer() const
{
	return animation;
}

Sprite& UIAnimation::getSprite()
{
	return sprite;
}

const Sprite& UIAnimation::getSprite() const
{
	return sprite;
}

Vector2f UIAnimation::getOffset() const
{
	return offset;
}

void UIAnimation::setOffset(Vector2f o)
{
	offset = o;
}

void UIAnimation::update(Time t, bool moved)
{
	if (animation.hasAnimation()) {
		animation.update(t);
		animation.updateSprite(sprite);
		sprite.setPos(getPosition() + offset);
	}
}

void UIAnimation::draw(UIPainter& painter) const
{
	if (animation.hasAnimation()) {
		painter.draw(sprite);
	}
}
