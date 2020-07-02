#include "widgets/ui_button.h"
#include "ui_style.h"
#include "ui_painter.h"
#include "widgets/ui_label.h"

using namespace Halley;

UIButton::UIButton(String id, UIStyle s, std::optional<UISizer> sizer)
	: UIClickable(std::move(id), s.getSprite("normal").getScaledSize(), std::move(sizer), s.getBorder("innerBorder"))
	, style(s)
{
	sprite = style.getSprite("normal");
	setMinSize(sprite.getOriginalSize());
}

UIButton::UIButton(String id, UIStyle style, LocalisedString label)
	: UIButton(std::move(id), std::move(style), UISizer())
{
	setLabel(std::move(label));
}

void UIButton::draw(UIPainter& painter) const
{
	if (sprite.hasMaterial()) {
		painter.draw(sprite);
	}
}

void UIButton::update(Time t, bool moved)
{
	const bool dirty = updateButton() | moved;

	if (dirty) {
		const auto b = sprite.getOuterBorder();
		const auto topLeftBorder = Vector2f(float(b.x), float(b.y));
		const auto bottomRightBorder = Vector2f(float(b.z), float(b.w));

		Vector2f basePos = getPosition();
		Vector2f imgBaseSize = sprite.getRawSize().abs() + topLeftBorder + bottomRightBorder;
		if (sprite.getClip()) {
			auto c = sprite.getClip().value();
			basePos -= c.getTopLeft();
			imgBaseSize = std::min(c.getSize(), imgBaseSize);
		}
		sprite
			.setPos(basePos)
			.setScale(getSize() / imgBaseSize);
	}
}

void UIButton::onClicked(Vector2f)
{
	sendEvent(UIEvent(UIEventType::ButtonClicked, getId()));
}

void UIButton::setInputType(UIInputType uiInput)
{
	lastInputType = uiInput;

	if (canDoBorderOnly) {
		if (uiInput != curInputType) {
			curInputType = uiInput;
			borderOnly = !getOnlyEnabledWithInput().empty() && curInputType != UIInputType::Mouse && curInputType != UIInputType::Keyboard;
			doForceUpdate();
			setState(State::Up);
			doSetState(State::Up);
		}		
	} else {
		UIWidget::setInputType(uiInput);
	}
}

bool UIButton::canInteractWithMouse() const
{
	return borderOnly ? false : UIClickable::canInteractWithMouse();
}

bool UIButton::isFocusLocked() const
{
	return borderOnly ? false : UIClickable::isFocusLocked();
}

void UIButton::onManualControlActivate()
{
	onClicked(getPosition());
}

void UIButton::setCanDoBorderOnly(bool canDo)
{
	canDoBorderOnly = canDo;
}

void UIButton::setLabel(LocalisedString label)
{
	const auto& renderer = style.getTextRenderer("label");
	auto uiLabel = std::make_shared<UILabel>(getId() + "_label", renderer, std::move(label));
	if (style.hasTextRenderer("hoveredLabel")) {
		uiLabel->setHoverable(style.getTextRenderer("label"), style.getTextRenderer("hoveredLabel"));
	}
	if (style.hasTextRenderer("selectedLabel"))	{
		uiLabel->setSelectable(style.getTextRenderer("label"), style.getTextRenderer("selectedLabel"));
	}
	add(uiLabel, 1, style.getBorder("labelBorder"), UISizerAlignFlags::Centre);
}

void UIButton::doSetState(State state)
{
	if (borderOnly) {
		sprite = style.getSprite("borderOnly");
	} else {
		if (!isEnabled()) {
			sprite = style.getSprite("disabled");
		} else if (state == State::Up) {
			sprite = style.getSprite("normal");
			playSound(style.getString("upSound"));
			sendEventDown(UIEvent(UIEventType::SetSelected, getId(), false));
			sendEventDown(UIEvent(UIEventType::SetHovered, getId(), false));
		} else if (state == State::Down) {
			sprite = style.getSprite("down");
			playSound(style.getString("downSound"));
			sendEventDown(UIEvent(UIEventType::SetSelected, getId(), true));
		} else if (state == State::Hover) {
			sprite = style.getSprite("hover");
			sendEventDown(UIEvent(UIEventType::SetHovered, getId(), true));
			playSound(style.getString("hoverSound"));
		}
	}
}

void UIButton::onStateChanged(State prev, State next)
{
	if (isEnabled() && !borderOnly && prev != next) {
		if (next == State::Up) {
			playSound(style.getString("upSound"));
		} else if (next == State::Down) {
			playSound(style.getString("downSound"));
		} else if (next == State::Hover) {
			playSound(style.getString("hoverSound"));
		}
	}
}

void UIButton::onShortcutPressed()
{
	playSound(style.getString("shortcutPressedSound"));
}
