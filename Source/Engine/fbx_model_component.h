#pragma once

#include <memory>
#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>

#include "ecs.h"
#include "cereal_helper.h"
#include "state_machine.h"
#include "model_data.h"
#include "locator.h"

class FbxModelResource;

namespace shader
{
	class MeshShaderState;
	enum class MeshShaderTypes;
}

enum class FbxAnimationState
{
	Switch,
	Blending,
	Update,
};

class FbxModelComponent final : public Component
{
public:
	struct Node
	{
		std::string					name{};
		int							parent_index{ -1 };
		//Node*						parent{};
		DirectX::XMFLOAT3			scale{};
		DirectX::XMFLOAT4			rotate{};
		DirectX::XMFLOAT3			translate{};
		DirectX::SimpleMath::Matrix	local_transform{};
		DirectX::SimpleMath::Matrix	world_transform{};

		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				cereal::make_nvp("node_name"  , name),
				CEREAL_NVP(parent_index),
				//cereal::make_nvp("parent node", parent),
				CEREAL_NVP(scale),
				CEREAL_NVP(rotate),
				CEREAL_NVP(translate),
				CEREAL_NVP(local_transform),
				CEREAL_NVP(world_transform)
			);
		}
	};

	explicit FbxModelComponent() = default; // For cereal
	explicit FbxModelComponent(Entity* entity, const std::shared_ptr<FbxModelResource>& resource);
	~FbxModelComponent()override = default;

	void NewFrame(const float delta_time) override;
	void Update(const float delta_time) override;
	virtual void RenderImGui() override;

	// ƒAƒjƒ[ƒVƒ‡ƒ“
	[[nodiscard]] bool IsPlayAnimation() const { return current_animation_index >= 0; }
	void SwitchAnimation(int animation_index, bool loop = false, float switch_time = 0.1f);

	[[nodiscard]] const DirectX::SimpleMath::Matrix& GetNodeMatrix(const char* node_name);
	[[nodiscard]] const DirectX::SimpleMath::Matrix& GetNodePareantMatrix(const char* node_name);
	[[nodiscard]] const std::vector<Node>& GetNodes() const { return nodes; }
	[[nodiscard]] std::vector<Node>& GetNodes() { return nodes; }
	[[nodiscard]] const FbxModelResource* GetResource() const { return resource.get(); }
	[[nodiscard]] const DirectX::XMFLOAT4& GetColor() const { return color; }

	void UpdateAnimState(const float delta_time) { anim_states.Update(delta_time); }

	// Œ»İ‚ÌƒL[ƒtƒŒ[ƒ€
	[[nodiscard]] int CurrentKeyframe() { return current_keyframe; }

	void Save(std::string file_path) override;
	void Load(Entity* entity, std::string file_path_and_name) override;

	// For cereal
	template<typename Archive>
	void serialize(Archive&& archive)
	{
		archive(
			cereal::base_class<Component>(this),
<<<<<<< HEAD
<<<<<<< HEAD
			CEREAL_NVP(resource),
			CEREAL_NVP(nodes),
=======
			//CEREAL_NVP(resource),
=======
			CEREAL_NVP(resource),
>>>>>>> origin/Nabe_Note
			//CEREAL_NVP(nodes),
>>>>>>> parent of 4df3c85... FbxModelCompoentã®Nodeæ§‹é€ ä½“ä¿®æ­£å‰ä¿å­˜

			CEREAL_NVP(current_animation_index),
			CEREAL_NVP(prev_animation_index),
			CEREAL_NVP(current_keyframe),
			CEREAL_NVP(prev_key_index),

			CEREAL_NVP(current_seconds),
			CEREAL_NVP(prev_seconds),
			CEREAL_NVP(animation_switch_time),
			CEREAL_NVP(changer_timer),

			CEREAL_NVP(loop_animation),
			CEREAL_NVP(end_animation),

			CEREAL_NVP(prev_animation),
			CEREAL_NVP(anim_states)
		);
	}

private:
	std::shared_ptr<FbxModelResource>	resource;
	std::vector<Node>					nodes;

	int	current_animation_index = -1;
	int	prev_animation_index	= -1;	// ‘O‚ÌƒAƒjƒ[ƒVƒ‡ƒ“‚ÌƒCƒ“ƒfƒbƒNƒX”Ô†(ƒuƒŒƒ“ƒh‚Åg—p)
	int	current_keyframe		= 0;	// Œ»İ‚ÌƒL[ƒtƒŒ[ƒ€
	int	prev_key_index			= 0;

	float current_seconds		= 0.0f;
	float prev_seconds			= 0.0f; // ƒAƒjƒ[ƒVƒ‡ƒ“‚ªØ‚è‘Ö‚í‚Á‚½“_‚Ì‘O‚ÌƒAƒjƒ[ƒVƒ‡ƒ“‚ÌƒL[ƒtƒŒ[ƒ€(ƒuƒŒƒ“ƒh‚Åg—p)
	float animation_switch_time = 0;
	float changer_timer			= 0;

	bool loop_animation = false;
	bool end_animation	= false;


	ModelData::Animation prev_animation;
	StateMachine<FbxAnimationState, void, const float>	anim_states;

	DirectX::XMFLOAT4 color = { 0.8f, 0.8f, 0.8f, 1.0f };

	void Initialize(const std::shared_ptr<FbxModelResource>& resource);

	// ‘O‚ÌƒAƒjƒ[ƒVƒ‡ƒ“‚ÌƒL[ƒtƒŒ[ƒ€‚ğZo
	// brief : ƒAƒjƒ[ƒVƒ‡ƒ“‚ªØ‚è‘Ö‚í‚é‚Ég—p
	[[nodiscard]] int CalcPrevKeyframe(int key_index);

	// s—ñŒvZ
	void CalculateLocalTransform();
	void CalculateWorldTransform(const DirectX::XMFLOAT4X4& world_transform);

	void BlendNextAnimation(float elapsedTime);
	void UpdateAnimation(float elapsedTime);
};

CEREAL_REGISTER_TYPE(FbxModelComponent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, FbxModelComponent)