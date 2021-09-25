#include "material_component.h"

#include <fstream>

#include <imgui.h>
#include <cereal/archives/json.hpp>

#include "ecs.h"
#include "locator.h"
#include "constant_buffer_mapping.h"

namespace cumulonimbus::component
{
	MaterialComponent::MaterialComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{
		cb_material = std::make_shared<buffer::ConstantBuffer<MaterialCB>>(locator::Locator::GetDx11Device()->device.Get());

		cb_material->data.material.base_color = {0,0,0,0};
		cb_material->data.material.emissive_color = { 0,0,0,0 };
		cb_material->data.material.roughness = 1;
		cb_material->data.material.reflectance = 1;
		cb_material->data.material.metalness = 0;
	}

	void MaterialComponent::GameUpdate(float dt)
	{

	}


	void MaterialComponent::RenderImGui()
	{
		cb_material->data.material.metalness = 0;

		int a;
		a = 0;


		//if(ImGui::TreeNode("MaterialComponent"))
		//{
		//	ImGui::ColorEdit4("Base Color"		, (float*)&cb_material->data.material.base_color);
		//	ImGui::ColorEdit4("Emissive Color"	, (float*)&cb_material->data.material.emissive_color);

		//	const float drag_speed = 0.001f;
		//	ImGui::DragFloat("Roughness"  , &cb_material->data.material.roughness	, drag_speed, .0f, 1.f);
		//	ImGui::DragFloat("Reflectance", &cb_material->data.material.reflectance	, drag_speed, .0f, 1.f);
		//	ImGui::DragFloat("metalness"  , &cb_material->data.material.metalness	, drag_speed, .0f, 1.f);

		//	ImGui::TreePop();
		//}
	}

	void MaterialComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	void MaterialComponent::SetMaterialCB(const MaterialCB& material) const
	{
		cb_material->data = material;
	}

	void MaterialComponent::BindCBuffer(bool set_in_vs, bool set_in_ps) const
	{
		cb_material->Activate(locator::Locator::GetDx11Device()->immediate_context.Get(), CBSlot_Material, set_in_vs, set_in_ps);
	}

	void MaterialComponent::SetAndBindCBuffer(const MaterialCB& material, bool set_in_vs, bool set_in_ps) const
	{
		SetMaterialCB(material);
		BindCBuffer(set_in_vs, set_in_ps);
	}


	void MaterialComponent::UnbindCBuffer() const
	{
		cb_material->Deactivate(locator::Locator::GetDx11Device()->immediate_context.Get());
	}
}
