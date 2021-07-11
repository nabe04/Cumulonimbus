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
		 * brief : CollisionComponent�����G���e�B�e�B�S�̂̓����蔻�菈��
		 */
		void Update(ecs::Registry* registry);

		/*
		 * brief : �n�`�Ƃ��Ĕ��肳���������̓o�^
		 *		   Scene��Initialize�֐����Ȃǂ�Component��ǉ������i�K
		 *		   �ɓo�^����
		 */
		void RegistryRayCastModel(mapping::rename_type::Entity ent);

	private:
		// ���C�L���X�g�̔�����s���ۂ̔��肪�s����(�n�`)�f�[�^�Q
		std::vector<mapping::rename_type::Entity> ent_terrains{};
		
		/*
		 * ���C�ƃ��f���̏Փ˔���
		 */
		bool IntersectRayVsModel(
			const DirectX::XMFLOAT3& start,
			const DirectX::XMFLOAT3& end,
			const component::FbxModelComponent& model,
			HitResult& result
		);
	};

} // cumulonimbus::collision