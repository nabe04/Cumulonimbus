#pragma once
#include "scene.h"
#include "my_effekseer.h"

class EffectLightningFloorComponent :public EffekseerComponent
{
public:

	EffectLightningFloorComponent(Entity* entity)
		: EffekseerComponent{ entity } {}
	void Play()override
	{
		auto enemy = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Enemy);
		if (!enemy)assert(!"Not found PlayerComponent");
		const auto& enemy_position = enemy->GetComponent<TransformComponent>()->GetTransform()->GetPosition();

		auto* manager = GetEntity()->GetScene()->GetEffekseerManager()->GetManager();

		auto effects = GetEntity()->GetScene()->GetEffekseerResource()->GetResource(EffekseerResourceName::EnemyLightningFloor);

		const_cast<Effekseer::Manager*>(manager)->Play(effects, { enemy_position.x, enemy_position.y, enemy_position.z });
	}
	void Play(DirectX::XMFLOAT3 circle_position)
	{
		//	auto player = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Player);
		//	if (!player)assert(!"Not found PlayerComponent");
		//	const auto& player_position = player->GetComponent<TransformComponent>()->GetTransform()->GetPosition();

		auto* manager = GetEntity()->GetScene()->GetEffekseerManager()->GetManager();

		auto effects = GetEntity()->GetScene()->GetEffekseerResource()->GetResource(EffekseerResourceName::EnemyLightningFloor);

		handle = const_cast<Effekseer::Manager*>(manager)->Play(effects, { circle_position.x, 0.01f, circle_position.z });

		const_cast<Effekseer::Manager*>(manager)->SetScale(handle, 3.f, 3.f, 3.f);
	}
	void PlayLoop(DirectX::XMFLOAT3 circle_position)
	{
		auto enemy = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Enemy);
		if (!enemy)assert(!"Not found PlayerComponent");
		const auto& enemy_position = enemy->GetComponent<TransformComponent>()->GetTransform()->GetPosition();

		auto* manager = GetEntity()->GetScene()->GetEffekseerManager()->GetManager();

		auto effects = GetEntity()->GetScene()->GetEffekseerResource()->GetResource(EffekseerResourceName::EnemyLightningFloor);

		if (const_cast<Effekseer::Manager*>(manager)->GetShown(handle) == false)
		{
			handle = const_cast<Effekseer::Manager*>(manager)->Play(effects, { circle_position.x, 0.01f, circle_position.z });
		}
		const_cast<Effekseer::Manager*>(manager)->SetScale(handle, 2.5f, 2.5f, 2.5f);
	}

	bool GetShown()
	{
		auto* manager = GetEntity()->GetScene()->GetEffekseerManager()->GetManager();

		return const_cast<Effekseer::Manager*>(manager)->GetShown(handle);
	}

	void Stop()
	{
		auto manager = GetEntity()->GetScene()->GetEffekseerManager()->GetManager();

		// インスタンスが存在しているか確認して
		if (const_cast<Effekseer::Manager*>(manager)->Exists(handle))
		{
			// 再生を停止する
			const_cast<Effekseer::Manager*>(manager)->StopEffect(handle);
		}
	}

private:

};
