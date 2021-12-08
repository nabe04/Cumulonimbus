#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include <unordered_map>
#include <string>

#include "collision_component.h"

namespace cumulonimbus
{
	namespace collision
	{
		struct Sphere
		{
			bool						 is_visible{ true };
			bool						 hidden_in_game{ true };
			float						 radius{ 1.f };	// 半径
			mapping::rename_type::UUID   id{}; // 球判別用ID(ユニークなもの)
			std::string					 bone_name{};	// FBXモデルのボーンの位置名
			CollisionPreset				 collision_preset{CollisionPreset::OverlapAll};	// コリジョンのプロファイル
			CollisionTag				 collision_tag{ CollisionTag::Object };
			DirectX::SimpleMath::Matrix  world_transform_matrix{ DirectX::SimpleMath::Matrix::Identity }; // ワールド変換行列
			DirectX::SimpleMath::Vector3 offset{};		// 球の位置調節値
			DirectX::SimpleMath::Vector4 base_color{ 1.f,1.f,1.f,1.f };
			DirectX::SimpleMath::Vector4 hit_color{ 1.f,.0f,.0f,1.f };
			std::unordered_map<mapping::rename_type::Entity, HitResult> hit_results{}; // ヒット結果(オブジェクト毎)

			template<class Archive>
			void load(Archive&& archive, uint32_t version);

			template<class Archive>
			void save(Archive&& archive, uint32_t version) const;

			HitResult* TryGetHitResult(const mapping::rename_type::Entity& ent);
			/**
			 * @brief : 判定されたエンティティの登録
			 */
			void RegisterHitEntity(const mapping::rename_type::Entity& ent, const HitResult& hit_result);
			/**
			 * @brief : 判定されたエンティティの登録解除
			 */
			void UnRegisterHitEntity(const mapping::rename_type::Entity& ent);
		};
	} // collision

	namespace component
	{
		class SphereCollisionComponent final : public CollisionComponent
		{
		public:
			using CollisionComponent::CollisionComponent;
			explicit SphereCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, collision::CollisionTag tag);
			explicit SphereCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const SphereCollisionComponent& copy_comp); // for prefab
			explicit SphereCollisionComponent()  = default; // for cereal
			~SphereCollisionComponent() override = default;

			void Start() override;

			void CommonUpdate(float dt) override;
			void PreGameUpdate(float dt)   override;
			void GameUpdate(float dt)     override;
			void PostGameUpdate(float dt) override;
			void RenderImGui()	 	  override;

			void Load(ecs::Registry* registry) override;

			template<class Archive>
			void load(Archive&& archive, uint32_t  version);

			template<class Archive>
			void save(Archive&& archive, uint32_t version) const;

			/**
			 * @brief		 : 球の追加
			 * @param name	 : 球名(std::unordered_mapのキー値として使用)
			 * @param sphere : 判定用の球データ
			 * @attention	 : nameを「""」とした場合、名前が「sphere(番号)」になる
			 */
			std::string AddSphere(const std::string& name = "", const collision::Sphere& sphere = {});

			/**
			 * @brief				: 球追加、位置をFBXモデルのボーンにアタッチ
			 * @param bone_name		: 関連付けたいFBXモデルのボーン名
			 * @param sphere_name	: 球名(std::unordered_mapのキー値として使用)
			 * @param sphere		: 判定用の球データ
			 * @attention			: nameを「""」とした場合、名前が「sphere(番号)」になる
			 */
			std::string AddSphereAndRegisterBoneName(
					const std::string&		 bone_name,
					const std::string&	     sphere_name = "",
					const collision::Sphere& sphere = {});

			/**
			 * @brief : sphere_idから保持している球コリジョンを取得
			 * @remark : sphere_idが一致しない場合nullptrを返す
			 * @param sphere_id : 取得したい球コリジョン情報
			 */
			collision::Sphere* TryGetSphere(const mapping::rename_type::UUID& sphere_id);

			/**
			 * @brief				: 個々のsphereの"offset"のセット
			 * @param sphere_name	: spheresに登録されているキー値
			 */
			void SetOffset(const std::string& sphere_name, const DirectX::SimpleMath::Vector3& offset);
			/**
			 * @brief : spheres全ての"offset"のセット
			 */
			void SetAllOffset(const DirectX::SimpleMath::Vector3& offset);

			/**
			 * @brief				: 個々のsphere1の"radius"のセット
			 * @param sphere_name	: spheresに登録されているキー値
			 * @param radius		: 半径
			 */
			void SetRadius(const std::string& sphere_name, float radius);
			/**
			 * @brief				: spheres全ての"offset"のセット
			 * @param radius		: 半径
			 */
			void SetAllRadius(float radius);

			/**
			 * @brief				: 個々のsphereの"collision_preset"のセット
			 * @param sphere_name	: spheresに登録されているキー値
			 * @param preset		: コリジョンのプロファイル
			 */
			void SetCollisionPreset(const std::string& sphere_name, collision::CollisionPreset preset);
			/**
			 * @brief		 : spheres全ての"collision_preset"のセット
			 * @param preset : コリジョンのプロファイル
			 */
			void SetAllCollisionPreset(collision::CollisionPreset preset);

			/**
			 * @brief : mapに登録されているcollision::Sphereの取得
			 */
			[[nodiscard]] std::unordered_map<std::string, collision::Sphere>& GetSpheres();
		private:
			// 判定用データ群
			std::unordered_map<std::string, collision::Sphere> spheres{};
		};
	} // component

} // cumulonimbus