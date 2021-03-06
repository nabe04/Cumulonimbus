#pragma once
#include <memory>
#include <unordered_map>

#include <d3d11.h>
#include <cereal/cereal.hpp>

#include "enum_state_map.h"
#include "gbuffer.h"
#include "generic.h"
#include "rename_type_mapping.h"
#include "shader.h"
#include "shader_asset_mapping.h"
#include "state_machine.h"

//-- Shader 3D --//
#include "3d_standard.h"
#include "diffuse.h"
#include "metal.h"
#include "phong.h"
#include "reflection_mapping.h"
#include "refraction_mapping.h"
#include "single_color.h"
#include "spot_light.h"
#include "toon.h"

//-- Shader 2D --//
#include "standard_sprite.h"
#include "texture2d_shader.h"


//namespace shader
//{
//	// Prototype declaration
//	class MeshShaderState;
//	class SpriteShaderState;
//
//	//---------------------------------------
//	//
//	// ShaderManager Class
//	//
//	//---------------------------------------
//	class ShaderManager final
//	{
//	public:
//		explicit ShaderManager(ID3D11Device* device);
//		~ShaderManager() = default;
//
//		/*
//		* brief       : Setting shader (3D)
//		* MeshShaderState : It's got information on each shader
//		*/
//		void Activate(ID3D11DeviceContext* immediate_context, const MeshShaderState& state);
//		void Deactivate(ID3D11DeviceContext* immediate_context, const MeshShaderState& state);
//
//		void ActivateSingleColor(ID3D11DeviceContext* immediate_context) const;
//		void DeactivateSingleColor(ID3D11DeviceContext* immediate_context) const;
//
//		/*
//		* brief       : Setting shader (2D)
//		* MeshShaderState : It's got information on each shader
//		*/
//		void Activate(ID3D11DeviceContext* immediate_context, const SpriteShaderState& state);
//		void Deactivate(ID3D11DeviceContext* immediate_context, const SpriteShaderState& state);
//
//	private:
//		//-- 3D --//
//		std::unique_ptr<Standard3D>  standard_3d = nullptr;
//		std::unique_ptr<Diffuse>	 diffuse = nullptr;
//		std::unique_ptr<Phong>		 phong = nullptr;
//		std::unique_ptr<Metal>		 metal = nullptr;
//		std::unique_ptr<Toon>		 toon = nullptr;
//		std::unique_ptr<ReflectionMapping> reflaction_mapping = nullptr;
//		std::unique_ptr<RefractionMapping> refraction_mapping = nullptr;
//		std::unique_ptr<SingleColor> single_color = nullptr;
//
//		//-- 2D --//
//		std::unique_ptr<Texture2DShader> texture_2d = nullptr;
//
//		inline static MeshShaderTypes   current_state_3d = MeshShaderTypes::End;
//		inline static SpriteShaderTypes current_state_2d = SpriteShaderTypes::End;
//
//		StateMachine<MeshShaderTypes, void, ID3D11DeviceContext*> active_shader_state_3d{};
//		StateMachine<MeshShaderTypes, void, ID3D11DeviceContext*> deactive_shader_state_3d{};
//		StateMachine<SpriteShaderTypes, void, ID3D11DeviceContext*> active_shader_state_2d{};
//		StateMachine<SpriteShaderTypes, void, ID3D11DeviceContext*> deactive_shader_state_2d{};
//
//		StateMachine<MeshShaderTypes, void, const MeshShaderState&> constant_buffer_state_3d{};
//	};
//
//	//---------------------------------------
//	//
//	// MeshShaderState Class
//	// shader_type
//	//
//	//---------------------------------------
//	class MeshShaderState final
//	{
//	public:
//		//-- Shader Types --//
//		M_Phong				m_phong{};
//		M_Metal				m_metal{};
//		M_Toon				m_toon{};
//		M_Refraction		m_refraction{};
//		M_SingleColor		m_single_color{};
//
//		MeshShaderState();
//		~MeshShaderState() = default;
//
//		void SetShaderState(const MeshShaderTypes& type) { shader_type.SetState(type); }
//		[[nodiscard]] const MeshShaderTypes& GetShaderState() const { return shader_type.GetCurrentState(); }
//
//
//		//-- ImGui --//
//		void RenderImGui();
//		void EditShaderParam(MeshShaderTypes type);
//
//		// cereal
//		template<class Archive>
//		void serialize(Archive&& archive)
//		{
//			archive(
//				cereal::make_nvp("M_Phong"		, m_phong),
//				cereal::make_nvp("M_Metal"		, m_metal),
//				cereal::make_nvp("M_Toon"		, m_toon),
//				cereal::make_nvp("M_Refraction"	, m_refraction),
//				cereal::make_nvp("M_SingleColor", m_single_color),
//				cereal::make_nvp("shader type"	,shader_type),
//				cereal::make_nvp("item current"	,item_current),
//				cereal::make_nvp("shader state"	,shader_states)
//			);
//		}
//
//	private:
//		EnumStateMap<MeshShaderTypes> shader_type;
//		//MeshShaderTypes shader_type = MeshShaderTypes::Diffuse;
//
//		//-- ImGui --//
//		inline static const char* items[] = { "Standard","Diffuse","Phong","Metal","Toon","ReflactionMapping","RefractionMapping","SingleColor" };
//		int item_current = 0;
//
//		StateMachine<MeshShaderTypes> shader_states{};
//	};
//
//	class SpriteShaderManager final
//	{
//	public:
//		explicit SpriteShaderManager(ID3D11Device* device);
//		~SpriteShaderManager() = default;
//
//		void Activate(ID3D11DeviceContext* immediate_context, const shader::SpriteShaderTypes& state);
//		void Deactivate(ID3D11DeviceContext* immediate_context, const shader::SpriteShaderTypes& state);
//
//	private:
//		std::unique_ptr<StandardSprite> standard_sprite = nullptr;
//
//		StateMachine<SpriteShaderTypes, void, ID3D11DeviceContext*> activate_shader_state{};
//		StateMachine<SpriteShaderTypes, void, ID3D11DeviceContext*> deactivate_shader_state{};
//	};
//
//	class SpriteShaderState final
//	{
//	public:
//		explicit SpriteShaderState()  = default;
//		~SpriteShaderState() = default;
//
//		void SetShaderState(const SpriteShaderTypes& type) { shader_type = type; }
//		[[nodiscard]] const SpriteShaderTypes& GetShaderState() const { return shader_type; }
//
//		template<class Archive>
//		void serialize(Archive&& archive)
//		{
//			archive(
//				CEREAL_NVP(shader_type)
//			);
//		}
//
//	private:
//		SpriteShaderTypes shader_type = SpriteShaderTypes::Standard;
//	};
//}

