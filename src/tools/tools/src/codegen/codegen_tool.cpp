#include "halley/tools/codegen/codegen_tool.h"
#include "halley/tools/codegen/codegen.h"
#include <iostream>

using namespace Halley;

int CodegenTool::run(Vector<std::string> args)
{
	if (args.size() == 2) {
		Codegen::run(args[0], args[1]);
		return 0;
	} else {
		std::cout << "Usage: halley-cmd codegen srcDir dstDir" << std::endl;
		return 1;
	}
}
