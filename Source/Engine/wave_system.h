#pragma once
#include <vector>

#include "rename_type_mapping.h"

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
	public:
		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void Update(const scene::SceneManager& scene_manager);

		void RenderImGui(ecs::Registry* registry);

		/**
		 * @brief : ウェーブシーンの追加
		 * @param scene_id : シーンID(UUID)
		 */
		void AddWave(const mapping::rename_type::UUID& scene_id);

		/**
		 * @brief : ウェーブシーンの変更
		 */
		void ChangeWave();

	private:
		/**
		 * @brief : ウェーブ用シーン
		 * @remark : 型(UUID) : シーンのアセットID
		 */
		std::vector<mapping::rename_type::UUID> waves{};

		u_int wave_index = 0;
	};

} // cumulonimbus::system
