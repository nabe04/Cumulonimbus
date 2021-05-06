#pragma once
#include "scene.h"
#include "my_effekseer.h"

class EffectEnemyBiteComponent :public EffekseerComponent
{
public:

	EffectEnemyBiteComponent(Entity* entity)
		:EffekseerComponent{ entity} {}

	void Play()override
	{
		if (1)assert(!"Use functions with arguments");
		auto enemy = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Enemy);

		const auto& enemy_position = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Enemy)->GetComponent<TransformComponent>()->GetTransform()->GetPosition();

		auto* manager = GetEntity()->GetScene()->GetEffekseerManager()->GetManager();

		auto effects = GetEntity()->GetScene()->GetEffekseerResource()->GetResource(EffekseerResourceName::EnemyFang);

		const_cast<Effekseer::Manager*>(manager)->Play(effects, { enemy_position.x, enemy_position.y, enemy_position.z });
	}

	void PlayLoop()
	{
		auto enemy = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Enemy);
		if (!enemy)assert(!"Not found EnemyComponent");
		const auto& enemy_position = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Enemy)->GetComponent<TransformComponent>()->GetTransform()->GetPosition();

		auto* manager = GetEntity()->GetScene()->GetEffekseerManager()->GetManager();

		auto effects = GetEntity()->GetScene()->GetEffekseerResource()->GetResource(EffekseerResourceName::EnemyFang);

		if (const_cast<Effekseer::Manager*>(manager)->GetShown(handle) == false)
		{
			handle = const_cast<Effekseer::Manager*>(manager)->Play(effects, { enemy_position.x, 0.01f, enemy_position.z });
		}
		const_cast<Effekseer::Manager*>(manager)->SetScale(handle, 5.f, 5.f, 5.f);
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
#pragma once
