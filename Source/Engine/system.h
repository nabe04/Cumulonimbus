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
		 * @brief : ƒvƒƒWƒFƒNƒg‹¤’Ê‚Ìİ’èŠÖ”‚Ì“o˜^
		 * @remark : ImGuiã‚Å‚Ìİ’è‚ğ–Ú“I‚Æ‚µ‚Ä‚¢‚é‚½‚ß
		 *           ImGui—p‚ÌŠÖ”‚ğ“o˜^‚·‚é
		 */
		void RegisterRenderFunction(const mapping::rename_type::Hash& hash, const  std::function<void(ecs::Registry* registry)>& render_func);
	private:
		// “o˜^‚³‚ê‚½Œ^‚ÌRenderŠÖ”‚Ì•Û
		std::map<mapping::rename_type::Hash, std::function<void(ecs::Registry* registry)>> render_functions{};

		std::unique_ptr<camera::CameraTexture> camera_texture{};
	};
} // cumulonimbus::system