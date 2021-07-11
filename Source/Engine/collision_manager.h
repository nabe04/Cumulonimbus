#pragma once
#include <map>

#include <DirectXMath.h>

#include "rename_type_mapping.h"

class ModelData;

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	}// ecs

	namespace component
	{
		class FbxModelComponent;
	} // component

	namespace collision
	{
		struct HitResult;
	} // collision

} // cumulonimbus

namespace cumulonimbus::collision
{
	class CollisionManager final
	{
	public:
		explicit CollisionManager() = default;
		~CollisionManager()			= default;

		/*
		 * brief : CollisionComponentを持つエンティティ全体の当たり判定処理
		 */
		void Update(ecs::Registry* registry);

		/*
		 * brief : 地形として判定させたい物の登録
		 *		   SceneのInitialize関数内などのComponentを追加した段階
		 *		   に登録する
		 */
		void RegistryRayCastModel(mapping::rename_type::Entity ent);

	private:
		// レイキャストの判定を行う際の判定が行われる(地形)データ群
		std::vector<mapping::rename_type::Entity> ent_terrains{};
		
		/*
		 * レイとモデルの衝突判定
		 */
		bool IntersectRayVsModel(
			const DirectX::XMFLOAT3& start,
			const DirectX::XMFLOAT3& end,
			const component::FbxModelComponent& model,
			HitResult& result
		);
	};

} // cumulonimbus::collision