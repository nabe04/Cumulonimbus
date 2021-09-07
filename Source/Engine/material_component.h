#pragma once
#include <memory>

#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

#include "component_base.h"
#include "constant_buffer.h"
#include "shader_interop_renderer.h"

namespace cumulonimbus::component
{
	class MaterialComponent : public ComponentBase
	{
	public:
		explicit MaterialComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit MaterialComponent()  = default; // for cereal
		~MaterialComponent() override = default;

		void NewFrame(float dt) override {}
		void Update(float dt)	override;
		void RenderImGui()		override;

		void Load(ecs::Registry* registry) override;
		
	//	[[nodiscard]] const MaterialCB& GetMaterialData() const { return cb_material->data; }
		void SetMaterialCB(const MaterialCB& material) const;
		void BindCBuffer(bool set_in_vs = true, bool set_in_ps = true) const;
		void SetAndBindCBuffer(const MaterialCB& material, bool set_in_vs = true, bool set_in_ps = true) const;
		void UnbindCBuffer() const;


		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				cereal::base_class<ComponentBase>(this),
				CEREAL_NVP(cb_material->data)
			);
		}
	private:
		std::shared_ptr<buffer::ConstantBuffer<MaterialCB>> cb_material;
	};
}

CEREAL_REGISTER_TYPE(cumulonimbus::component::MaterialComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::MaterialComponent)

