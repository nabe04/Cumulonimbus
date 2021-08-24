#pragma once
#include <string>

namespace cumulonimbus::mapping::rename_type
{
	enum class Entity : uint64_t {};
	using EntityId		= uint64_t;	// Entityの識別子
	using EntityName	= std::string;
	using ComponentId	= uint64_t;	// Componentの識別子
	using ComponentName = std::string;
} // cumulonimbus::mapping::rename_type
