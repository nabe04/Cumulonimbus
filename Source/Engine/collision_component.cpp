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
	//template <class Archive>
	//void CollisionComponent::serialize(Archive&& archive)
	//{
	//	archive(
	//		cereal::base_class<ComponentBase>(this),
	//		CEREAL_NVP(collision_tag)
	//	);
	//}

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
	}

	CollisionComponent::CollisionComponent(ecs::Registry* registry,
		mapping::rename_type::Entity ent, CollisionTag tag)
		:ComponentBase{ registry,ent }
	{

	}

	CollisionComponent::CollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const CollisionComponent& copy_comp)
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	void CollisionComponent::SetCollisionTag(CollisionTag tag)
	{
		collision_tag = tag;
	}

	CollisionTag CollisionComponent::GetCollisionTag() const
	{
		return collision_tag;
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

		if (helper::imgui::Combo("Socket", socket_name, node_name_items))
		{// 親のボーン位置が変更されたので変更されたパラメータの再設定
			//for (int node_index = 0; node_index < node_name_items.size(); ++node_index)
			//{
			//	// 選択されたいるノード名と現在のノード名が一致するまで処理を飛ばす
			//	if (node_name_items.at(node_index) != parent_node_data.node_name)
			//		continue;
			//	// 「None」+ ノードの名郡なので実際のノード位置は -> node_index - 1
			//	// 「None」の場合は-1になるが親のボーン計算時に0未満はワールドトランスフォームを掛けるようにする
			//	parent_node_data.node_index = node_index - 1;
			//	GetRegistry()->GetComponent<TransformComponent>(GetEntity()).SetDirtyFlg(TransformComponent::Animation_Changed);
			//	break;
			//}
		}
	}

} // cumulonimbus::component