#pragma once
#include <string>
#include <cassert>
#include <map>

#include <nameof.h>

#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>

template <typename T>
concept StateEnum = requires ()
{
	std::is_enum_v<T>;
	T::End;
};

template <StateEnum T>
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

			state_names.emplace_back(nameof::nameof_enum(flag).data());
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
	[[nodiscard]] const std::map<std::string, T>& GetStateMap() const { return state_map; }
	[[nodiscard]] const std::vector<std::string>&  GetStateNames() const { return state_names; }

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
			CEREAL_NVP(state_map),
			CEREAL_NVP(state_names),
			CEREAL_NVP(current_state)
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
	std::vector<std::string> state_names{};

	T current_state{};
};