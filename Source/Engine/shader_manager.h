#pragma once

#include <memory>
#include <unordered_map>

#include <d3d11.h>
#include <cereal/cereal.hpp>

#include "shader.h"
#include "state_machine.h"
#include "enum_state_map.h"
#include "shader_asset_mapping.h"
#include "gbuffer.h"

//-- Shader 3D --//
#include "3d_standard.h"
#include "diffuse.h"
#include "phong.h"
#include "metal.h"
#include "toon.h"
#include "reflection_mapping.h"
#include "spot_light.h"
#include "refraction_mapping.h"
#include "single_color.h"

//-- Shader 2D --//
#include "texture2d_shader.h"
#include "standard_sprite.h"

class Scene;
//namespace cumulonimbus::shader_system
//{
//	class
//}

namespace shader
{
	// Prototype declaration
	class MeshShaderState;
	class SpriteShaderState;

	//---------------------------------------
	//
	// ShaderManager Class
	//
	//---------------------------------------
	class ShaderManager final
	{
	public:
		explicit ShaderManager(ID3D11Device* device);
		~ShaderManager() = default;

		/*
		* brief       : Setting shader (3D)
		* MeshShaderState : It's got information on each shader
		*/
		void Activate(ID3D11DeviceContext* immediate_context, const MeshShaderState& state);
		void Deactivate(ID3D11DeviceContext* immediate_context, const MeshShaderState& state);

		void ActivateSingleColor(ID3D11DeviceContext* immediate_context) const;
		void DeactivateSingleColor(ID3D11DeviceContext* immediate_context) const;

		/*
		* brief       : Setting shader (2D)
		* MeshShaderState : It's got information on each shader
		*/
		void Activate(ID3D11DeviceContext* immediate_context, const SpriteShaderState& state);
		void Deactivate(ID3D11DeviceContext* immediate_context, const SpriteShaderState& state);

	private:
		//-- 3D --//
		std::unique_ptr<Standard3D>  standard_3d = nullptr;
		std::unique_ptr<Diffuse>	 diffuse = nullptr;
		std::unique_ptr<Phong>		 phong = nullptr;
		std::unique_ptr<Metal>		 metal = nullptr;
		std::unique_ptr<Toon>		 toon = nullptr;
		std::unique_ptr<ReflectionMapping> reflaction_mapping = nullptr;
		std::unique_ptr<RefractionMapping> refraction_mapping = nullptr;
		std::unique_ptr<SingleColor> single_color = nullptr;

		//-- 2D --//
		std::unique_ptr<Texture2DShader> texture_2d = nullptr;

		inline static MeshShaderTypes   current_state_3d = MeshShaderTypes::End;
		inline static SpriteShaderTypes current_state_2d = SpriteShaderTypes::End;

		StateMachine<MeshShaderTypes, void, ID3D11DeviceContext*> active_shader_state_3d{};
		StateMachine<MeshShaderTypes, void, ID3D11DeviceContext*> deactive_shader_state_3d{};
		StateMachine<SpriteShaderTypes, void, ID3D11DeviceContext*> active_shader_state_2d{};
		StateMachine<SpriteShaderTypes, void, ID3D11DeviceContext*> deactive_shader_state_2d{};

		StateMachine<MeshShaderTypes, void, const MeshShaderState&> constant_buffer_state_3d{};
	};

	//---------------------------------------
	//
	// MeshShaderState Class
	// shader_type
	//
	//---------------------------------------
	class MeshShaderState final
	{
	public:
		//-- Shader Types --//
		M_Phong				m_phong{};
		M_Metal				m_metal{};
		M_Toon				m_toon{};
		M_Refraction		m_refraction{};
		M_SingleColor		m_single_color{};

		MeshShaderState();
		~MeshShaderState() = default;

		void SetShaderState(const MeshShaderTypes& type) { shader_type.SetState(type); }
		[[nodiscard]] const MeshShaderTypes& GetShaderState() const { return shader_type.GetCurrentState(); }


		//-- ImGui --//
		void RenderImGui();
		void EditShaderParam(MeshShaderTypes type);

