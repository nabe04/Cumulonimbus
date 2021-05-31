#pragma once
#include <string>
#include <cassert>
#include <map>

#include <nameof.h>

#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>


template <typename T>
class EnumStateMap
{
public:
	EnumStateMap()
	{
		const int size = static_cast<int>(T::End);

		for (int i = 0; i < size; ++i)
		{
			auto flag = static_cast<T>(i);
			state_map.insert(
				std::make_pair(
					nameof::nameof_enum(flag).data(),
					static_cast<T>(i)));
		}

		auto flag	  = static_cast<T>(0);
		current_state = state_map.at(nameof::nameof_enum(flag).data());
	}
	EnumStateMap(const T& state)
		:EnumStateMap()
	{
		current_state = state;
	}

	[[nodiscard]] unsigned int NumState() const { return state_map.size(); }

	[[nodiscard]] const T& GetCurrentState() const { return current_state; }
	[[nodiscard]] std::string GetCurrentStateName() { return nameof::nameof_enum(current_state).data(); }
	void SetState(const T& state)
	{
		if (!state_map.contains(nameof::nameof_enum(state).data()))
		{
			assert(!"Don't have state");
		}

		current_state = state;
	}

	template <typename Archive>
	void serialize(Archive&& archive)
	{
		archive(
			cereal::make_nvp("State Map"	, state_map),
			cereal::make_nvp("Current State", current_state)
		);
	}

	//void operator{} (const T& state)
	//{
	//	if (!state_map.contains(nameof::nameof_enum(state).data()))
	//	{
	//		assert(!"Don't have state");
	//	}

	//	current_state = state;
	//}

private:
	std::map<std::string, T> state_map{};

	T current_state{};
};