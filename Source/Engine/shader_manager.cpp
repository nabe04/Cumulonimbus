#include "shader_manager.h"

#include <imgui.h>

#include "scene.h"

// Shaders
// 3D
#include "3d_standard.h"
#include "sample_shader.h"
#include "diffuse.h"
#include "phong.h"
#include "metal.h"
#include "toon.h"
#include "reflection_mapping.h"
#include "refraction_mapping.h"
#include "single_color.h"
// 2D
#include "standard_sprite.h"
#include "billboard.h"
// Other
#include "debug_collision.h"

namespace shader
{
	//***************************************
	//
	// ShaderManager Class
	//
	//***************************************
	ShaderManager::ShaderManager(ID3D11Device* device)
	{
		//-- Shader 3D --//
		standard_3d = std::make_unique<Standard3D>(device);
		diffuse = std::make_unique<Diffuse>(device);
		phong = std::make_unique<Phong>(device);
		metal = std::make_unique<Metal>(device);
		toon = std::make_unique<Toon>(device);
		reflaction_mapping = std::make_unique<ReflectionMapping>(device);
		refraction_mapping = std::make_unique<RefractionMapping>(device);
		single_color = std::make_unique<SingleColor>(device);

		{//-- Add shader state 3d(active) --//
			active_shader_state_3d.AddState
			(
				MeshShaderTypes::Standard,
				[this](ID3D11DeviceContext* immediate_context) { this->standard_3d->Activate(immediate_context); }
			);

			active_shader_state_3d.AddState
			(
				MeshShaderTypes::Diffuse,
				[this](ID3D11DeviceContext* immediate_context) { this->diffuse->Activate(immediate_context); }
			);

			active_shader_state_3d.AddState
			(
				MeshShaderTypes::Phong,
				[this](ID3D11DeviceContext* immediate_context) { this->phong->Activate(immediate_context); }
			);

			active_shader_state_3d.AddState
			(
				MeshShaderTypes::Metal,
				[this](ID3D11DeviceContext* immediate_context) {this->metal->Activate(immediate_context); }
			);

			active_shader_state_3d.AddState
			(
				MeshShaderTypes::Toon,
				[this](ID3D11DeviceContext* immediate_context) {this->toon->Activate(immediate_context); }
			);

			active_shader_state_3d.AddState
			(
				MeshShaderTypes::ReflectionMapping,
				[this](ID3D11DeviceContext* immediate_context) {this->reflaction_mapping->Activate(immediate_context); }
			);

			active_shader_state_3d.AddState
			(
				MeshShaderTypes::RefractionMapping,
				[this](ID3D11DeviceContext* immediate_context) {this->refraction_mapping->Activate(immediate_context); }
			);

			active_shader_state_3d.AddState
			(
				MeshShaderTypes::SingleColor,
				[this](ID3D11DeviceContext* immediate_context) {this->single_color->Activate(immediate_context); }
			);
		}

		{//-- Add shader state 3d(deactivate) --//
			deactive_shader_state_3d.AddState
			(
				MeshShaderTypes::Standard,
				[this](ID3D11DeviceContext* immediate_context) { this->standard_3d->Deactivate(immediate_context); }
			);

			deactive_shader_state_3d.AddState
			(
				MeshShaderTypes::Diffuse,
				[this](ID3D11DeviceContext* immediate_context) { this->diffuse->Deactivate(immediate_context); }
			);

			deactive_shader_state_3d.AddState
			(
				MeshShaderTypes::Phong,
				[this](ID3D11DeviceContext* immediate_context) { this->phong->Deactivate(immediate_context); }
			);

			deactive_shader_state_3d.AddState
			(
				MeshShaderTypes::Metal,
				[this](ID3D11DeviceContext* immediate_context) { this->metal->Deactivate(immediate_context); }
			);

			deactive_shader_state_3d.AddState
			(
				MeshShaderTypes::Toon,
				[this](ID3D11DeviceContext* immediate_context) {this->toon->Deactivate(immediate_context); }
			);

			deactive_shader_state_3d.AddState
			(
				MeshShaderTypes::ReflectionMapping,
				[this](ID3D11DeviceContext* immediate_context) {this->reflaction_mapping->Deactivate(immediate_context); }
			);

			deactive_shader_state_3d.AddState
			(
				MeshShaderTypes::RefractionMapping,
				[this](ID3D11DeviceContext* immediate_context) {this->refraction_mapping->Deactivate(immediate_context); }
			);

			deactive_shader_state_3d.AddState
			(
				MeshShaderTypes::SingleColor,
				[this](ID3D11DeviceContext* immediate_context) {this->single_color->Deactivate(immediate_context); }
			);
		}

		{//-- Add Constant buffer state(3d) --//
			constant_buffer_state_3d.AddState
			(
				MeshShaderTypes::Standard,
				[](const MeshShaderState& state) {}
			);

			constant_buffer_state_3d.AddState
			(
				MeshShaderTypes::Diffuse,
				[](const MeshShaderState& state) {}
			);

			constant_buffer_state_3d.AddState
			(
				MeshShaderTypes::Phong,
				[this](const MeshShaderState& state) {this->phong->GetMaterial()->SetData(state.m_phong); }
			);

			constant_buffer_state_3d.AddState
			(
				MeshShaderTypes::Metal,
				[this](const MeshShaderState& state) {this->metal->GetMaterial()->SetData(state.m_metal); }
			);

			constant_buffer_state_3d.AddState
			(
				MeshShaderTypes::Toon,
				[this](const MeshShaderState& state) {this->toon->GetMaterial()->SetData(state.m_toon); }
			);

			constant_buffer_state_3d.AddState
			(
				MeshShaderTypes::ReflectionMapping,
				[](const MeshShaderState& state) {}
			);

			constant_buffer_state_3d.AddState
			(
				MeshShaderTypes::RefractionMapping,
				[this](const MeshShaderState& state) {this->refraction_mapping->GetMaterial()->SetData(state.m_refraction); }
			);

			constant_buffer_state_3d.AddState
			(
				MeshShaderTypes::SingleColor,
				[this](const MeshShaderState& state) {this->single_color->GetMaterial()->SetData(state.m_single_color); }
			);
		}

		//-- Shader 2D --//
		texture_2d = std::make_unique<Texture2DShader>(device);

		{//-- Add shader state 2d(active) --//
			active_shader_state_2d.AddState
			(
				SpriteShaderTypes::Standard,
				[this](ID3D11DeviceContext* immediate_context) {this->texture_2d->Activate(immediate_context); }
			);
		}

		{//-- Add shader state 2d(deactive) --//
			deactive_shader_state_2d.AddState
			(
				SpriteShaderTypes::Standard,
				[this](ID3D11DeviceContext* immediate_context) {this->texture_2d->Deactivate(immediate_context); }
			);
		}
	}


