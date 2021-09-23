#pragma once
#include <string>

namespace cumulonimbus::mapping::rename_type
{
	using EntityId		= uint64_t;	// Entity�̎��ʎq
	using EntityName	= std::string;
	using ComponentId	= uint64_t;	// Component�̎��ʎq
	using ComponentName = std::string;
	using UUID			= std::string;
	using Hash			= size_t;
	using Entity        = UUID;
} // cumulonimbus::mapping::rename_type
