#include "collision_component.h"

#include "scene.h"
#include "collision.h"
#include "sphere_collision_component.h"
#include "file_path_helper.h"

collision_component::collision_component(Entity* entity,CollisionTag tag)
	: Component{ entity }
	, tag{ tag }
{
	shader_state.SetShaderState(shader::MeshShaderTypes::Standard);
}

void collision_component::Update(const float delta_time)
{
	auto* scene = GetEntity()->GetScene();
}

// Ž‚Á‚Ä‚é”»’èî•ñ‚Ìˆ—
bool collision_component::Intersect(collision_component* other)
{
	using namespace intersect;

	EachOtherType each_other_type = EachOtherType::End;

	if (this->type == CollisionType::Sphere && other->GetType() == CollisionType::Sphere)
	{
		each_other_type = EachOtherType::SphereAndSphere;
	}
	if (this->type == CollisionType::InnerSphere && other->GetType() == CollisionType::Sphere)
	{
		each_other_type = EachOtherType::InterSphereAndSphere;
	}

	if (this->is_judge == false || other->IsJudge() == false)
		return false;

	switch (each_other_type)
	{
	case EachOtherType::SphereAndSphere:
		return IntersectSphere(static_cast<Spherecollision_component*>(other));
		break;
	case EachOtherType::InterSphereAndSphere:
		return IntersectInnerSphere(static_cast<Spherecollision_component*>(other));
		break;
	}

	return false;
}

bool collision_component::CompareTag(CollisionTag tag)
{
	return this->tag == tag;
}

void collision_component::Save(std::string file_path)
{
	const std::string file_path_and_name = file_path + component_name + ".json";
	std::ofstream ofs(file_path_and_name);
	cereal::JSONOutputArchive o_archive(ofs);
	o_archive(
		cereal::base_class<Component>(this),
		CEREAL_NVP(type),
		CEREAL_NVP(tag),

		CEREAL_NVP(shader_state),
		CEREAL_NVP(is_judge)
	);
}

void collision_component::Load(Entity* entity, std::string file_path_and_name)
{
	std::ifstream ifs(file_path_and_name);
	cereal::JSONInputArchive i_archive(ifs);
	i_archive(
		cereal::base_class<Component>(this),
		CEREAL_NVP(type),
		CEREAL_NVP(tag),

		CEREAL_NVP(shader_state),
		CEREAL_NVP(is_judge)
	);

	this->entity = entity;
}


namespace cumulonimbus::component
{
	CollisionComponent::CollisionComponent(ecs::Registry* registry, ecs::Entity ent, CollisionTag tag)
		: ComponentBase{ registry , ent }
		, tag{ tag }
	{
		shader_state.SetShaderState(shader::MeshShaderTypes::Standard);
	}

	void CollisionComponent::Update(const float delta_time)
	{
		auto* scene = GetRegistry()->GetScene();
	}

	// Ž‚Á‚Ä‚é”»’èî•ñ‚Ìˆ—
	bool CollisionComponent::Intersect(CollisionComponent* other)
	{
		using namespace intersect;

		EachOtherType each_other_type = EachOtherType::End;

		if (this->type == CollisionType::Sphere && other->GetType() == CollisionType::Sphere)
		{
			each_other_type = EachOtherType::SphereAndSphere;
		}
		if (this->type == CollisionType::InnerSphere && other->GetType() == CollisionType::Sphere)
		{
			each_other_type = EachOtherType::InterSphereAndSphere;
		}

		if (this->is_judge == false || other->IsJudge() == false)
			return false;

		switch (each_other_type)
		{
		case EachOtherType::SphereAndSphere:
			return IntersectSphere(static_cast<component::SphereCollisionComponent*>(other));
			break;
		case EachOtherType::InterSphereAndSphere:
			return IntersectInnerSphere(static_cast<component::SphereCollisionComponent*>(other));
			break;
		}

		return false;
	}

	bool CollisionComponent::CompareTag(CollisionTag tag)
	{
		return this->tag == tag;
	}

	void CollisionComponent::Save(const std::string& file_path)
	{
		const std::string file_path_and_name = file_path + file_path_helper::GetTypeName<CollisionComponent>() + file_path_helper::GetJsonExtension();
		std::ofstream ofs(file_path_and_name);
		cereal::JSONOutputArchive o_archive(ofs);
		o_archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(type),
			CEREAL_NVP(tag),

			CEREAL_NVP(shader_state),
			CEREAL_NVP(is_judge)
		);
	}

	void CollisionComponent::Load(const std::string& file_path_and_name)
	{
		std::ifstream ifs(file_path_and_name);
		cereal::JSONInputArchive i_archive(ifs);
		i_archive(
			cereal::base_class<ComponentBase>(this),
			CEREAL_NVP(type),
			CEREAL_NVP(tag),

			CEREAL_NVP(shader_state),
			CEREAL_NVP(is_judge)
		);

	}
}