namespace cumulonimbus
{
	// TODO : GBuffer?n????????RenderPath??????
	namespace shader_system
	{
		class ShaderManager final
		{
		public:
			explicit ShaderManager();
			~ShaderManager() = default;

			/**
			 * @brief : Material3DComponent???o?^??????????
			 *	 	    ???????}?e???A???????????Z?b?g????
			 *		    ?V?F?[?_?[????????
			 */
			void BindShader(mapping::shader_assets::ShaderAsset3D asset) const;
			void UnbindShader(mapping::shader_assets::ShaderAsset3D asset) const;

			/**
			 * @brief : Material2DComponent???o?^??????????
			 *		    ???????}?e???A???????????Z?b?g????
			 *		    ?V?F?[?_?[????????
			 */
			void BindShader2D(mapping::shader_assets::ShaderAsset2D asset);
			void UnbindShader2D(mapping::shader_assets::ShaderAsset2D asset);

			/**
			 * @brief : LocalShader?????????????????V?F?[?_?[???o?C???h
			 */
			void BindLocalShader(mapping::shader_assets::LocalShaderAsset asset);
			void UnbindLocalShader(mapping::shader_assets::LocalShaderAsset asset);

			/**
			 * @brief : ShaderAsset?N???X???^?n?b?V???l?????R?l?N?^?[??
			 *			?o?^??????????ShaderAsset2D???????o??
			 */
			[[nodiscard]]
			mapping::shader_assets::ShaderAsset2D GetAsset2DFromConnector(const mapping::rename_type::Hash& hash) const;

		private:
			// ???f?????g?p?????V?F?[?_?[???}?b?v(3D)
			std::unordered_map<mapping::shader_assets::ShaderAsset3D, std::unique_ptr<shader_system::Shader>> shader3d_map;
			// ?X?v???C?g???g?p?????V?F?[?_?[???}?b?v(2D)
			std::unordered_map<mapping::shader_assets::ShaderAsset2D, std::unique_ptr<shader_system::Shader>> shader2d_map;
			// ???f?????`?????O???V?F?[?_?[(?|?X?g?v???Z?X????)
			std::unordered_map<mapping::shader_assets::LocalShaderAsset  , std::unique_ptr<shader_system::Shader>> local_shader_map;
			// ShaderAsset(2D)?^??mapping::shader_assets::ShaderAsset2D?????A?t?????s??
			std::unordered_map<mapping::rename_type::Hash, mapping::shader_assets::ShaderAsset2D> connector_shader2d{};

			/*
			 * brief : ?????????V?F?[?_?[???o?^(3D)
			 */
			template<class T>
			void RegistryShader(mapping::shader_assets::ShaderAsset3D asset_type)
			{
				if (shader3d_map.contains(asset_type))
					return;

				shader3d_map.emplace(asset_type, std::make_unique<T>());
			}

			/**
			 * @brief : ?????????V?F?[?_?[???o?^(2D)
			 * @remark : ??????connector_shader2d
			 */
			template<class Shader, class ShaderAsset>
			void RegistryShader2D(const mapping::shader_assets::ShaderAsset2D asset_type)
			{
				// ??????????????????????????????????
				if (shader2d_map.contains(asset_type))
					return;

				const auto& hash = utility::GetHash<ShaderAsset>();
				shader2d_map.emplace(asset_type, std::make_unique<Shader>());
				connector_shader2d.emplace(hash, asset_type);
			}

			/**
			 * @brief :?????????V?F?[?_?[???o?^(LocalShaderAsset)
			 */
			template<class T>
			void RegisterShader(mapping::shader_assets::LocalShaderAsset asset_type)
			{
				if (local_shader_map.contains(asset_type))
					return;

				local_shader_map.emplace(asset_type, std::make_unique<T>());
			}
		};
	} // shader_system
} // cumulonimbus
