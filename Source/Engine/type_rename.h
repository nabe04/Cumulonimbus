#pragma once
#include <cstdint>
#include <string>

namespace cumulonimbus::mapping::type_rename
{
	enum class Entity : uint64_t {};
	using EntityId		= uint64_t;	   // Entity‚ÌŽ¯•ÊŽq
	using ComponentId	= uint64_t;	   // Component‚ÌŽ¯•ÊŽq
	using ComponentName = std::string; // Component–¼
}