	void ShaderManager::Activate(ID3D11DeviceContext* immediate_context,const MeshShaderState& state)
	{
		current_state_3d = state.GetShaderState();

		constant_buffer_state_3d.SetState(state.GetShaderState());
		constant_buffer_state_3d.Update(state);

		active_shader_state_3d.SetState(state.GetShaderState());
		active_shader_state_3d.Update(immediate_context);

		//Todo : 別のRenderTargetに書き込む処理(実行)
	}

	void ShaderManager::Deactivate(ID3D11DeviceContext* immediate_context, const MeshShaderState& state)
	{
		deactive_shader_state_3d.SetState(state.GetShaderState());
		deactive_shader_state_3d.Update(immediate_context);

		//Todo : 別のRenderTargetに書き込む処理(非アクティブ化)
	}

	void ShaderManager::ActivateSingleColor(ID3D11DeviceContext* immediate_context) const
	{
		single_color->Activate(immediate_context);
	}

	void ShaderManager::DeactivateSingleColor(ID3D11DeviceContext* immediate_context) const
	{
		single_color->Deactivate(immediate_context);
	}

	void ShaderManager::Activate(ID3D11DeviceContext* immediate_context,const SpriteShaderState& state)
	{
		current_state_2d = state.GetShaderState();

		active_shader_state_2d.SetState(state.GetShaderState());
		active_shader_state_2d.Update(immediate_context);
	}

	void ShaderManager::Deactivate(ID3D11DeviceContext* immediate_context, const SpriteShaderState& state)
	{
		deactive_shader_state_2d.SetState(state.GetShaderState());
		deactive_shader_state_2d.Update(immediate_context);
	}

	//********************************************
	//
	// MeshShaderState Class
	//
	//********************************************
	MeshShaderState::MeshShaderState()
	{
		shader_type.SetState(MeshShaderTypes::Diffuse);

		shader_states.AddState(MeshShaderTypes::Standard	, [] {});
		shader_states.AddState(MeshShaderTypes::Diffuse		, [] {});
		shader_states.AddState(MeshShaderTypes::Phong		, [this]() {m_phong.EditParam();});
		shader_states.AddState(MeshShaderTypes::Metal		, [this]() {m_metal.EditParam(); });
		shader_states.AddState(MeshShaderTypes::Toon		, [this]() {m_toon.EditParam(); });
		shader_states.AddState(MeshShaderTypes::ReflectionMapping, []() {});
		shader_states.AddState(MeshShaderTypes::RefractionMapping, [this]() {m_refraction.EditParam(); });
		shader_states.AddState(MeshShaderTypes::SingleColor	, [this]() {m_single_color.EditParam(); });
	}

