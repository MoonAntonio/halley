#pragma once
#include <memory>
#include "halley/tools/dll/dynamic_library.h"
#include "symbol_loader.h"
#include <halley/core/game/main_loop.h>
#include "halley/core/entry/game_loader.h"

namespace Halley
{
	class IHalleyEntryPoint;
	class Game;

	class DynamicGameLoader : public GameLoader
	{
	public:
		DynamicGameLoader(std::string dllName);
		~DynamicGameLoader();

		std::unique_ptr<Core> createCore(Vector<std::string> args) override;
		std::unique_ptr<Game> createGame() override;
		bool needsToReload() const override;
		void reload() override;
		void setCore(IMainLoopable& core) override;

	private:
		std::string libName;
		DynamicLibrary lib;
		IHalleyEntryPoint* entry = nullptr;
		IMainLoopable* core = nullptr;

		Vector<DebugSymbol> symbols;
		Vector<DebugSymbol> prevSymbols;
		
		void load();
		void unload();
		void hotPatch();
	};
}
