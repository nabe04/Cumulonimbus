#pragma once

#include <cassert>
#include <type_traits>
#include <functional>

#include "input_system.h"
#include "resource_manager.h"
#include "window.h"

class Locator final
{
public:
	static void Initialize()
	{
		input = nullptr;
		resource_manager = nullptr;
		window = nullptr;
	}

	static InputSystem& GetInput() { return *input; }
	static ResourceManager& GetResourceManager() { return *resource_manager; }
	static Window& GetWindow() { return *window; }

	template<class T>
	static void Provide(T* s)
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
		else
		{
			static_assert(false, "Don't have type (class::Locator)");
		}

		assert(!"Don't have type (class::Locator)");
	}

private:
	inline static InputSystem* input;
	inline static ResourceManager* resource_manager;
	inline static Window* window;
};