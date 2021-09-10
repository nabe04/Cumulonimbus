#pragma once
#include <string>

namespace cumulonimbus::mapping::rename_type
{
	enum class Entity : uint64_t {};
	using EntityId		= uint64_t;	// Entity�̎��ʎq
	using EntityName	= std::string;
	using ComponentId	= uint64_t;	// Component�̎��ʎq
	using ComponentName = std::string;
	using UUID			= std::string;
	using Hash			= size_t;
} // cumulonimbus::mapping::rename_type
