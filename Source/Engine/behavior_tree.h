#pragma once

#include <functional>
#include <map>

namespace cumulonimbus::system
{
	template<class Key,class ReturnVal = void,class... Args>
	class BehaviorTree final
	{
	public:
		/**
		 * @brief : �r�w�C�r�A�c���[�̐V�K�쐬
		 */
		void CreateBehavior(const Key behavior_type)
		{
			if (behaviors.contains(behavior_type))
				return;

			behaviors.emplace(behavior_type);
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

		void Update(Args... args)
		{
			if (!behaviors.contains(current_behavior))
				//return;
				assert(!"Not found behavior");

			behaviors.at(current_behavior).at(current_behavior_index)(args...);
		}

		void NextSequence()
		{

		}

	private:
		Key current_behavior{};
		Key old_behavior{};

		u_int current_behavior_index{ 0 };

		using Sequence = std::vector<std::function<ReturnVal(Args...)>>;
		std::map<Key, Sequence> behaviors{};
	};
} // cumulonimbus::system
