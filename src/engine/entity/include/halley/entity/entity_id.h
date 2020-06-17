#pragma once

#include <cstdint>
#include "halley/text/halleystring.h"
#include "halley/text/string_converter.h"

namespace Halley {
	struct alignas(8) EntityId {
		int64_t value;

		EntityId() : value(-1) {}
		
		explicit EntityId(const String& str)
		{
			value = str.toInteger64();
		}
		
		bool isValid() const { return value != -1; }
		bool operator==(const EntityId& other) const { return value == other.value; }
		bool operator!=(const EntityId& other) const { return value != other.value; }
		bool operator<(const EntityId& other) const { return value < other.value; }
		bool operator>(const EntityId& other) const { return value > other.value; }
		bool operator<=(const EntityId& other) const { return value <= other.value; }
		bool operator>=(const EntityId& other) const { return value >= other.value; }

		String toString() const
		{
			return Halley::toString(value);
		}
	};
}


namespace std {
	template<>
	struct hash<Halley::EntityId>
	{
		size_t operator()(const Halley::EntityId& v) const 
		{
			return std::hash<int64_t>()(v.value);
		}
	};
}

