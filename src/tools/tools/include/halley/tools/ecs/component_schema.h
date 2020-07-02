#pragma once

#include "fields_schema.h"
#include <unordered_set>
#include "halley/data_structures/maybe.h"

namespace YAML
{
	class Node;
}

namespace Halley
{
	class ComponentSchema
	{
	public:
		ComponentSchema();
		explicit ComponentSchema(YAML::Node node, bool generate);

		int id = -1;
		String name;
		Vector<MemberSchema> members;
		std::unordered_set<String> includeFiles;
		std::optional<String> customImplementation;
		bool generate = false;
	};
}