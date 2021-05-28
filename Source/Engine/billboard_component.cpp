#include "billboard_component.h"

#include "scene.h"
#include "transform.h"
#include "transform_component.h"
#include "geometric_primitive_component.h"

BillbordComponent::BillbordComponent(Entity* entity)
	:MeshObject{ entity }
{
	GetEntity()->AddComponent<GeomPrimComponent>(GeomPrimType::Cube);

	// Active billboard
	GetEntity()->GetComponent<TransformComponent>()->ActiveBillboard();
}

void BillbordComponent::Update(const float delta_time)
{
	auto* transform = GetEntity()->GetComponent<TransformComponent>();

	GetEntity()->GetComponent<TransformComponent>()->GetBillboardRotation(transform->GetPosition(), GetEntity()->GetScene()->GetView()->GetEyePosition());
}