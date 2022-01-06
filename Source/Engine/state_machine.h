#pragma once

#include <cassert>
#include <functional>
#include <map>

#include <cereal/cereal.hpp>

// brief : State Machine
// Key	 : State Key
template<class Key, class ReturnVal = void , class... Args>
class StateMachine
{
public:
	StateMachine()	= default;
	~StateMachine() = default;

	/**
	 * @brief : StateMachine�œ������֐��̒ǉ�
	 */
	void AddState(Key state, std::function<ReturnVal(Args...)> callback)
	{
		if (states.contains(state))
			return;

		states.insert(std::make_pair(state, callback));
	}

	/**
	 * @brief : StateMachine�ɓo�^����Ă����Ԃ̍폜
	 */
	void ClearState()
	{
		states.clear();
		current_state	= {};
		old_state		= {};
		is_switched		= false;
	}

	/**
	 * @breif : ���݂̏�Ԃ̊֐����s
	 */
	void Update(Args... args)
	{
		if (!states.contains(current_state))
			//return;
			assert(!"Not found state");

		states.at(current_state)(args...);
	}

	[[nodiscard]]
	size_t GetNumState() const { return states.size(); }
	[[nodiscard]]
	Key GetState()		{ return current_state; }
	[[nodiscard]]
	Key GetOldState()	{ return old_state; }

	/**
	 * @brief : State���؂�ւ�������̏��߂̃t���[���̂ݓ��鏈��
	 */
	[[nodiscard]]
	bool GetInitialize()
	{
		if (!is_switched)
			return false;

		is_switched = false;
		return true;
	}
	void SetState(Key state)
	{
		old_state = current_state;
		current_state = state;
		is_switched = true;
	}

	template<class Archive>
	void load(Archive&& archive)
	{
		archive(
			cereal::make_nvp("current state", current_state),
			cereal::make_nvp("old state", old_state),
			//cereal::make_nvp("states"		, states),
			cereal::make_nvp("is switched", is_switched)
		);
	}

	template<class Archive>
	void save(Archive&& archive) const
	{
		archive(
			cereal::make_nvp("current state", current_state),
			cereal::make_nvp("old state", old_state),
			//cereal::make_nvp("states"		, states),
			cereal::make_nvp("is switched", is_switched)
		);
	}
private:
	Key current_state{ static_cast<int>(-1) };
	Key old_state{ static_cast<int>(-1) };		// State�̏������ȂǂɎg��
	//std::function<ReturnVal(Args...)> call_back;

	std::map<Key, std::function<ReturnVal(Args...)>> states{};

	bool is_switched{ false };

};