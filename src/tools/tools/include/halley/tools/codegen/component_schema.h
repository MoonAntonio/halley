#pragma once

#include "fields_schema.h"
#include <unordered_set>

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
		explicit ComponentSchema(YAML::Node node);

		int id = -1;
		String name;
		Vector<VariableSchema> members;
		std::unordered_set<String> includeFiles;
	};
}