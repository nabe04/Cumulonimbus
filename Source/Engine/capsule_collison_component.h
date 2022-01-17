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
		struct Capsule
		{
			bool						 is_enable{ true }; // 判定処理を行うか
			bool						 is_visible{ true }; // 表示するか
			bool						 hidden_in_game{ true }; // ゲーム中に表示するか
			float						 length{ 2 };		 // カプセルの端点間の長さ
			float						 radius{ 1 };		 // カプセルの半径
			std::string					 name{};
			std::string					 bone_name{};		 // FBXモデルのボーンの位置名
			mapping::rename_type::UUID   id{};				 // カプセルID(ゲーム開始毎に変わる)
			CollisionType				 collision_type{ CollisionType::OverlapAll }; // コリジョンのプロファイル
			CollisionTag				 collision_tag{ CollisionTag::Object }; // 判定用タグ
			DirectX::SimpleMath::Matrix  world_transform_matrix{ DirectX::SimpleMath::Matrix::Identity }; // ワールド変換行列
			DirectX::SimpleMath::Vector3 offset{};			// カプセルの位置調節値
			DirectX::SimpleMath::Vector3 rotation{};		// カプセルの回転調節値(度数法)
			DirectX::SimpleMath::Vector3 start{ 0,-1,0 };	// カプセルの始点(デフォルト値 : { 0, 1, 0})
			DirectX::SimpleMath::Vector3 end  { 0, 1,0 };	// カプセルの終点(デフォルト値 : { 0,-1, 0})
			DirectX::SimpleMath::Vector4 base_color{ 1.f,1.f,1.f,1.f }; // コリジョンの基本色(判定外時)
			DirectX::SimpleMath::Vector4 hit_color{ 1.f,.0f,.0f,1.f };  // コリジョンの判定色(判定時)
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
		class CapsuleCollisionComponent final : public CollisionComponent
		{
		public:
			using CollisionComponent::CollisionComponent;
			explicit CapsuleCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, collision::CollisionTag tag);
			explicit CapsuleCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const CapsuleCollisionComponent& copy_comp); // for prefab
			explicit CapsuleCollisionComponent()  = default; // for cereal
			~CapsuleCollisionComponent() override = default;

			void Start() override;

			void CommonUpdate(float dt)		override;
			void PreGameUpdate(float dt)	override;
			void GameUpdate(float dt)		override;
			void PostGameUpdate(float dt)	override;
			void RenderImGui()				override;

			void Load(ecs::Registry* registry) override;

			template<class Archive>
			void load(Archive&& archive, uint32_t  version);

			template<class Archive>
			void save(Archive&& archive, uint32_t version) const;

			/**
			 * @brief		 : カプセルの追加
			 * @param name	 : カプセル名(std::unordered_mapのキー値として使用)
			 * @capsule		 : 判定用カプセルデータ
			 * @attencion	 : nameを「""」とした場合、名前が「capsule(番号)」になる
			 *
			 */
			std::string AddCapsule(
				const std::string&		  name = "",
				const collision::Capsule& capsule = {});

			/**
			 * @brief : カプセルの追加、位置をFBXモデルのボーンにアタッチ
			 * @param bone_name		: 関連付けたいFBXモデルのボーン名
			 * @param capsule_name	: 球名(std::unordered_mapのキー値として使用)
			 * @param capsule		: 判定用の球データ
			 * @attention			: nameを「""」とした場合、名前が「capsule(番号)」になる
			 */
			std::string AddAndRegisterBoneName(
				const std::string&		  bone_name,
				const std::string&		  capsule_name = "",
				const collision::Capsule& capsule = {});

			/**
			 * @brief : capsule_idから保持しているカプセルコリジョンを取得
			 * @remark : capsule_idが一致しない場合nullptrを返す
			 * @param capsule_id : 取得したいカプセルコリジョンを情報
			 */
			collision::Capsule* TryGetCapsuleFromId(const mapping::rename_type::UUID& capsule_id);
			/**
			 * @brief : カプセル名から個々のカプセル情報取得
			 * @remark : 存在しない場合はnullptrを返す
			 */
			[[nodiscard]]
			collision::Capsule* TryGetCapsuleFromName(const std::string& c_name);
			/**
			 * @brief : 全てのカプセルの当たり判定の行うかの設定
			 */
			void SetAllCollisionEnable(bool is_enable);

			/**
			 * @brief				: 個々のカプセルの"offset"のセット
			 * @param capsule_name	: capsuleに登録されているキー値
			 */
			void SetOffset(const std::string& capsule_name, const DirectX::SimpleMath::Vector3& offset);
			/**
			 * @brief : 全てのカプセルの"offset"のセット
			 */
			void SetAllOffset(const DirectX::SimpleMath::Vector3& offset);

			/**
			 * @brief				: 個々のcapsuleの"radius"のセット
			 * @param capsule_name	: capsuleに登録されているキー値
			 * @param radius		: 半径
			 */
			void SetRadius(const std::string& capsule_name, float radius);
			/**
			 * @brief		  : 全てのcapsuleの"offset"のセット
			 * @param radius  : 半径
			 */
			void SetAllRadius(float radius);

			/**
			 * @brief				: 個々のcapsuleの"length"のセット
			 * @param capsule_name	: capsuleに登録されているキー値
			 * @param length		: カプセル内の線分の長さ
			 */
			void SetLength(const std::string& capsule_name, float length);
			/**
			 * @brief		  : 全てのcapsuleの"length"のセット
			 * @param length  : カプセル内の線分の長さ
			 */
			void SetAllLength(float length);

			/**
			 * @brief				: 個々のcapsuleの"collision_type"のセット
			 * @param capsule_name	: capsulesに登録されているキー値
			 * @param preset		: コリジョンのプロファイル
			 */
			void SetCollisionType(const std::string& capsule_name, collision::CollisionType preset);
			/**
			 * @brief : capsules全ての"collision_type"のセット
			 */
			void SetAllCollisionType(collision::CollisionType preset);

			/**
			* @brief : mapに登録されているcollision::Capsuleの取得
			*/
			[[nodiscard]]
			std::unordered_map<std::string, collision::Capsule>& GetCapsules();
		private:
			// 判定用データ群
			std::unordered_map<std::string, collision::Capsule> capsules{};
		};
	} // component

} // cumulonimbus
