#pragma once
#include <halley/tools/codegen/icode_generator.h>
#include "halley/file/path.h"

namespace Halley
{
	class CodegenCPP : public ICodeGenerator
	{
	public:
		Path getDirectory() const override { return "cpp"; }
		CodegenLanguage getLanguage() const override { return CodegenLanguage::CPlusPlus; }

		CodeGenResult generateComponent(ComponentSchema component) override;
		CodeGenResult generateSystem(SystemSchema system) override;
		CodeGenResult generateMessage(MessageSchema message) override;

		CodeGenResult generateRegistry(const Vector<ComponentSchema>& components, const Vector<SystemSchema>& systems) override;

	private:
		Vector<String> generateComponentHeader(ComponentSchema component);
		Vector<String> generateSystemHeader(SystemSchema& system) const;
		Vector<String> generateSystemStub(SystemSchema& system) const;
		Vector<String> generateMessageHeader(MessageSchema message);
	
	};
}