	void MeshShaderState::RenderImGui()
	{
		if (ImGui::TreeNode("Shader Parameter"))
		{
			item_current = static_cast<int>(shader_type.GetCurrentState()); // To correspond to the Shader that the current Component has
			ImGui::Combo("Shader Type", &item_current, items, IM_ARRAYSIZE(items));
			SetShaderState(static_cast<MeshShaderTypes>(item_current));

			EditShaderParam(shader_type.GetCurrentState());

			ImGui::TreePop();
		}
	}

	void MeshShaderState::EditShaderParam(MeshShaderTypes type)
	{
		shader_states.SetState(type);
		shader_states.Update();
	}


	//-- SpriteShaderManager class--//
	SpriteShaderManager::SpriteShaderManager(ID3D11Device* device)
	{
		standard_sprite = std::make_unique<StandardSprite>(device);

		activate_shader_state.AddState
		(
			SpriteShaderTypes::Standard,
			[this](ID3D11DeviceContext* immediate_context) { this->standard_sprite->Activate(immediate_context); }
		);

		deactivate_shader_state.AddState
		(
			SpriteShaderTypes::Standard,
			[this](ID3D11DeviceContext* immediate_context) { this->standard_sprite->Deactivate(immediate_context); }
		);
	}

	void SpriteShaderManager::Activate(ID3D11DeviceContext* immediate_context, const shader::SpriteShaderTypes& state)
	{
		activate_shader_state.SetState(state);
		activate_shader_state.Update(immediate_context);
	}

	void SpriteShaderManager::Deactivate(ID3D11DeviceContext* immediate_context, const shader::SpriteShaderTypes& state)
	{
		deactivate_shader_state.SetState(state);
		deactivate_shader_state.Update(immediate_context);
	}
}


namespace cumulonimbus::shader_system
{	
	ShaderManager::ShaderManager()
	{
		using namespace mapping::shader_assets;
		// Shaderのコンパイル
		// 3D
		RegistryShader<shader_system::SampleShader>				(ShaderAsset3D::SampleShader);
		RegistryShader<shader_system::DiffuseShader>			(ShaderAsset3D::Diffuse);
		RegistryShader<shader_system::PhongShader>				(ShaderAsset3D::Phong);
		RegistryShader<shader_system::MetalShader>				(ShaderAsset3D::Metal);
		RegistryShader<shader_system::ToonShader>				(ShaderAsset3D::Toon);
		RegistryShader<shader_system::ReflectionMappingShader>	(ShaderAsset3D::ReflectionMapping);
		RegistryShader<shader_system::RefractionMappingShader>	(ShaderAsset3D::RefractionMapping);
		RegistryShader<shader_system::SingleColorShader>		(ShaderAsset3D::SingleColor);
		// 2D
		RegistryShader2D<shader_system::StandardSpriteShader, shader_asset::StandardSpriteAsset>(ShaderAsset2D::Standard);
		RegistryShader2D<shader_system::BillboardShader		, shader_asset::BillboardAsset>		(ShaderAsset2D::Billboard);
		// その他のシェーダー(Local Shader)
		RegisterShader<shader_system::DebugCollisionShader>     (LocalShaderAsset::Collision);
	}

	//-------------------  3D用シェーダーのBind,Unbind  ----------------------//
	void ShaderManager::BindShader(const mapping::shader_assets::ShaderAsset3D asset)
	{
		shader3d_map.at(asset)->BindShader();
		//shader3d_map.at(mapping::shader_assets::ShaderAsset3D::SampleShader)->BindShader();
	}

	void ShaderManager::UnbindShader(const mapping::shader_assets::ShaderAsset3D asset)
	{
		shader3d_map.at(asset)->UnbindShader();
	}

	//-------------------  2D用シェーダーのBind,Unbind  ----------------------//
	mapping::shader_assets::ShaderAsset2D ShaderManager::GetAsset2DFromConnector(const mapping::rename_type::Hash& hash) const
	{
		if (!connector_shader2d.contains(hash))
			assert(!"Don't have key value");

		return connector_shader2d.at(hash);
	}


	void ShaderManager::BindShader2D(const mapping::shader_assets::ShaderAsset2D asset)
	{
		if (!shader2d_map.contains(asset))
			assert(!"Don't have shader asset(ShaderManager::BindShader2D)");

		shader2d_map.at(asset)->BindShader();
	}

	void ShaderManager::UnbindShader2D(const mapping::shader_assets::ShaderAsset2D asset)
	{
		if (!shader2d_map.contains(asset))
			assert(!"Don't have shader asset(ShaderManager::BindShader2D)");

		shader2d_map.at(asset)->UnbindShader();
	}

	//------------------ モデル以外のシェーダーのBind,Unbind -----------------//
	void ShaderManager::BindLocalShader(mapping::shader_assets::LocalShaderAsset asset)
	{
		local_shader_map.at(asset)->BindShader();
	}

	void ShaderManager::UnbindLocalShader(mapping::shader_assets::LocalShaderAsset asset)
	{
		local_shader_map.at(asset)->UnbindShader();
	}
} // cumulonimbus::shader_system
