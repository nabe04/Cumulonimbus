#include "camera_component.h"

#include <cassert>
#include <cmath>
#include <wrl.h>

#include <imgui.h>

#include "arithmetic.h"
#include "cereal_helper.h"
#include "cum_imgui_helper.h"
#include "ecs.h"
#include "framework.h"
#include "frame_buffer.h"
#include "locator.h"
#include "texture_loader.h"
// components
#include "billboard_component.h"
#include "scene.h"
#include "transform_component.h"
using namespace DirectX;

CEREAL_REGISTER_TYPE(cumulonimbus::component::CameraComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::CameraComponent)
CEREAL_CLASS_VERSION(cumulonimbus::component::CameraComponent, 1)

namespace cumulonimbus::camera
{
	CameraTexture::CameraTexture()
	{

	}

	CameraTexture::CameraTexture(system::System& system)
	{
		// System::Render関数内で使用する関数の登録
		system.RegisterRenderFunction(utility::GetHash<CameraTexture>(),
									  [&](ecs::Registry* registry) {RenderImGui(registry); });
	}

	void CameraTexture::RenderImGui(ecs::Registry* registry)
	{
		if (ImGui::CollapsingHeader(ICON_FA_VIDEO"Camera Setting", ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto* asset_manager = locator::Locator::GetAssetManager();
			auto* texture_loader = asset_manager->GetLoader<asset::TextureLoader>();
			// テクスチャ選択
			texture_loader->SelectableTexture(*asset_manager, tex_id);
			// テクスチャサイズ設定
			IMGUI_LEFT_LABEL(ImGui::DragFloat2, "Texture Size", reinterpret_cast<float*>(&tex_size), .5f, 1.f, 100000.f);
		}
	}

} // cumulonimbus::camera

namespace cumulonimbus::component
{
	//template <class Archive>
	//void CameraComponent::serialize(Archive&& archive)
	//{
	//	archive(
	//		cereal::base_class<ComponentBase>(this),
	//		CEREAL_NVP(camera),

	//		CEREAL_NVP(attach_entity),
	//		CEREAL_NVP(camera_length),
	//		CEREAL_NVP(is_use_camera_for_debug),
	//		CEREAL_NVP(is_active),
	//		CEREAL_NVP(is_main_camera)
	//	);
	//}

	template <class Archive>
	void CameraComponent::load(Archive&& archive, const uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(camera),

			CEREAL_NVP(attach_entity),
			CEREAL_NVP(camera_length),
			CEREAL_NVP(is_use_camera_for_debug),
			CEREAL_NVP(is_active),
			CEREAL_NVP(is_main_camera)
		);

		if(version == 1)
		{
			archive(
				CEREAL_NVP(camera_offset)
			);
		}

		SetCameraOffset(camera_offset);
	}

	template <class Archive>
	void CameraComponent::save(Archive&& archive, const uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(camera),

			CEREAL_NVP(attach_entity),
			CEREAL_NVP(camera_length),
			CEREAL_NVP(is_use_camera_for_debug),
			CEREAL_NVP(is_active),
			CEREAL_NVP(is_main_camera)
		);

