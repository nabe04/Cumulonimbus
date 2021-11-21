#pragma once
#include <unordered_map>
#include <string>

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
		explicit EnemyBaseComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const EnemyBaseComponent& copy_comp); // for prefab
		explicit EnemyBaseComponent()  = default; // for cereal
		~EnemyBaseComponent() override = default;

		void Start()					   override{};

		void GameUpdate(float dt)		   override {}
		void RenderImGui()				   override {}
		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

	protected:
		/**
		 * @brief : �ŏ��l�A�ő�l���烉���_����float�l��ݒ肷��\����
		 * @brief : �p�r�͎��R
		 */
		struct RandomFloat
		{
			float min = 0; // �ŏ��l
			float max = 1; // �ő�l
			float random_val = 0; // min����max�܂ł̃����_���l
			float current_time = 0; // �v������

			/**
			 * @brief : min,max�̒l�����Ƃ�random_val�̒l��ݒ�
			 */
			void SetRandomVal();

			template <class Archive>
			void serialize(Archive&& archive)
			{
				archive(
					CEREAL_NVP(min),
					CEREAL_NVP(max),
					CEREAL_NVP(random_val),
					CEREAL_NVP(current_time)
				);
			}
		};


		std::unordered_map<std::string, RandomFloat> transition_timer{};
		// ���f����yaw��](�x���@)�̃����_���l
		RandomFloat random_rotation_angle{};

		/**
		 * @brief : �_���[�W����
		 */
		virtual void OnDamaged() {};

		/**
		 * @brief			: "transition_timer"�̓o�^
		 *					  ���łɓo�^����Ă����ꍇ�l�̏㏑��������
		 * @param key_value	: "transition_timer"�Ŏg�p����L�[�l
		 * @param min		: "transition_timer"���̃����_���l(�ŏ��l)
		 * @param max		: "transition_timer"���̃����_���l(�ő�l)
		 */
		void RegisterTransitionTimer(const std::string& key_value,float min,float max);

		/**
		* @brief		: "random_rotation_angle"�����o�ϐ��̒l�ݒ�p�֐�
		* @brief		: min�Amax�Ŏw�肵���͈͓��ł̃����_���l�̎Z�o
		* @param min	: �ŏ��l
		* @param max	: �ő�l
		*/
		void SetRandomRotationAngle(float min, float max);

		/**
		 * @brief : ���g�̈ʒu�ƑO���x�N�g��������G�͈͓�(rad)�A���G������(length)��
		 *			�^�[�Q�b�g(target_pos)�����邩����
		 * @param target_pos    : ���G�Ώۈʒu
		 * @param search_range  : ���G�͈�(�x���@)
		 * @param search_length : ���G����
		 * @return : true -> ���G�͈͓�
		 */
		[[nodiscard]] bool Search(const DirectX::SimpleMath::Vector3& target_pos, float search_range, float search_length) const;

		/**
		 * @brief				: �w�肵���ʒu�ւ̒ǐՏ���(���f���̉�]�������܂�)
		 * @param target_pos	: �ǐՐ�̈ʒu
		 * @param velocity		: �ǐՑ��x
		 * @param distance		: ���g�ƒǐՑΏۂ̒�����Ԃ�
		 */
		void Tracking(DirectX::SimpleMath::Vector3 target_pos, const DirectX::SimpleMath::Vector3& velocity, float& distance);

		/**
		 * @brief : �v���C���[�̕����ւ̉�]����
		 */
		void RotateToPlayerDirection() const ;
	};
} // cumulonimbus::component
