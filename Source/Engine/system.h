#pragma once
#include <map>
#include <functional>

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

} // cumulonimbus

namespace cumulonimbus::system
{
	class System final
	{
	public:
		explicit System();
		~System() = default;

		void Render(ecs::Registry* registry);
		/**
		 * @brief : �v���W�F�N�g���ʂ̐ݒ�֐��̓o�^
		 * @remark : ImGui��ł̐ݒ��ړI�Ƃ��Ă��邽��
		 *           ImGui�p�̊֐���o�^����
		 */
		void RegisterRenderFunction(const mapping::rename_type::Hash& hash, const  std::function<void(ecs::Registry* registry)>& render_func);
	private:
		// �o�^���ꂽ�^��Render�֐��̕ێ�
		std::map<mapping::rename_type::Hash, std::function<void(ecs::Registry* registry)>> render_functions{};

		std::unique_ptr<camera::CameraTexture> camera_texture{};
	};
} // cumulonimbus::system