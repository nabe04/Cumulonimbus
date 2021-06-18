#pragma once

#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "collision_component.h"
#include "geometric_primitive_resource.h"

struct InnerSphereCollisionData
{
	DirectX::SimpleMath::Matrix  world_matrix;
	DirectX::SimpleMath::Vector3 ajust_pos;
	XMFLOAT3		color_RGB;
	float			radius;
	bool			isHit = false;
};