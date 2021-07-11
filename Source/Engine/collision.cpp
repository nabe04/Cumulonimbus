//#include "collision.h"
//
//#include "scene.h"
//#include "collision_component.h"
//#include "sphere_collision_component.h"
//#include "inner_sphere_collision.h"
//
//namespace intersect
//{
//	bool SphereSphere(const SphereCollisionData& sphere_data1, const SphereCollisionData& sphere_data2)
//	{
//		const float dis_x = sphere_data1.world_matrix.Translation().x - sphere_data2.world_matrix.Translation().x;
//		const float dis_y = sphere_data1.world_matrix.Translation().y - sphere_data2.world_matrix.Translation().y;
//		const float dis_z = sphere_data1.world_matrix.Translation().z - sphere_data2.world_matrix.Translation().z;
//
//		const float two_dis = sphere_data1.radius + sphere_data2.radius;
//		const float dis = sqrtf(dis_x * dis_x + dis_y * dis_y + dis_z * dis_z);
//
//		if (dis < two_dis)
//			return true;
//
//		return false;
//	}
//}

//void CollisionManager::Update(Scene* scene)
//{
//
//	//auto entities = scene->GetEntities<collision_component>();
//	//std::vector<collision_component*> collisions;
//
//	//if (entities.empty())
//	//	return;
//
//	//for (auto& ent : entities)
//	//{// Get all collision_components
//	//	collisions.emplace_back(ent->GetComponent<collision_component>());
//	//}
//
//	//for (size_t i = 0, size = collisions.size(); i < size - 1; ++i)
//	//{
//	//	auto* col1 = collisions[i];
//	//	for (size_t j = i + 1; j < size; ++j)
//	//	{
//	//		auto* col2 = collisions[j];
//	//		if (col1->Intersect(col2))
//	//		{
//	//			col1->OnCollision(col2);
//	//			col2->OnCollision(col1);
//	//		}
//	//	}
//	//}
//
//	CollisionTest(scene);
//
//}
//
//void CollisionManager::CollisionTest(Scene* scene)
//{
//	namespace cum = cumulonimbus;
//
//	//auto entities = scene->GetEntities<cum::component::CollisionComponent>();
//	//if (entities.empty())
//	//	return;
//
//	//typedef std::vector<cum::component::CollisionComponent*> my_collision_types;
//
//	//std::vector<my_collision_types> my_collisions;
//
//	//for (auto& ent : entities)
//	//{
//	//	my_collisions.emplace_back(ent->GetSameComponents<cum::component::CollisionComponent>());
//	//}
//
//	//if (my_collisions.empty())
//	//	return;
//
//
//	//for (size_t my_collision_index = 0; my_collision_index < my_collisions.size(); ++my_collision_index)
//	//{
//	//	for (size_t i_1 = 0 ; i_1 < my_collisions.at(my_collision_index).size(); ++i_1)
//	//	{
//	//		auto* col1 = my_collisions.at(my_collision_index).at(i_1);
//	//		for (size_t j = my_collision_index + 1; j < my_collisions.size(); ++j)
//	//		{
//	//			for (size_t i_2 = 0; i_2 < my_collisions.at(j).size(); ++i_2)
//	//			{
//	//				auto* col2 = my_collisions.at(j).at(i_2);
//
//	//				if (col1->Intersect(col2))
//	//				{
//	//					col1->OnCollision(col2);
//	//					col2->OnCollision(col1);
//	//				}
//	//			}
//	//		}
//	//	}
//	//}
//}
