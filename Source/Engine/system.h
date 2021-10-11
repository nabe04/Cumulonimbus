#pragma once
#include <filesystem>
#include <functional>
#include <map>

#include "rename_type_mapping.h"

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	} // ecs

	namespace camera
	{
		class CameraTexture;
	} // camera

	namespace graphics
	{
		class SkyBox;
	} // graphics

} // cumulonimbus

namespace cumulonimbus::system
{
	class System final
	{
	public:
		explicit System();
		~System() = default;

		//template<class Archive>
		//void serialize(Archive&& archive, std::uint32_t version);

		template<class Archive>
		void save(Archive&& archive, std::uint32_t version) const;

		template<class Archive>
		void load(Archive&& archive, std::uint32_t version);

		/**
		 * @brief : �u.system�v�t�@�C���̃Z�[�u
		 * @param save_path : �u.system�v�t�@�C���܂ł̃p�X(���g���q(�u.sys�v)�܂�)
		 *					   (�p�X���w�肵�Ȃ��ꍇ�f�t�H���g��
		 *					   �p�X�ɕۑ������(./Data/System/system.sys))
		 */
		void Save(const std::filesystem::path& save_path = {""});

		/**
		 * @brief : �u.system�v�t�@�C���̃��[�h
		 * @param load_path : �u.system�v�t�@�C���܂ł̃p�X(���g���q(�u.sys�v)�܂�)
		 *					   (�p�X���w�肵�Ȃ��ꍇ�f�t�H���g��
		 *					   �p�X���烍�[�h�����(./Data/System/system.sys))
		 */
		void Load(const std::filesystem::path& load_path = {""});

		void Update(float dt);

		/**
		 * @brief : render_functions�ɓo�^���ꂽ�֐��̎��s
		 */
		void Render(ecs::Registry* registry);

		/**
		 * @brief : �v���W�F�N�g���ʂ̐ݒ�֐��̓o�^
		 * @remark : ImGui��ł̐ݒ��ړI�Ƃ��Ă��邽��
		 *           ImGui�p�̊֐���o�^����
		 */
		void RegisterRenderFunction(const mapping::rename_type::Hash& hash, const  std::function<void(ecs::Registry* registry)>& render_func);

		[[nodiscard]]
		camera::CameraTexture& GetCameraTexture() const;
		[[nodiscard]]
		graphics::SkyBox& GetSkyBox() const;
		[[nodiscard]]
		const std::string& GetCurrentScenePath() const { return current_scene_path; }
		[[nodiscard]]
		const std::string& GetDefaultScenePath() const { return default_scene_path; }

		void SetCurrentScenePath(const std::string& scene_path)
		{
			current_scene_path = scene_path;
			int a;
			a = 0;
		}
	private:
		// �o�^���ꂽ�^��Render�֐��̕ێ�
		std::map<mapping::rename_type::Hash, std::function<void(ecs::Registry* registry)>> render_functions{};

		std::unique_ptr<camera::CameraTexture> camera_texture{};
		std::unique_ptr<graphics::SkyBox>	   sky_box{};

		std::string current_scene_path{};  // ���݊J����Ă���V�[��(cereal��std::filesystem���Ή��̂���std::string�ɂ��Ă���)
		std::string default_scene_path{}; // �E�B���h�E�쐬���ɊJ���V�[��(cereal��std::filesystem���Ή��̂���std::string�ɂ��Ă���)
	};
} // cumulonimbus::system