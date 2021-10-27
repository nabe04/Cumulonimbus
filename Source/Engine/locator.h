#pragma once

#include <cassert>
#include <memory>
#include <type_traits>

#include "asset_manager.h"
#include "dx11_configurator.h"
#include "input_system.h"
#include "resource_manager.h"
#include "system.h"
#include "texture_resource_manager.h"
#include "window.h"

namespace cumulonimbus::locator
{
	class Locator final
	{
	public:
		static void Initialize()
		{
			input					 = nullptr;
			window					 = nullptr;
			system					 = nullptr;
			asset_manager			 = nullptr;
			resource_manager		 = nullptr;
			dx11_configurator		 = nullptr;
			texture_resource_manager = nullptr;
		}
		static void UnInitialize()
		{
			input.reset();
			window.reset();
			system.reset();
			resource_manager.reset();
			dx11_configurator.reset();
			texture_resource_manager.reset();
		}
		~Locator() = default;

		static Window*			GetWindow()				{ return window.get(); }
		static Dx11Device*		GetDx11Device()			{ return dx11_configurator.get(); }
		static InputSystem*		GetInput()				{ return input.get(); }
		static system::System*  GetSystem()				{ return system.get();	 }
		static ResourceManager* GetResourceManager()	{ return resource_manager.get(); }
		static asset::AssetManager* GetAssetManager()	{ return asset_manager.get(); }
		static manager::texture::TextureResourceManager* GetTextureResourceManager() { return texture_resource_manager.get(); }

		template<class T>
		static void Provide(const std::shared_ptr<T>& s)
		{
			if constexpr (std::is_same<T, Window>::value)
			{
				window = s;
				return;
			}
			else if constexpr (std::is_same<T, Dx11Device>::value)
			{
				dx11_configurator = s;
				return;
			}
			else if constexpr (std::is_same<T, InputSystem>::value)
			{
				input = s;
				return;
			}
			else if constexpr (std::is_same<T,system::System>::value)
			{
				system = s;
				return;
			}
			else if constexpr (std::is_same<T, ResourceManager>::value)
			{
				resource_manager = s;
				return;
			}
			else if constexpr(std::is_same<T,asset::AssetManager>::value)
			{
				asset_manager = s;
				return;
			}
			else if constexpr (std::is_same<T,manager::texture::TextureResourceManager>::value)
			{
				texture_resource_manager = s;
				return;
			}
			else
			{
				static_assert(false, "Don't have type (class::Locator)");
			}

			assert(!"Don't have type (class::Locator)");
		}

	private:
		inline static std::shared_ptr<Window>				window{};
		inline static std::shared_ptr<Dx11Device>			dx11_configurator{};
		inline static std::shared_ptr<InputSystem>			input{};
		inline static std::shared_ptr<system::System>		system{};
		inline static std::shared_ptr<ResourceManager>		resource_manager{};
		inline static std::shared_ptr<asset::AssetManager>	asset_manager{};
		inline static std::shared_ptr<manager::texture::TextureResourceManager> texture_resource_manager{ nullptr };
	};
} // cumulonimbus::locator