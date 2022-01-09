#pragma once

#include <functional>
#include <map>

namespace cumulonimbus::system
{
	template<class Key,class ReturnVal = void,class... Args>
	class BehaviorTree final
	{
	public:
		using Sequence = std::vector<std::function<ReturnVal(Args...)>>;

		/**
		 * @brief : �r�w�C�r�A�c���[�̐V�K�쐬
		 */
		void CreateBehavior(const Key behavior_type)
		{
			if (behaviors.contains(behavior_type))
				return;

			behaviors.emplace(behavior_type, Sequence{});
		}

		/**
		 * @brief : �r�w�C�r�A�c���[���֐��̒ǉ�
		 */
		void AddSequence(const Key behavior_type, std::function<ReturnVal(Args...)> callback)
		{
			// �r�w�C�r�A�����݂��Ȃ��ꍇ�V�������
			if (!behaviors.contains(behavior_type))
				CreateBehavior(behavior_type);

			behaviors.at(behavior_type).emplace_back(callback);
		}

		void Clear(const Key behavior_type)
		{
			if (!behaviors.contains(behavior_type))
				return;

			behaviors.erase(behavior_type);
		}

		void ClearAll()
		{
			behaviors.clear();
		}

		void Update(Args... args)
		{
			// �r�w�C�r�A�̏������o�^����Ă��Ȃ��ꍇ�����𒆒f����
			if (!behaviors.contains(current_behavior))
				//return;
				assert(!"Not found behavior");

			// ���݂̃r�w�C�r�A���S�ďI�����Ă���̂ŏ������΂�
			if (is_completed)
				return;

			// �r�w�C�r�A���ύX���ꂽ�����̏��߂̃t���[���̏ꍇ
			if (is_switched)
			{
				// �����t���O��true�ɂ���
				is_start = true;
				is_switched = false;
			}

			// �r�w�C�r�A�c���[�X�V����
			behaviors.at(current_behavior).at(index)(args...);

			// �����t���O��True�̏ꍇFalse��
			if (is_start)
				is_start = false;
		}

		void NextSequence()
		{
			++index;
			is_switched = true;
			is_start	= true;

			if (index >= behaviors.at(current_behavior).size())
			{
				index = 0;
				is_completed = true;
			}
		}

		void SetBehavior(const Key behavior)
		{
			if(!behaviors.contains(behavior))
				assert(!"Not found behavior");

			old_behavior		= current_behavior;
			current_behavior	= behavior;
			index				= 0;
			is_switched			= true;
			is_start			= true;
			is_completed		= false;

		}

		[[nodiscard]]
		bool GetIsStart() const
		{
			return is_start;
		}

		[[nodiscard]]
		bool GetIsCompleted() const
		{
			return is_completed;
		}
	private:
		// ���݂̃r�w�C�r�A���
		Key current_behavior{};
		// �ߋ��̃r�w�C�r�A���
		Key old_behavior{};
		// �r�w�C�r�A�c���[�̗v�f�ԍ�
		u_int index{ 0 };
		// �r�w�C�r�A���ύX���ꂽ��
		bool is_switched{ false };
		// �r�w�C�r�A���ύX���ꂽ�ŏ��̃t���[����
		bool is_start{ false };
		// ���݂̃r�w�C�r�A�c���[�ɓo�^����Ă��鏈�����S�Ċ���������
		bool is_completed{ false };
		// �r�w�C�r�A�c���[�{��
		std::map<Key, Sequence> behaviors{};
	};
} // cumulonimbus::system
