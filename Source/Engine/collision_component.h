#pragma once
#include <functional>

#include <DirectXMath.h>

#include <cereal/cereal.hpp>

#include "component_base.h"
#include "cereal_helper.h"
#include "constant_buffer.h"
#include "debug_collision.h"
#include "event.h"

namespace cumulonimbus::component
{
	class SphereCollisionComponent;
} // cumulonimbus::component

///*
//* brief : Shape of the hit decision
//*/
//enum class CollisionType
//{
//	Sphere,
//	InnerSphere,
//};
//

namespace cumulonimbus::collision
{
	/*
	 * brief : Tag for judging a hit
	 */
	enum class CollisionTag
	{
		Object,
		Player,
		Sword,
		Enemy,
		Camera,
		Player_Weapon,
		Enemy_Weapon,

		End
	};

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
		DirectX::XMFLOAT3			position		{ 0.0f,0.0f,0.0f };	// ヒット位置
		DirectX::XMFLOAT3			normal			{ 0.0f,0.0f,0.0f };	// ヒット位置の法線
		float						distance		{0.0f};
		int							material_index	{ -1 };
		bool						is_hit			{ false };	// ヒット状態
		bool						is_old_hit		{ false };	// 1フレーム前の"is_hit"値
		HitEvent					hit_event		{};	// ヒットイベント
		collision::CollisionTag     collision_tag   {}; // 判定されたコリジョンタグ
		ecs::Registry*				registry		{};	// 判定されたレジストリ
		mapping::rename_type::UUID  entity			{};	// 判定されたエンティティ
		mapping::rename_type::UUID	collision_id	{}; // 判定されたコリジョンID

		template <class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(position),
				CEREAL_NVP(normal),
				CEREAL_NVP(distance),
				CEREAL_NVP(material_index),
				CEREAL_NVP(is_hit),
				CEREAL_NVP(is_old_hit),
				CEREAL_NVP(hit_event)
			);
		}
	};
} // cumulonimbus::collision

namespace cumulonimbus::component
{
	class CollisionComponent : public ComponentBase
	{
	public:
		using ComponentBase::ComponentBase;
		explicit CollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, collision::CollisionTag tag);
		explicit CollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const CollisionComponent& copy_comp); // for prefab
		explicit CollisionComponent()  = default; // for cereal
		CollisionComponent(const CollisionComponent& other);
		CollisionComponent& operator=(const CollisionComponent& other);
		~CollisionComponent() override = default;

		void PreGameUpdate(float dt)   override {}
		void GameUpdate(float dt)     override {}
		void PostGameUpdate(float dt) override {}
		void RenderImGui()			 override {}

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		[[nodiscard]]
		buffer::ConstantBuffer<DebugCollisionCB>& GetCBuffer() const
		{
			return *cbuffer.get();
		}

		/**
		 * @brief : 他のCollisionに触れたときのイベント登録
		 */
		void RegisterEventEnter(const mapping::rename_type::UUID& id,const std::function<void(const collision::HitResult&)>& func);

		/**
		 * @brief : 他のCollisionに触れるのをやめたときのイベント登録
		 */
		void RegisterEventExit(const mapping::rename_type::UUID& id, const std::function<void(const collision::HitResult&)>& func);

		/**
		 * @brief : 他のCollisionに触れている間のイベント登録
		 */
		void RegisterEventStay(const mapping::rename_type::UUID& id, const std::function<void(const collision::HitResult&)>& func);

		/**
		 * @brief : 他のどのCollisionにも触れていない間のイベント登録
		 */
		void RegisterEventNone(const mapping::rename_type::UUID& id, const std::function<void(const collision::HitResult&)>& func);

	protected:
		std::unique_ptr<buffer::ConstantBuffer<DebugCollisionCB> >cbuffer{};
		// 他のCollisionに触れたときのイベント
		system::Event<void, const collision::HitResult&> on_collision_enter_event;
		// 他のCollisionに触れるのをやめたときのイベント
		system::Event<void, const collision::HitResult&> on_collision_exit_event;
		// 他のCollisionに触れている間のイベント
		system::Event<void, const collision::HitResult&> on_collision_stay_event;
		// 他のどのCollisionにも触れていない間のイベント
		system::Event<void, const collision::HitResult&> on_collision_none;
		// GUI上で現在選択されているコリジョンの名前
		std::string selected_collision_name{};
		collision::CollisionTag tag{};

		/**
		 * @brief : ModelComponentを保持していた場合のボーン位置の変更
		 * @remark : ImGui描画処理内で使用する
		 */
		void AttachSocket(std::string& socket_name);
	};
} // cumulonimbus::component

