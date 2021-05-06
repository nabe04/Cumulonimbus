#pragma once
#include "scene.h"
#include "my_effekseer.h"

class EffectEnemyScratchComponent :public EffekseerComponent
{
public:

	EffectEnemyScratchComponent(Entity* entity)
		:EffekseerComponent{ entity } {}

	void Play()override {
		auto enemy = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Enemy);
		if (!enemy)assert(!"Not found PlayerComponent");
		const auto& enemy_position = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Enemy)->GetComponent<TransformComponent>()->GetTransform()->GetPosition();

		auto* manager = GetEntity()->GetScene()->GetEffekseerManager()->GetManager();

		auto effects = GetEntity()->GetScene()->GetEffekseerResource()->GetResource(EffekseerResourceName::EnemyClaw);

		handle = const_cast<Effekseer::Manager*>(manager)->Play(effects, { enemy_position.x, enemy_position.y, enemy_position.z });

		const_cast<Effekseer::Manager*>(manager)->SetScale(handle, 10.f, 10.f, 10.f);
	}

	//void Play()
	//{
	//	auto enemy = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Enemy);
	//	if (!enemy)assert(!"Not found PlayerComponent");
	//	const auto& enemy_position = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Enemy)->GetComponent<TransformComponent>()->GetTransform()->GetPosition();

	//	auto* manager = GetEntity()->GetScene()->GetEffekseerManager()->GetManager();

	//	auto effects = GetEntity()->GetScene()->GetEffekseerResource()->GetResource(EffekseerResourceName::EnemyClaw);

	//	handle = const_cast<Effekseer::Manager*>(manager)->Play(effects, { enemy_position.x, enemy_position.y, enemy_position.z });

	//	const_cast<Effekseer::Manager*>(manager)->SetScale(handle, 10.f, 10.f, 10.f);
	//}

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

