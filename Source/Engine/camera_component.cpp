#include "camera_component.h"

#include <cassert>
#include <cmath>
#include <wrl.h>

#include <imgui.h>

#include "arithmetic.h"
#include "cum_imgui_helper.h"
#include "ecs.h"
#include "framework.h"
#include "frame_buffer.h"
#include "locator.h"
// components
#include "billboard_component.h"
#include "scene.h"
#include "transform_component.h"
using namespace DirectX;

CEREAL_REGISTER_TYPE(cumulonimbus::component::CameraComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::CameraComponent)

namespace cumulonimbus::component
{
	template <class Archive>
	void CameraComponent::serialize(Archive&& archive)
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
	}

	CameraComponent::CameraComponent()
		:ComponentBase{}
	{
		camera = std::make_shared<camera::Camera>(locator::Locator::GetWindow()->Width(),
			locator::Locator::GetWindow()->Height());;
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
		camera = std::make_shared<camera::Camera>(width, height);

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
		camera = std::make_shared<camera::Camera>(*other.camera.get());
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
		camera					= std::make_shared<camera::Camera>(*other.camera.get());

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
		const auto camera_tex_id = GetRegistry()->GetScene()->GetEditorManager()->GetSystemInspector().GetCameraTexID();
		GetRegistry()->GetComponent<BillboardComponent>(GetEntity()).SetTextureId(camera_tex_id);
	}

	void CameraComponent::SceneUpdate(const float dt)
	{
		////if (is_use_camera_for_debug)
		////{
		////	camera->CalcCameraDirectionalVector();
		////	camera->CalcCameraAngle();
		////	camera->UpdateDefaultCamera(dt);
		////	//CalcCameraDirectionalVector();
		////	//CalcCameraAngle(); //オイラー角で(現在)計算しているので今は使わない
		////	//UpdateDefaultCamera(dt);
		////}
		////else
		////{
		//	UpdateObjectCamera(dt);
		////}

		//{// TODO : クォータニオンテスト
		//	SimpleMath::Quaternion q;
		//	q = q.CreateFromAxisAngle({ 0,1,0 }, XMConvertToRadians(90));
		//	SimpleMath::Quaternion p = { 1,0,0,0 };
		//	SimpleMath::Quaternion inv_q;
		//	q.Inverse(inv_q);
		//	SimpleMath::Quaternion qp = q * p * inv_q;
		//	SimpleMath::Vector3 v{ 1,0,0 };
		//	SimpleMath::Vector3::Transform(v, q, v);
		//}

		//camera->Update(dt);
	}

	void CameraComponent::GameUpdate(float dt)
	{

	}

	void CameraComponent::RenderImGui()
	{
		if (ImGui::TreeNode("CameraComponent"))
		{
			ImGui::DragFloat("Camera Length", &camera_length, 0.1f, 0.1f, 1000.0f);

			ImGui::TreePop();
		}
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
		auto& position = GetRegistry()->GetComponent<TransformComponent>(GetEntity()).GetPosition();

		camera->SetFocusPosition(position + camera->GetFocusOffset());
		//camera->SetEyePosition(position + camera->GetFocusOffset());
		camera->SetEyePosition(camera->GetFocusPosition() + (camera->GetCameraFront() * -1) * camera_length);
	}
} // cumulonimbus::component
