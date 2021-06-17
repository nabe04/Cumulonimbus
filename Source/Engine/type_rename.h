#pragma once
#include <cstdint>
#include <string>

namespace cumulonimbus::mapping::type_rename
{
	enum class Entity : uint64_t {};
	using EntityId		= uint64_t;	   // Entity�̎��ʎq
	using ComponentId	= uint64_t;	   // Component�̎��ʎq
	using ComponentName = std::string; // Component��
}
