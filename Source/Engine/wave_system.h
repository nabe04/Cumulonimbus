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
		 * @brief : �E�F�[�u�̓ǂݍ��ݏ���
		 * @param load_path : ���[�h���̃t�@�C���p�X(���g���q�܂�)
		 */
		void Load(const std::filesystem::path& load_path = { "" });
		/**
		 * @brief : �E�F�[�u�̕ۑ�����
		 * @param save_path : ���[�h���̃t�@�C���p�X(���g���q�܂�)
		 */
		void Save(const std::filesystem::path& save_path = { "" });

		/**
		 * @brief : �E�F�[�u�ύX���̃C�x���g�o�^
		 */
		void RegisterEventChanged(const mapping::rename_type::UUID& id, const std::function<void()>& func);

		/**
		 * @brief : �E�F�[�u�V�[���̒ǉ�
		 * @param scene_id : �V�[��ID(UUID)
		 */
		void AddWave(const mapping::rename_type::UUID& scene_id);

		/**
		 * @brief : �E�F�[�u�V�[���̕ύX
		 */
		void ChangeWave(scene::SceneManager& scene_manager, bool is_first);

	private:
		/**
		 * @brief : �E�F�[�u�p�V�[��
		 * @remark : �^(UUID) : �V�[���̃A�Z�b�gID
		 */
		std::vector<mapping::rename_type::UUID> waves_id{};

		u_int wave_index{ 0 };
		bool is_active{ true };

		Event<void> on_wave_changed{};
		StateMachine<WaveState, void, scene::SceneManager&> wave_state{};

		//--  @remark : StateMachine�o�^�p�֐� --//
		/**
		 * @brief : �Q�[���J�n���̏��
		 */
		void StartGame(scene::SceneManager& scene_manager);
		/**
		 * @brief :  �Q�[�����̏��
		 */
		void UpdateGame(scene::SceneManager& scene_manager);
		/**
		 *  @brief : �E�F�[�u�V�[���̕ύX���̏��
		 */
		void ChangeWave(scene::SceneManager& scene_manager);
	};

} // cumulonimbus::system
