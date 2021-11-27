#pragma once
#include <map>
#include <ranges>

#include "rename_type_mapping.h"

namespace cumulonimbus::system
{
	template<class ReturnVal = void,class... Args>
	class Event final
	{
	public:
		Event()							= default;
 		~Event()						= default;

		void RegistryEvent(const mapping::rename_type::UUID& id, const std::function<ReturnVal(Args...)>& callback)
		{
			if (events.contains(id))
			{
				events.at(id) = callback;
				return;
			}
			events.emplace(id, callback);
		}

		void Invoke(const mapping::rename_type::UUID& id,Args... args)
		{
			if (!events.contains(id))
				return;

			events.at(id)(args...);
		}

		void InvokeAll(Args... args)
		{
			for(auto& eve : events | std::views::values)
			{
				eve(args...);
			}
		}

	private:
		std::map<mapping::rename_type::UUID, std::function<ReturnVal(Args...)>> events{};
	};
} // cumulonimbus::system