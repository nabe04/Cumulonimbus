#pragma once

#include <memory>
#include <vector>

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
using Microsoft::WRL::ComPtr;

#include "ecs.h"
#include "shader.h"
#include "shader_manager.h"
#include "blend.h"
#include "rasterizer.h"
#include "geometric_primitive_resource.h"

class Spherecollision_component;
class InnerSpherecollision_component;

struct OriginCollisionData
{
	// Empty Data
};

/*
* brief : Shape of the hit decision
*/
enum class CollisionType
{
	Sphere,
	InnerSphere,
};

/*
* brief : Tag for judging a hit
*/
enum class CollisionTag
{
	Object,
	Player,
	Evasion,
	Enemy,
	Enemy_Bite,
	Enemy_Rush,
	Enemy_Scratch,
	Effect_Thunder,
	Effect_Lightning,
	Sword,
	Magic,
	Stage,
	Camera,

};

/**********************************************
*
*	collision_component Class
*
**********************************************/
class collision_component :public Component
{
private:
	virtual bool IntersectSphere(Spherecollision_component* other) { return false; };
	virtual bool IntersectInnerSphere(Spherecollision_component* other) { return false; }

protected:
	CollisionType type;
	CollisionTag  tag = CollisionTag::Object;

	//-- Debug info --//
	const XMFLOAT3 color_blue = XMFLOAT3{ 0.0f,0.7f,1.0f };
	const XMFLOAT3 color_red  = XMFLOAT3{ 1.0f,0.0f,1.0f };

	//-- State of Shader --//
	//shader::MeshShaderState shader_state{};
	shader::MeshShaderState shader_state;

	bool is_judge = true;

public:
	explicit collision_component(Entity* entity, CollisionTag tag);

	void Update(const float delta_time) override;

	bool Intersect(collision_component* comp);
	bool CompareTag(CollisionTag tag);
	CollisionTag GetTag() { return this->tag; }

	//-- Judgmental process --//
	virtual void OnCollision(const collision_component* other) {}

	//-- Get debug primitive info --//
	const shader::MeshShaderState& GetShaderState() const { return shader_state; }

	CollisionType GetType()const { return type; }
	CollisionTag GetTag()const { return tag; }

	void IsJudge(bool flg) { is_judge = flg; }
	bool IsJudge() { return is_judge; }
};