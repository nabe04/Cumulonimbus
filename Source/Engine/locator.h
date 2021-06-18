#pragma once

#include <cassert>
#include <type_traits>
#include <memory>

#include "input_system.h"
#include "resource_manager.h"
#include "window.h"
#include "dx11_configurator.h"

class Locator final
{
public:
	static void Initialize()
	{
		input			 = nullptr;
		resource_manager = nullptr;
		window			 = nullptr;
	}
	~Locator() = default;

	static InputSystem* GetInput() { return input.get(); }
	static ResourceManager* GetResourceManager() { return resource_manager.get(); }
	static Window* GetWindow() { return window.get(); }
	static Dx11Device* GetDx11Device() { return dx11_configurator.get(); }

	template<class T>
	static void Provide(const std::shared_ptr<T>& s)
	{
		if constexpr (std::is_same<T,InputSystem>::value)
		{
			input = s;
			return;
		}
		else if constexpr (std::is_same<T, ResourceManager>::value)
		{
			resource_manager = s;
			return;
		}
		else if constexpr (std::is_same<T, Window>::value)
		{
			window = s;
			return;
		}
		else if constexpr (std::is_same<T,Dx11Device>::value)
		{
			dx11_configurator = s;
			return;
		}
		else
		{
			static_assert(false, "Don't have type (class::Locator)");
		}

		assert(!"Don't have type (class::Locator)");
	}

private:
	inline static std::shared_ptr<InputSystem> input{};
	inline static std::shared_ptr<ResourceManager> resource_manager{};
	inline static std::shared_ptr<Window> window{};
	inline static std::shared_ptr<Dx11Device> dx11_configurator{};
};