		if(version >= 0)
		{
			archive(
				CEREAL_NVP(camera_offset)
			);
		}
	}

	CameraComponent::CameraComponent()
		:ComponentBase{}
	{
		camera = std::make_unique<camera::Camera>(static_cast<float>(locator::Locator::GetWindow()->Width()),
												  static_cast<float>(locator::Locator::GetWindow()->Height()));;
	}

	CameraComponent::CameraComponent(
		ecs::Registry* registry,
		const mapping::rename_type::Entity ent,
		const CameraComponent& copy_comp)
		:ComponentBase{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);

		registry->AddComponent<BillboardComponent>(ent);
		registry->GetComponent<BillboardComponent>(ent).SetRenderingTarget(render::RenderingTarget::SceneOnly);

		if (is_main_camera)
			SwitchMainCamera();
	}

	CameraComponent::CameraComponent(
		cumulonimbus::ecs::Registry* registry,
		const mapping::rename_type::Entity ent,
		const bool is_main_camera,
		float width, float height)
		:ComponentBase{ registry,ent }
	{
		camera = std::make_unique<camera::Camera>(width, height);

		registry->AddComponent<BillboardComponent>(ent);
		registry->GetComponent<BillboardComponent>(ent).SetRenderingTarget(render::RenderingTarget::SceneOnly);

		if (is_main_camera)
			SwitchMainCamera();
	}

	CameraComponent::CameraComponent(mapping::component_tag::ComponentTag tag)
		:ComponentBase{ tag }
	{

	}

	CameraComponent::CameraComponent(const CameraComponent& other)
		:ComponentBase{other},
		 attach_entity{other.attach_entity},
		 camera_length{other.camera_length},
		 is_active{other.is_active},
		 is_use_camera_for_debug{other.is_use_camera_for_debug},
		 is_main_camera{ other.is_main_camera }
	{
		camera = other.camera->Clone();
	}

	CameraComponent& CameraComponent::operator=(const CameraComponent& other)
	{
		if(this == &other)
		{
			return *this;
		}

		attach_entity			= other.attach_entity;
		camera_length			= other.camera_length;
		is_active				= other.is_active;
		is_use_camera_for_debug = other.is_use_camera_for_debug;
		is_main_camera			= other.is_main_camera;
		camera					= other.camera->Clone();

		return *this;
	}


	void CameraComponent::Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent)
	{
		// SceneView表示用ビルボード(カメラテクスチャ)の追加
		registry->AddComponent<BillboardComponent>(ent);
		registry->GetComponent<BillboardComponent>(ent).SetRenderingTarget(render::RenderingTarget::SceneOnly);

		// メインカメラにセット
		SwitchMainCamera();
	}

	void CameraComponent::CommonUpdate(const float dt)
	{
		UpdateObjectCamera(dt);
		camera->Update(dt);

		// Scene View表示用ビルボードテクスチャのセット
		const auto camera_tex_id = locator::Locator::GetSystem()->GetCameraTexture().GetTexId();
		GetRegistry()->GetComponent<BillboardComponent>(GetEntity()).SetTextureId(camera_tex_id);
	}

	void CameraComponent::SceneUpdate(const float dt)
	{

	}

	void CameraComponent::GameUpdate(float dt)
	{

	}

	void CameraComponent::RenderImGui()
	{
		if (GetRegistry()->CollapsingHeader<CameraComponent>(GetEntity(), "Camera"))
		{
			camera->RenderImGui();
			ImGui::Checkbox("Main Camera", &is_main_camera);
			ImGui::DragFloat("Camera Length" , &camera_length, 0.1f, 0.1f, FLT_MAX);
			ImGui::DragFloat3("Camera Offset", &camera_offset.x, .5f, 0, FLT_MAX);

			if(is_main_camera && (!is_old_main_camera))
			{
				SwitchMainCamera();
			}

			is_old_main_camera = is_main_camera;
		}

		//if (bool is_removed_component = false;
		//	CollapsingHeader("Camera",is_removed_component))
		//{
		//	if (is_removed_component)
		//	{
		//		GetRegistry()->RemoveComponent<CameraComponent>(GetEntity());
		//	}
		//}

		//ImGui::PushID(GetEntity().c_str());
		//if (ImGui::Button(ICON_FA_ELLIPSIS_V))
		//{
		//	ImGui::OpenPopup("context_id");
		//}
		//ImGui::PopID();
		//if (ImGui::BeginPopup("context_id"))
		//{
		//	if (ImGui::MenuItem("Remove Component"))
		//	{
		//		GetRegistry()->RemoveComponent<CameraComponent>(GetEntity());
		//	}
		//	ImGui::EndPopup();
		//}
		//ImGui::SameLine();
		//if (ImGui::TreeNode("CameraComponent"))
		//{
		//	if (ImGui::Button(ICON_FA_ELLIPSIS_V))
		//	{
		//		ImGui::OpenPopup("context_id");
		//	}
		//	ImGui::DragFloat("Camera Length", &camera_length, 0.1f, 0.1f, 1000.0f);

		//	ImGui::TreePop();
		//}
	}

	void CameraComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);

		camera->Initialize();
	}

	void CameraComponent::AttachObject(cumulonimbus::mapping::rename_type::Entity ent)
	{
		attach_entity = ent;
		is_use_camera_for_debug = false;
	}

	void CameraComponent::InitializeObjectCameraParam(float camera_length)
	{
		this->camera_length = camera_length;
		auto& transform_comp = GetRegistry()->GetComponent<cumulonimbus::component::TransformComponent>(attach_entity);
		// カメラの位置をエンティティの持つオブジェクトの後方にセット
		camera->SetEyePosition(transform_comp.GetPosition() + (transform_comp.GetModelFront() * -1 * camera_length));
		// 注視点をエンティティの位置にセット
		camera->SetFocusPosition(transform_comp.GetPosition());
	}

	void CameraComponent::SwitchMainCamera()
	{
		// 一度全てのCameraComponentのis_main_cameraをfalseに
		for(auto& camera_comp : GetRegistry()->GetArray<CameraComponent>().GetComponents())
		{
			camera_comp.is_main_camera = false;
		}
		// 自クラスのis_main_cameraをtrueに
		is_main_camera = true;
	}

	void CameraComponent::UpdateObjectCamera(float dt)
	{
		// camera_lengthとfront_vecからカメラの位置を算出
		const auto& position = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetPosition();

		camera->SetFocusPosition(position + camera->GetFocusOffset());
		camera->SetEyePosition(camera->GetFocusPosition() + (camera->GetCameraFront() * -1) * camera_length);
		//camera->SetFocusOffset(camera_offset);

		if(transition_time_counter <= transition_time)
		{
			transition_time_counter += dt;
			const float offset_x = Easing::GetEasingVal(transition_time_counter
				, camera_old_offset.x,
				camera_offset.x - camera_old_offset.x,
				transition_time,
				easing_name, easing_type);

			const float offset_y = Easing::GetEasingVal(transition_time_counter
				, camera_old_offset.y,
				camera_offset.y - camera_old_offset.y,
				transition_time,
				easing_name, easing_type);

			const float offset_z = Easing::GetEasingVal(transition_time_counter
				, camera_old_offset.z,
				camera_offset.z - camera_old_offset.z,
				transition_time,
				easing_name, easing_type);

			camera->SetFocusOffset(DirectX::SimpleMath::Vector3{ offset_x,offset_y,offset_z });
		}
		else
		{
			camera->SetFocusOffset(camera_offset);
		}


	}
} // cumulonimbus::component
