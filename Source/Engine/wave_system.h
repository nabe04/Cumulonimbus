#pragma once
#include <vector>

#include "rename_type_mapping.h"
#include "state_machine.h"
#include "event.h"

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	} // ecs

	namespace scene
	{
		class SceneManager;
	} // scene
} // cumulonimbus

namespace cumulonimbus::system
{
	class WaveSystem final
	{
	private:
		enum class WaveState
		{
			Start,
			Update,
			Change,

			End
		};

	public:
		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void Initialize(scene::SceneManager& scene_manager);

		void Start(scene::SceneManager& scene_manager);
		void End(scene::SceneManager& scene_manager);

		void Update(scene::SceneManager& scene_manager);

		void RenderImGui(ecs::Registry* registry);

		/**
		 * @brief : ウェーブの読み込み処理
		 * @param load_path : ロード時のファイルパス(※拡張子含む)
		 */
		void Load(const std::filesystem::path& load_path = { "" });
		/**
		 * @brief : ウェーブの保存処理
		 * @param save_path : ロード時のファイルパス(※拡張子含む)
		 */
		void Save(const std::filesystem::path& save_path = { "" });

		/**
		 * @brief : ウェーブ変更時のイベント登録
		 */
		void RegisterEventChanged(const mapping::rename_type::UUID& id, const std::function<void()>& func);

		/**
		 * @brief : ウェーブシーンの追加
		 * @param scene_id : シーンID(UUID)
		 */
		void AddWave(const mapping::rename_type::UUID& scene_id);

		/**
		 * @brief : ウェーブシーンの変更
		 */
		void ChangeWave(scene::SceneManager& scene_manager, bool is_first);

	private:
		/**
		 * @brief : ウェーブ用シーン
		 * @remark : 型(UUID) : シーンのアセットID
		 */
		std::vector<mapping::rename_type::UUID> waves_id{};

		u_int wave_index{ 0 };
		bool is_active{ true };

		Event<void> on_wave_changed{};
		StateMachine<WaveState, void, scene::SceneManager&> wave_state{};

		//--  @remark : StateMachine登録用関数 --//
		/**
		 * @brief : ゲーム開始時の状態
		 */
		void StartGame(scene::SceneManager& scene_manager);
		/**
		 * @brief :  ゲーム中の状態
		 */
		void UpdateGame(scene::SceneManager& scene_manager);
		/**
		 *  @brief : ウェーブシーンの変更時の状態
		 */
		void ChangeWave(scene::SceneManager& scene_manager);
	};

} // cumulonimbus::system
