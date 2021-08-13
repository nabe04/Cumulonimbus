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
			DirectX::SimpleMath::Matrix  world_transform_matrix{ DirectX::SimpleMath::Matrix::Identity }; // ワールド変換行列
			DirectX::SimpleMath::Vector3 offset{};					 // カプセルの位置調節値
			DirectX::SimpleMath::Vector3 rotation{};				 // カプセルの回転調節値(度数法)
			DirectX::SimpleMath::Vector3 start{ 0,-1,0 };			 // カプセルの始点(デフォルト値 : { 0, 1, 0})
			DirectX::SimpleMath::Vector3 end  { 0, 1,0 };			 // カプセルの終点(デフォルト値 : { 0,-1, 0})
			std::string bone_name{ "" };							 // FBXモデルのボーンの位置名
			float		length = 2;									 // カプセルの端点間の長さ
			float		radius = 1;									 // カプセルの半径
			HitResult   hit_result;									 // ヒット結果
		};
	} // collision

	namespace component
	{
		class CapsuleCollisionComponent final : public CollisionComponent
		{
		public:
			explicit CapsuleCollisionComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, CollisionTag tag);
			explicit CapsuleCollisionComponent()  = default; // for cereal
			~CapsuleCollisionComponent() override = default;

			void NewFrame(float dt)		override;
			void Update(float dt)		override;
			void PostUpdate(float dt)	override;
			void RenderImGui()			override;

			void Load(const std::string& file_path_and_name) override;
			void Save(const std::string& file_path) override;

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
			 * @brief				: 個々のカプセルの"offset"のセット
			 * @param capsule_name	: capsuleに登録されているキー値
			 */
			void SetOffset(const std::string& capsule_name, const DirectX::SimpleMath::Vector3& offset);
			/**
			 * @brief : 全てのカプセルの"offset"のセット
			 */
			void SetAllOffset(const DirectX::SimpleMath::Vector3& offset);

			/**
			 * @brief				: 個々の"カプセルのradius"のセット
			 * @param capsule_name	: capsuleに登録されているキー値
			 * @param radius		: 半径
			 */
			void SetRadius(const std::string& capsule_name, float radius);

			/**
			 * @brief		  : 全てのカプセルの"offset"のセット
			 * @param radius  : 半径
			 */
			void SetAllRadius(float radius);

			/**
			 * @brief				: 個々のカプセルの"length"のセット
			 * @param capsule_name	: capsuleに登録されているキー値
			 * @param length		: カプセル内の線分の長さ
			 */
			void SetLength(const std::string& capsule_name, float length);

			/**
			 * @brief		  : 全てのカプセルの"length"のセット
			 * @param length  : カプセル内の線分の長さ
			 */
			void SetAllLength(float length);

			/**
			* @brief : mapに登録されているcollision::Capsuleの取得
			*/
			[[nodiscard]] std::unordered_map<std::string, collision::Capsule>& GetCapsules();
		private:
			// 判定用データ群
			std::unordered_map<std::string, collision::Capsule> capsules{};
		};
	} // component

} // cumulonimbus
