#pragma once

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

  Copyright (c) 2007-2014 - Rodrigo Braz Monteiro.
  This file is subject to the terms of halley_license.txt.

\*****************************************************************/

#include "halleytime.h"
#include <chrono>
#include <cstdint>

namespace Halley {
	class Stopwatch {
	public:
		Stopwatch(bool start = true);

		void start();
		void pause();
		void reset();

		Time elapsedSeconds() const;
		int64_t elapsedMicroSeconds() const;
		int64_t elapsedNanoSeconds() const;

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
		int64_t measuredTime = 0;
		bool running = false;
	};

	class StopwatchAveraging
	{
	public:
		enum class Mode
		{
			Average,
			Latest
		};

		explicit StopwatchAveraging(int nSamples = 30);
		void beginSample();
		void endSample();

		int64_t elapsedNanoSeconds(Mode mode) const;
		int64_t averageElapsedNanoSeconds() const;
		int64_t lastElapsedNanoSeconds() const;

	private:
		int nSamples;
		int nsTakenAvgSamples = 0;

		std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

		int64_t nsTaken = 0;
		int64_t nsTakenAvg = 0;
		int64_t nsTakenAvgAccum = 0;
	};
}