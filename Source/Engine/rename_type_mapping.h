#pragma once
#include <string>

namespace cumulonimbus::mapping::rename_type
{
	enum class Entity : uint64_t {};
	using EntityId		= uint64_t;	// Entity�̎��ʎq
	using EntityName	= std::string;
	using ComponentId	= uint64_t;	// Component�̎��ʎq
	using ComponentName = std::string;
} // cumulonimbus::mapping::rename_type
