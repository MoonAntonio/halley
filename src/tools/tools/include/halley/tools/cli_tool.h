#pragma once
#include <string>
#include <halley/data_structures/vector.h>
#include <halley/text/halleystring.h>
#include <memory>
#include <functional>
#include <map>
#include "halley/core/game/environment.h"

namespace Halley
{
	class HalleyStatics;

	class CommandLineTool
	{
	public:
		virtual ~CommandLineTool();

		virtual int runRaw(int argc, char** argv);
		virtual int run(Vector<std::string> args);

	protected:
		std::unique_ptr<HalleyStatics> statics;
		Environment env;
	};

	class CommandLineTools
	{
	public:
		CommandLineTools();

		Vector<std::string> getToolNames();
		std::unique_ptr<CommandLineTool> getTool(std::string name);

	private:
		using ToolFactory = std::function<std::unique_ptr<CommandLineTool>()>;
		std::map<std::string, ToolFactory> factories;
	};
}
