#pragma once

#include "halley/tools/tasks/editor_task.h"
#include "prec.h"

namespace Halley
{
	class EditorTaskSet;

	class TaskBar : public UIWidget
	{
		class TaskDisplay
		{
		public:
			std::shared_ptr<EditorTaskAnchor> task;

			float progressDisplay = 0;
			float completeTime = 0;
			float displaySlot = -1;
		};

	public:
		TaskBar(UIFactory& factory, EditorTaskSet& taskSet);
		void update(Time time, bool moved) override;
		void draw(UIPainter& painter) const override;

	private:
		Resources& resources;
		EditorTaskSet& taskSet;
		std::vector<TaskDisplay> tasks;

		mutable Sprite barSolid;
		mutable	Sprite barFade;
		mutable Sprite halleyLogo;

		std::shared_ptr<const Font> font;

		float displaySize = 0;

		TaskDisplay& getDisplayFor(const std::shared_ptr<EditorTaskAnchor>& task);
	};
}
