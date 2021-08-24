#pragma once
#include <string>

namespace cumulonimbus::mapping::rename_type
{
	enum class Entity : uint64_t {};
	using EntityId		= uint64_t;	// Entity‚ÌŽ¯•ÊŽq
	using EntityName	= std::string;
	using ComponentId	= uint64_t;	// Component‚ÌŽ¯•ÊŽq
	using ComponentName = std::string;
} // cumulonimbus::mapping::rename_type
