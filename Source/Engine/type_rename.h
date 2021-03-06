#pragma once
#include <cstdint>
#include <string>

namespace cumulonimbus::mapping::type_rename
{
	enum class Entity : uint64_t {};
	using EntityId		= uint64_t;	   // Entityの識別子
	using ComponentId	= uint64_t;	   // Componentの識別子
	using ComponentName = std::string; // Component名
}
