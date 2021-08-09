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
		 * @brief : �ŏ��l�A�ő�l���烉���_����float�l��ݒ肷��\����
		 * @brief : �p�r�͎��R
		 */
		struct RandomFloat
		{
			float min			= 0; // �ŏ��l
			float max			= 1; // �ő�l
			float random_val	= 0; // min����max�܂ł̃����_���l
			float current_time	= 0; // �v������

			/**
			 * @brief : min,max�̒l�����Ƃ�random_val�̒l��ݒ�
			 */
			void SetRandomVal();
		};

	};
} // cumulonimbus::component
