#pragma once

#include <memory>

#include <d3d11.h>

#include <cereal/cereal.hpp>

#include "shader.h"
#include "state_machine.h"

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
	private:
		ShaderManager() = default;
		~ShaderManager() = default;

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

		StateMachine<MeshShaderTypes,   void, ID3D11DeviceContext*> active_shader_state_3d{};
		StateMachine<MeshShaderTypes,   void, ID3D11DeviceContext*> deactive_shader_state_3d{};
		StateMachine<SpriteShaderTypes, void, ID3D11DeviceContext*> active_shader_state_2d{};
		StateMachine<SpriteShaderTypes, void, ID3D11DeviceContext*> deactive_shader_state_2d{};

		StateMachine<MeshShaderTypes, void, const std::shared_ptr<const MeshShaderState>&> constant_buffer_state_3d{};
	public:
		// Singleton used
		static ShaderManager* GetInstance()
		{
			static ShaderManager instance{};
			return &instance;
		}

		void Initialize(ID3D11Device* device);

		/*
		* brief       : Setting shader (3D)
		* MeshShaderState : It's got information on each shader
		*/
		void Activate(ID3D11DeviceContext* immediate_context, const std::shared_ptr<const MeshShaderState>& state);
		void Deactivate(ID3D11DeviceContext* immediate_context, const std::shared_ptr<const MeshShaderState>& state);

		void ActiveteSingleColor(ID3D11DeviceContext* immediate_context);
		void DeactivateSingleColor(ID3D11DeviceContext* immediate_context);

		/*
		* brief       : Setting shader (2D)
		* MeshShaderState : It's got information on each shader
		*/
		void Activate(ID3D11DeviceContext* immediate_context, const std::shared_ptr<const SpriteShaderState>& state);
		void Deactivate(ID3D11DeviceContext* immediate_context, const std::shared_ptr<const SpriteShaderState>& state);
	};

	//---------------------------------------
	//
	// MeshShaderState Class
	// shader_type
	//
	//---------------------------------------
	class MeshShaderState final
	{
	private:
		MeshShaderTypes shader_type = MeshShaderTypes::Diffuse;

		//-- Imgui --//
		inline static const char* items[] = { "Standard","Diffuse","Phong","Metal","Toon","ReflactionMapping","RefractionMapping","SingleColor" };
		int item_current = 0;

		StateMachine<MeshShaderTypes> shader_states{};

	public:
		//-- Shader Types --//
		M_Phong				m_phong{};
		M_Metal				m_metal{};
		M_Toon				m_toon{};
		M_SpotLight			m_spot_light{};
		M_Refraction		m_refraction{};
		M_SingleColor		m_single_color{};

	public:
		MeshShaderState();
		~MeshShaderState() = default;

		void SetShaderState(MeshShaderTypes type) { shader_type = type; }
		MeshShaderTypes GetShaderState() const { return shader_type; }


		//-- ImGui --//
		void RenderImgui();
		void EditShaderParam(MeshShaderTypes type);
	};

	class SpriteShaderManager final
	{
	private:
		std::unique_ptr<StandardSprite> standard_sprite = nullptr;

		StateMachine<SpriteShaderTypes, void, ID3D11DeviceContext*> activate_shader_state{};
		StateMachine<SpriteShaderTypes, void, ID3D11DeviceContext*> deactivate_shader_state{};

	public:
		explicit SpriteShaderManager(ID3D11Device* device);
		~SpriteShaderManager() = default;

		void Activate(ID3D11DeviceContext* immediate_context, const shader::SpriteShaderTypes& state);
		void Deactivate(ID3D11DeviceContext* immediate_context, const shader::SpriteShaderTypes& state);
	};

	class SpriteShaderState final
	{
	private:
		SpriteShaderTypes shader_type = SpriteShaderTypes::Standard;

	public:
		SpriteShaderState()  = default;
		~SpriteShaderState() = default;

		void SetShaderState(SpriteShaderTypes type) { shader_type = type; }
		const SpriteShaderTypes& GetShaderState() const { return shader_type; }

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(shader_type)
			);
		}
	};
}
