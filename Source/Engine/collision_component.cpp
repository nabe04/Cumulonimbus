#include "collision_component.h"

#include "scene.h"
#include "filename_helper.h"
// components
#include "model_component.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::CollisionComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::CollisionComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::CollisionComponent, 0)

namespace cumulonimbus::component
{
	template <class Archive>
	void CollisionComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(collision_tag)
		);
	}

	template <class Archive>
	void CollisionComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(collision_tag)
		);
	}

	void CollisionComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
		if (cbuffer)
			cbuffer.reset();
		cbuffer = std::make_unique<buffer::ConstantBuffer<DebugCollisionCB>>(locator::Locator::GetDx11Device()->device.Get());
	}

	CollisionComponent::CollisionComponent(ecs::Registry* registry,
		mapping::rename_type::Entity ent, CollisionTag tag)
		:ComponentBase{ registry,ent }
	{
		cbuffer = std::make_unique<buffer::ConstantBuffer<DebugCollisionCB>>(locator::Locator::GetDx11Device()->device.Get());
	}

	CollisionComponent::CollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const CollisionComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);

		if (cbuffer)
			cbuffer.reset();
		cbuffer = std::make_unique<buffer::ConstantBuffer<DebugCollisionCB>>(locator::Locator::GetDx11Device()->device.Get());
	}

	CollisionComponent::CollisionComponent(const CollisionComponent& other)
		:ComponentBase{other},
	     collision_tag{other.collision_tag},
	     selected_collision_name{other.selected_collision_name}
	{
		if (cbuffer)
			cbuffer.reset();
		cbuffer = std::make_unique<buffer::ConstantBuffer<DebugCollisionCB>>(locator::Locator::GetDx11Device()->device.Get());
	}

	CollisionComponent& CollisionComponent::operator=(const CollisionComponent& other)
	{
		if (this == &other)
		{
			return *this;
		}

		ComponentBase::operator=(other);
		collision_tag = other.collision_tag;
		selected_collision_name = other.selected_collision_name;

		if (cbuffer)
			cbuffer.reset();

		cbuffer = std::make_unique<buffer::ConstantBuffer<DebugCollisionCB>>(locator::Locator::GetDx11Device()->device.Get());
		return *this;
	}



	void CollisionComponent::SetCollisionTag(CollisionTag tag)
	{
		collision_tag = tag;
	}

	CollisionTag CollisionComponent::GetCollisionTag() const
	{
		return collision_tag;
	}

	void CollisionComponent::RegisterEventEnter(const mapping::rename_type::UUID& id, const std::function<void()>& func)
	{
		on_collision_enter_event.RegistryEvent(id, func);
	}

	void CollisionComponent::RegisterEventExit(const mapping::rename_type::UUID& id, const std::function<void()>& func)
	{
		on_collision_exit_event.RegistryEvent(id, func);
	}

	void CollisionComponent::RegisterEventStay(const mapping::rename_type::UUID& id, const std::function<void()>& func)
	{
		on_collision_stay_event.RegistryEvent(id, func);
	}

	void CollisionComponent::RegisterEventNone(const mapping::rename_type::UUID& id, const std::function<void()>& func)
	{
		on_collision_none.RegistryEvent(id, func);
	}

	void CollisionComponent::AttachSocket(std::string& socket_name)
	{
		auto* model_comp = GetRegistry()->TryGetComponent<ModelComponent>(GetEntity());
		if (!model_comp)
			return;

		// ImGui::Comboで表示用のボーン名一覧
		std::vector<std::string> node_name_items{};
		node_name_items.reserve(model_comp->GetNodes().size() + 1);
		node_name_items.emplace_back(filename_helper::GetNone());
		for (const auto& node : model_comp->GetNodes())
		{
			node_name_items.emplace_back(node.name);
		}

		helper::imgui::Combo("Socket", socket_name, node_name_items);
	}

} // cumulonimbus::component