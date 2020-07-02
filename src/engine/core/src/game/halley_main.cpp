#include "halley/core/game/halley_main.h"
#include "halley/core/game/core.h"
#include "halley/support/console.h"
#include "game/halley_main.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#endif

using namespace Halley;

int HalleyMain::runMain(GameLoader& loader, const Vector<std::string>& args)
{
	std::unique_ptr<Core> core;
	try {
		core = loader.createCore(args);
		loader.setCore(*core);
		core->getAPI().system->runGame([&]() {
			core->init();
			MainLoop loop(*core, loader);
			loop.run();
		});
		return core->getExitCode();
	} catch (std::exception& e) {
		if (core) {
			core->onTerminatedInError(e.what());
		} else {
			std::cout << "Exception initialising core: " + String(e.what()) << std::endl;
		}
		return 1;
	} catch (...) {
		if (core) {
			core->onTerminatedInError("");
		} else {
			std::cout << "Unknown exception initialising core." << std::endl;
		}
		return 1;
	}
}

Vector<std::string> HalleyMain::getWin32Args()
{
	Vector<std::string> args;
#if defined(_WIN32) && !defined(WINDOWS_STORE)
	auto cmd = GetCommandLineW();
	int argc;
	auto argv = CommandLineToArgvW(cmd, &argc);
	for (int i = 0; i < argc; i++) {
		args.push_back(String(reinterpret_cast<wchar_t**>(argv)[i]).cppStr());
	}
	LocalFree(argv);
#endif
	return args;
}

Vector<std::string> HalleyMain::getArgs(int argc, char* argv[])
{
	Vector<std::string> args;
	args.reserve(argc);
	for (int i = 0; i < argc; i++) {
		args.push_back(argv[i]);
	}
	return args;
}
