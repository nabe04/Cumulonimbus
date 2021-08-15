#pragma once
#include <DirectXMath.h>

#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

#include "component_base.h"

namespace cumulonimbus::component
{
	class SphereCollisionComponent;
} // cumulonimbus::component


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
	Enemy,
	Camera,
};

namespace cumulonimbus::collision
{
	/**
	 * @brief : コリジョンのプロファイル
	 */
	enum class CollisionPreset
	{
		BlockAll,		// すべてのアクタをブロックする(お互いのコリジョンがBlockAll時に限る)(RigidBodyComponentを持つものに限る)
		OverlapAll,		// すべてのアクトをオーバーラップする
		NoCollision,	// コリジョンなし

		End
	};

	/**
	 * @brief : コリジョンに発生するヒットイベント
	 */
	enum class HitEvent
	{
		OnCollisionEnter,	// 他のCollisionに触れたとき
		OnCollisionExit,	// 他のCollisionに触れるのをやめたとき
		OnCollisionStay,	// 他のCollisionに触れている間
		None,				// 他のどのCollisionにも触れていない間

		End
	};

	/**
	 * @brief : ヒット結果
	 */
	struct HitResult
	{
		DirectX::XMFLOAT3	position		= { 0.0f,0.0f,0.0f };	// ヒット位置
		DirectX::XMFLOAT3	normal			= { 0.0f,0.0f,0.0f };	// ヒット位置の法線
		float				distance		= 0.0f;
		int					material_index	= -1;
		bool				is_hit			= false;				// ヒット状態
		bool				is_old_hit		= false;				// 1フレーム前の"is_hit"値
		HitEvent			hit_event;								// ヒットイベント
	};
} // cumulonimbus::collision

namespace cumulonimbus::component
{
	class CollisionComponent : public ComponentBase
	{
	public:
		explicit CollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, CollisionTag tag);
		explicit CollisionComponent()  = default;
		~CollisionComponent() override = default;

		void NewFrame(float dt)   override {}
		void Update(float dt)     override {}
		void PostUpdate(float dt) override {}
		void RenderImGui()		  override {}

		void Load(const std::string& file_path_and_name) override {};
		void Save(const std::string& file_path)			 override {};

		void SetCollisionTag(CollisionTag tag);
		[[nodiscard]] CollisionTag GetCollisionTag() const;

		collision::HitResult& GetHitResult();
	protected:
		CollisionTag		 collision_tag;
		collision::HitResult hit_result;
	};
} // cumulonimbus::component

CEREAL_REGISTER_TYPE(cumulonimbus::component::CollisionComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::CollisionComponent)


///**********************************************
//*
//*	collision_component Class
//*
//**********************************************/
//namespace cumulonimbus::component
//{
//	class CollisionComponent :public ComponentBase
//	{
//	public:
//		explicit CollisionComponent() = default;
//		explicit CollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, CollisionTag tag);
//		~CollisionComponent() = default;
//
//		void NewFrame(float dt) override {};
//		void Update(float delta_time) override;
//		void RenderImGui() override {};
//
//		bool Intersect(CollisionComponent* comp);
//		bool CompareTag(CollisionTag tag);
//		CollisionTag GetTag() { return this->tag; }
//
//		//-- Judgmental process --//
//		virtual void OnCollision(const CollisionComponent* other) {}
//
//		//-- Get debug primitive info --//
//		[[nodiscard]] const shader::MeshShaderState& GetShaderState() const { return shader_state; }
//
//		[[nodiscard]] CollisionType GetType()const { return type; }
//		[[nodiscard]] CollisionTag GetTag()const { return tag; }
//
//		void IsJudge(bool flg) { is_judge = flg; }
//		[[nodiscard]] bool IsJudge() const { return is_judge; }
//
//		void Save(const std::string& file_path) override;
//		void Load(const std::string& file_path_and_name) override;
//
//		template<typename Archive>
//		void serialize(Archive&& archive)
//		{
//			archive(
//				cereal::base_class<ComponentBase>(this),
//				CEREAL_NVP(type),
//				CEREAL_NVP(tag),
//
//				CEREAL_NVP(shader_state),
//				CEREAL_NVP(is_judge)
//			);
//		}
//
//	protected:
//		CollisionType type = CollisionType::Sphere;
//		CollisionTag  tag = CollisionTag::Object;
//
//		//-- Debug info --//
//		DirectX::XMFLOAT3 color_blue = DirectX::XMFLOAT3{ 0.0f,0.7f,1.0f };
//		DirectX::XMFLOAT3 color_red  = DirectX::XMFLOAT3{ 1.0f,0.0f,1.0f };
//
//		//-- State of Shader --//
//		//shader::MeshShaderState shader_state{};
//		shader::MeshShaderState shader_state{};
//
//		bool is_judge = true;
//
//	private:
//		virtual bool IntersectSphere(component::SphereCollisionComponent* other) { return false; }
//		virtual bool IntersectInnerSphere(component::SphereCollisionComponent* other) { return false; }
//	};
//}
//
//CEREAL_REGISTER_TYPE(cumulonimbus::component::CollisionComponent)
//CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::CollisionComponent)