//#include "test_mesh_particle.h"
//
//#include "arithmetic.h"
//#include "locator.h"
//#include "fbx_model_component.h"
//#include "scene.h"
//#include "transform.h"
//
//void TestMeshParticle::NewFrame(const float delta_time)
//{
//
//}
//
//void TestMeshParticle::Update(const float delta_time)
//{
//	const auto& key = Locator::GetInput()->Keyboard();
//
//	if(key.GetState(Keycode::V) == ButtonState::Press)
//	{
//		const auto* ent = GetEntity()->GetScene()->GetOtherEntity(EntityTag::Bunny);
//		SetParticleDataFromFbx(ent->GetComponent<FbxModelComponent>(), *ent->GetComponent<TransformComponent>()->GetTransform());
//	}
//
//	if(key.GetState(Keycode::B) == ButtonState::Press)
//	{
//		velocity.clear();
//		for(int i = 0;i<GetParticleDatas()->size();++i)
//		{
//			velocity.emplace_back(arithmetic::RandomFloatInRange(1, 5));
//		}
//	}
//
//	if(!velocity.empty())
//	{
//		for (int i = 0; i < GetParticleDatas()->size(); ++i)
//		{
//			auto& particle_data = GetParticleDatas()->at(i);
//			auto pos = particle_data.transform.GetPosition();
//			DirectX::SimpleMath::Vector3 v{ velocity.at(i) };
//			v.Normalize();
//			pos +=  v * velocity.at(i);
//			particle_data.transform.SetPosition(pos);
//		}
//	}
//}
//
//void TestMeshParticle::RenderImGui()
//{
//
//}
//
