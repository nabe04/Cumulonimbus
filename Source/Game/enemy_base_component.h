#pragma once
#include "actor3d_component.h"

namespace cumulonimbus::ecs
{
	class Registry;
} // cumulonimbus::ecs

namespace cumulonimbus::component
{
	class EnemyBaseComponent : public Actor3DComponent
	{
	public:
		explicit EnemyBaseComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit EnemyBaseComponent()  = default;
		~EnemyBaseComponent() override = default;

		void Update(float dt) override {}
		void RenderImGui()	  override {}
		void Save(const std::string& file_path)			 override {}
		void Load(const std::string& file_path_and_name) override {}

	protected:
		/**
		 * @brief : 最小値、最大値からランダムなfloat値を設定する構造体
		 * @brief : 用途は自由
		 */
		struct RandomFloat
		{
			float min			= 0; // 最小値
			float max			= 1; // 最大値
			float random_val	= 0; // minからmaxまでのランダム値
			float current_time	= 0; // 計測時間

			/**
			 * @brief : min,maxの値をもとにrandom_valの値を設定
			 */
			void SetRandomVal();
		};

	};
} // cumulonimbus::component
