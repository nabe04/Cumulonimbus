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
			OnKeyRangeEnter,	// キーイベントに入った時
			OnKeyRangeExit,		// キーイベントから抜けた時
			OnKeyRangeStay,		// キーイベントに入っている間
			None,				// キーイベントに入っていない間

			End
		};

		struct KeyEvent
		{
			KeyState key_state{};
			u_int min{};
			u_int max{};
			bool is_in_range{ false }; // min以上max以下の範囲に存在するか
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
		 * @brief : キーイベントの追加
		 * @param key_name : 登録するキー値
		 * @param key_event :
		 */
		void RegistryEvent(const std::string& key_name, const KeyEvent& key_event);

		/**
		 * @brief : キーイベントの追加
		 * @param key_name : 登録する時のキー値
		 * @param min : イベントの最小フレーム
		 * @param max : イベントの最大フレーム
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
		// キーフレームイベント
		std::map<std::string, KeyEvent> key_events{};
	};

} // cumulonimbus::system