		// cereal
		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				cereal::make_nvp("M_Phong"		, m_phong),
				cereal::make_nvp("M_Metal"		, m_metal),
				cereal::make_nvp("M_Toon"		, m_toon),
				cereal::make_nvp("M_Refraction"	, m_refraction),
				cereal::make_nvp("M_SingleColor", m_single_color),
				cereal::make_nvp("shader type"	,shader_type),
				cereal::make_nvp("item current"	,item_current),
				cereal::make_nvp("shader state"	,shader_states)
			);
		}

	private:
		EnumStateMap<MeshShaderTypes> shader_type;
		//MeshShaderTypes shader_type = MeshShaderTypes::Diffuse;

		//-- ImGui --//
		inline static const char* items[] = { "Standard","Diffuse","Phong","Metal","Toon","ReflactionMapping","RefractionMapping","SingleColor" };
		int item_current = 0;

		StateMachine<MeshShaderTypes> shader_states{};
	};

	class SpriteShaderManager final
	{
	public:
		explicit SpriteShaderManager(ID3D11Device* device);
		~SpriteShaderManager() = default;

		void Activate(ID3D11DeviceContext* immediate_context, const shader::SpriteShaderTypes& state);
		void Deactivate(ID3D11DeviceContext* immediate_context, const shader::SpriteShaderTypes& state);

	private:
		std::unique_ptr<StandardSprite> standard_sprite = nullptr;

		StateMachine<SpriteShaderTypes, void, ID3D11DeviceContext*> activate_shader_state{};
		StateMachine<SpriteShaderTypes, void, ID3D11DeviceContext*> deactivate_shader_state{};
	};

	class SpriteShaderState final
	{
	public:
		explicit SpriteShaderState()  = default;
		~SpriteShaderState() = default;

		void SetShaderState(const SpriteShaderTypes& type) { shader_type = type; }
		[[nodiscard]] const SpriteShaderTypes& GetShaderState() const { return shader_type; }

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(shader_type)
			);
		}

	private:
		SpriteShaderTypes shader_type = SpriteShaderTypes::Standard;
	};
}

namespace cumulonimbus
{
	// TODO : GBuffer系の処理をRenderPathに移す
	namespace shader_system
	{
		class ShaderManager final
		{
		public:
			explicit ShaderManager();
			~ShaderManager() = default;

			/*
			 * brief : Material3DComponentに登録されている
			 *		   現在のマテリアルに応じてセットする
			 *		   シェーダーを変える
			 */
			void BindShader(mapping::shader_assets::ShaderAsset3D asset);
			void UnbindShader(mapping::shader_assets::ShaderAsset3D asset);

			/*
			 * brief : Material2DComponentに登録されている
			 *		   現在のマテリアルに応じてセットする
			 *		   シェーダーを変える
			 */
			void BindShader(mapping::shader_assets::ShaderAsset2D asset);
			void UnbindShader(mapping::shader_assets::ShaderAsset2D asset);

			/*
			 * brief : LocalShaderの種類に応じてのシェーダーのバインド
			 */
			void BindLocalShader(mapping::shader_assets::LocalShaderAsset asset);
			void UnbindLocalShader(mapping::shader_assets::LocalShaderAsset asset);

		private:
			// モデルが使用するシェーダーのマップ(3D)
			std::unordered_map<mapping::shader_assets::ShaderAsset3D, std::unique_ptr<shader_system::Shader>> shader3d_map;
			// スプライトが使用するシェーダーのマップ(2D)
			std::unordered_map<mapping::shader_assets::ShaderAsset2D, std::unique_ptr<shader_system::Shader>> shader2d_map;
			// モデルの描画以外のシェーダー(ポストプロセスなど)
			std::unordered_map<mapping::shader_assets::LocalShaderAsset  , std::unique_ptr<shader_system::Shader>> local_shader_map;
			/*
			 * brief : 作成したシェーダーの登録(3D)
			 */
			template<typename T>
			void RegistryShader(mapping::shader_assets::ShaderAsset3D asset_type)
			{
				if (shader3d_map.contains(asset_type))
					return;

				shader3d_map.emplace(asset_type, std::make_unique<T>());
			}

			/*
			 * brief : 作成したシェーダーの登録(2D)
			 */
			template<typename T>
			void RegistryShader(mapping::shader_assets::ShaderAsset2D asset_type)
			{
				if (shader2d_map.contains(asset_type))
					return;

				shader2d_map.emplace(asset_type, std::make_unique<T>());
			}

			/*
			 * brief :作成したシェーダーの登録(LocalShaderAsset)
			 */
			template<typename T>
			void RegisterShader(mapping::shader_assets::LocalShaderAsset asset_type)
			{
				if (local_shader_map.contains(asset_type))
					return;

				local_shader_map.emplace(asset_type, std::make_unique<T>());
			}
		};
	} // shader_system
} // cumulonimbus
