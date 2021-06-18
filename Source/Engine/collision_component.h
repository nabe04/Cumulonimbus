#pragma once

#include <memory>
#include <vector>

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
using Microsoft::WRL::ComPtr;

#include <cereal/cereal.hpp>

#include "ecs.h"
#include "component_base.h"
#include "shader_manager.h"

namespace cumulonimbus::component
{
	class SphereCollisionComponent;
}


struct OriginCollisionData
{
	// Empty Data
};

/*
* brief : Shape of the hit decision
*/
enum class CollisionType
{
	Sphere,
	InnerSphere,
};

/*
* brief : Tag for judging a hit
*/
enum class CollisionTag
{
	Object,
	Player,
	Evasion,
	Enemy,
	Enemy_Bite,
	Enemy_Rush,
	Enemy_Scratch,
	Effect_Thunder,
	Effect_Lightning,
	Sword,
	Magic,
	Stage,
	Camera,

};

/**********************************************
*
*	collision_component Class
*
**********************************************/
namespace cumulonimbus::component
{
	class CollisionComponent :public ComponentBase
	{
	public:
		explicit CollisionComponent() = default;
		explicit CollisionComponent(ecs::Registry* registry, ecs::Entity ent, CollisionTag tag);
		~CollisionComponent() = default;

		void NewFrame(float dt) override {};
		void Update(float delta_time) override;
		void RenderImGui() override {};

		bool Intersect(CollisionComponent* comp);
		bool CompareTag(CollisionTag tag);
		CollisionTag GetTag() { return this->tag; }

		//-- Judgmental process --//
		virtual void OnCollision(const CollisionComponent* other) {}

		//-- Get debug primitive info --//
		[[nodiscard]] const shader::MeshShaderState& GetShaderState() const { return shader_state; }

		[[nodiscard]] CollisionType GetType()const { return type; }
		[[nodiscard]] CollisionTag GetTag()const { return tag; }

		void IsJudge(bool flg) { is_judge = flg; }
		[[nodiscard]] bool IsJudge() const { return is_judge; }

		void Save(const std::string& file_path) override;
		void Load(const std::string& file_path_and_name) override;

		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				cereal::base_class<ComponentBase>(this),
				CEREAL_NVP(type),
				CEREAL_NVP(tag),

				CEREAL_NVP(shader_state),
				CEREAL_NVP(is_judge)
			);
		}

	protected:
		CollisionType type = CollisionType::Sphere;
		CollisionTag  tag = CollisionTag::Object;

		//-- Debug info --//
		XMFLOAT3 color_blue = XMFLOAT3{ 0.0f,0.7f,1.0f };
		XMFLOAT3 color_red = XMFLOAT3{ 1.0f,0.0f,1.0f };

		//-- State of Shader --//
		//shader::MeshShaderState shader_state{};
		shader::MeshShaderState shader_state{};

		bool is_judge = true;

	private:
		virtual bool IntersectSphere(component::SphereCollisionComponent* other) { return false; }
		virtual bool IntersectInnerSphere(component::SphereCollisionComponent* other) { return false; }
	};
}

CEREAL_REGISTER_TYPE(cumulonimbus::component::CollisionComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::CollisionComponent)