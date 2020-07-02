/*****************************************************************\
           __
          / /
		 / /                     __  __
		/ /______    _______    / / / / ________   __       __
	   / ______  \  /_____  \  / / / / / _____  | / /      / /
	  / /      | / _______| / / / / / / /____/ / / /      / /
	 / /      / / / _____  / / / / / / _______/ / /      / /
	/ /      / / / /____/ / / / / / / |______  / |______/ /
   /_/      /_/ |________/ / / / /  \_______/  \_______  /
                          /_/ /_/                     / /
			                                         / /
		       High Level Game Framework            /_/

  ---------------------------------------------------------------

  Copyright (c) 2007-2011 - Rodrigo Braz Monteiro.
  This file is subject to the terms of halley_license.txt.

\*****************************************************************/

#pragma once

#include <halley/maths/vector2.h>
#include <halley/time/halleytime.h>

namespace Halley {
	class InputTouch {
	public:
		InputTouch(Vector2f initialPos);

		bool isPressed() const;
		bool isReleased() const;
		Vector2f getInitialPos() const;
		Vector2f getCurrentPos() const;
		Time getTimeElapsed() const;

		void setPos(Vector2f pos);
		void update(Time t);
		void setReleased();

	private:
		Vector2f initialPos;
		Vector2f curPos;
		bool pressed;
		bool released;
		Time elapsed;
	};

	typedef std::shared_ptr<InputTouch> spInputTouch;
}
