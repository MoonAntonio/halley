#include "taskbar.h"
#include "halley/tools/tasks/editor_task_set.h"

using namespace Halley;

TaskBar::TaskBar(UIFactory& ui, EditorTaskSet& taskSet)
	: UIWidget("taskBar", Vector2f(150.0f, 80.0), UISizer())
	, resources(ui.getResources())
	, taskSet(taskSet)
{
	{
		auto col = Colour4f(0.9882f, 0.15686f, 0.27843f, 1);
		halleyLogo = Sprite()
			.setImage(resources, "halley/halley_logo_dist.png", "Halley/DistanceFieldSprite")
			.setPivot(Vector2f(0.5f, 0.5f))
			.setScale(Vector2f(0.5f, 0.5f))
			.setColour(col);
		halleyLogo.getMaterial()
			.set("u_smoothness", 16.0f)
			.set("u_outline", 0.0f)
			.set("u_outlineColour", col);
	}

	{
		Colour4f col(0.12f, 0.12f, 0.12f);
		barSolid = Sprite().setMaterial(resources, "Halley/SolidColour").setSize(Vector2f(1, 1)).setColour(col);
		barFade = Sprite().setImage(resources, "fade_right.png").setColour(col);
	}

	font = resources.get<Font>("Ubuntu Bold");
}

void TaskBar::update(Time time, bool moved)
{
	auto taskData = taskSet.getTasks();

	// Ensure it has a display associated
	for (auto& t : taskData) {
		if (t->isVisible() && t->getStatus() == EditorTaskStatus::Started) {
			getDisplayFor(t);
		}
	}

	// Update and decay
	for (size_t i = 0; i < tasks.size();) {
		auto& t = tasks[i];

		float targetDisplaySlot = float(i);
		if (t.displaySlot < 0) {
			t.displaySlot = targetDisplaySlot;
		} else {
			t.displaySlot = lerp(t.displaySlot, targetDisplaySlot, static_cast<float>(6 * time));
		}

		if (t.task->getStatus() == EditorTaskStatus::Done) {
			t.progressDisplay = 1;
			t.completeTime += static_cast<float>(time);
			if (t.completeTime >= 1.5f && !t.task->hasError()) {
				tasks.erase(tasks.begin() + i);
				continue;
			}
		} else {
			t.progressDisplay = lerp(t.progressDisplay, t.task->getProgress(), static_cast<float>(10 * time));
		}
		++i;
	}

	// Update bar size
	displaySize = lerp(displaySize, float(tasks.size()), static_cast<float>(6 * time));
}

void TaskBar::draw(UIPainter& painter) const
{
	// Setup for tasks
	const Vector2f anchor = getPosition() + Vector2f(0, 80.0f);
	const Vector2f baseDrawPos = getPosition() + Vector2f(150.0f, 8.0f);
	const Vector2f size = Vector2f(std::min(400.0f, getSize().x / std::max(1.0f, displaySize)), 40);
	float totalLen = baseDrawPos.x + (displaySize * size.x) + 10.0f;

	// Draw logo
	painter.draw(barSolid.setScale(Vector2f(totalLen, 32)).setPos(anchor + Vector2f(0, -56)), true);
	painter.draw(barFade.setPos(anchor + Vector2f(totalLen, -56)), true);
	painter.draw(halleyLogo.setPos(anchor + Vector2f(80, -41)), true);

	if (tasks.empty()) {
		// Nothing to do here!
		return;
	}

	// Setup text renderer
	TextRenderer text;
	text.setFont(font).setOffset(Vector2f(0, 0)).setColour(Colour(1, 1, 1)).setOutline(2.0f).setOutlineColour(Colour(0, 0, 0, 0.35f));

	// Draw tasks
	for (auto& t : tasks) {
		Vector2f drawPos = baseDrawPos + Vector2f((size.x + 20) * t.displaySlot, 0);

		Colour col = t.task->hasError() ? Colour(0.93f, 0.2f, 0.2f) : (t.task->getProgress() > 0.9999f ? Colour(0.16f, 0.69f, 0.34f) : Colour(0.18f, 0.53f, 0.87f));

		auto sprite = Sprite()
			.setImage(resources, "round_rect.png", "Halley/DistanceFieldSprite")
			.setPos(drawPos + Vector2f(6.0f, 6.0f))
			.scaleTo(size + Vector2f(12, 12))
			.setPivot(Vector2f(0, 0));

		sprite.getMaterial()
			.set("u_smoothness", 6.0f)
			.set("u_outline", 0.4f)
			.set("u_outlineColour", col);

		// Background
		painter.draw(sprite.setColour(Colour4f(0.15f, 0.15f, 0.19f)), true);

		// Progress
		painter.draw(sprite
			.setClip(Rect4f(Vector2f(), (size.x + 10) * t.progressDisplay, size.y + 10))
			.scaleTo(size + Vector2f(10, 10))
			.setColour(col), true);

		// Text
		painter.draw(text.setSize(14).setText(t.task->getName()).setPosition(drawPos + Vector2f(24, 12)), true);
		painter.draw(text.setSize(17).setText(t.task->getProgressLabel()).setPosition(drawPos + Vector2f(24, 30)), true);
	}
}

TaskBar::TaskDisplay& TaskBar::getDisplayFor(const std::shared_ptr<EditorTaskAnchor>& task)
{
	for (auto& t : tasks) {
		// Already assigned one!
		if (t.task == task) {
			return t;
		}

		// Replace error:
		if (t.task->hasError() && t.task->getName() == task->getName()) {
			t.task = task;
			return t;
		}
	}

	tasks.push_back(TaskDisplay());
	TaskDisplay& display = tasks.back();

	display.task = task;
	return display;
}
