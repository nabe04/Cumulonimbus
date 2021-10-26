#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include <string>

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	} // ecs
} // cumulonimbus

namespace cumulonimbus::asset
{
	class Wave final
	{
	public:
		//struct SpawnData
		//{
		//	DirectX::SimpleMath::Vector3 spawn_pos{};
		//	DirectX::SimpleMath::Vector3 spawn_
		//};

	private:
		std::string wave_name{};
	};
} // cumulonimbus::asset