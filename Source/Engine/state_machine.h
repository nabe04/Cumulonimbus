#pragma once

#include <assert.h>
#include <functional>
#include <map>

// brief : State Machine
// Key	 : State Key
template<typename Key, typename ReturnVal = void ,typename... Args>
class StateMachine
{
private:
	Key current_state{ static_cast<int>(-1) };
	Key old_state{ static_cast<int>(-1)};		// State‚Ì‰Šú‰»‚È‚Ç‚Ég‚¤
	//std::function<ReturnVal(Args...)> call_back;

	std::map < Key, std::function<ReturnVal(Args...)>> states;

	bool is_switched = false;

public:
	void AddState(Key state, std::function<ReturnVal(Args...)> callback)
	{
		states.insert(std::make_pair(state, callback));
	}

	void Update(Args... args)
	{
		if (!states.contains(current_state))
			//return;
			assert(!"Not found state");

		states.at(current_state)(args...);

		//old_state = current_state;

		//for (auto& [state, func] : states)
		//{
		//	func(args...);
		//}
	}

	Key GetState() { return current_state; }
	Key GetOldState() { return old_state; }
	bool GetInitialize()
	{
		if (!is_switched) return false;

		is_switched = false;
		return true;
	}
	void SetState(Key state)
	{
		old_state = current_state;
		current_state = state;
		is_switched = true;
	}
};