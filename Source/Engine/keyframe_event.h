#pragma once
#include <string>

namespace cumulonimbus::ecs
{
	class Registry;
} // cumulonimbus::ecs

namespace cumulonimbus::system
{
	class KeyframeEvent final
	{
	public:
		enum class KeyState
		{
			OnKeyRangeEnter,	// �L�[�C�x���g�ɓ�������
			OnKeyRangeExit,		// �L�[�C�x���g���甲������
			OnKeyRangeStay,		// �L�[�C�x���g�ɓ����Ă����
			None,				// �L�[�C�x���g�ɓ����Ă��Ȃ���

			End
		};

		struct KeyEvent
		{
			KeyState key_state{};
			u_int min{};
			u_int max{};
			bool is_in_range{ false }; // min�ȏ�max�ȉ��͈̔͂ɑ��݂��邩
			bool is_old_in_range{ false };

			template<class Archive>
			void load(Archive&& archive, uint32_t version);

			template<class Archive>
			void save(Archive&& archive, uint32_t version) const;
		};

	public:
		explicit KeyframeEvent() = default;
		~KeyframeEvent() = default;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void Update(ecs::Registry* registry, const mapping::rename_type::Entity& ent, const std::string& key_name);

		/**
		 * @brief : �L�[�C�x���g�̒ǉ�
		 * @param key_name : �o�^����L�[�l
		 * @param key_event :
		 */
		void RegistryEvent(const std::string& key_name, const KeyEvent& key_event);

		/**
		 * @brief : �L�[�C�x���g�̒ǉ�
		 * @param key_name : �o�^���鎞�̃L�[�l
		 * @param min : �C�x���g�̍ŏ��t���[��
		 * @param max : �C�x���g�̍ő�t���[��
		 */
		void RegistryEvent(const std::string& key_name, u_int min = 0, u_int max = 1);

		[[nodiscard]]
		KeyEvent& GetKeyEvent(const std::string& key_name);

		void SetKeyEvent(const std::string& key_name,u_int min,u_int max);

		[[nodiscard]]
		bool HasKeyEvent(const std::string& key_name) const
		{
			if (key_events.contains(key_name))
				return true;

			return false;
		}

	private:
		// �L�[�t���[���C�x���g
		std::map<std::string, KeyEvent> key_events{};
	};

} // cumulonimbus::system
