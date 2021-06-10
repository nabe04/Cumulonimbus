#pragma once

#include <iostream>
#include <ostream>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>

#include "ecs.h"

namespace cumulonimbus::component
{
	class ComponentBase
	{
	public:
		ComponentBase() = default;
		ComponentBase(ecs::Registry* registry, ecs::Entity ent)
			: registry{ registry }
			, entity{ ent }
		{
		}

		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(entity)
			);
		}

	protected:
		[[nodiscard]] ecs::Registry* GetRegistry() const { return registry; }
		[[nodiscard]] ecs::Entity    GetEntity()   const { return entity; }

	private:
		ecs::Registry* registry{};
		ecs::Entity entity;
	};

	class TransformComponent2 : public ComponentBase
	{
	public:
		TransformComponent2() = default;
		TransformComponent2(ecs::Registry* registry, ecs::Entity ent,
			float x, float y, float z)
			: ComponentBase(registry, ent)
			, x{ x }
			, y{ y }
			, z{ z }
		{
		}

		float x{}, y{}, z{};

		template<typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(x),
				CEREAL_NVP(y),
				CEREAL_NVP(z)
			);
		}
	};

	class MeshComponent : public ComponentBase
	{
	public:
		MeshComponent() = default;
		MeshComponent(ecs::Registry* registry, ecs::Entity ent, const std::string& name, int i_no, float f_no)
			: ComponentBase(registry, ent)
			, mesh_name{ name }
			, i_mesh_no{ i_no }
			, f_mesh_no{ f_no }
		{
		}

		void PrintMesh() const
		{
			auto* t = GetRegistry()->TryGetComponent<TransformComponent2>(GetEntity());

			std::cout << t->x << std::endl;
			std::cout << t->y << std::endl;
			std::cout << t->z << std::endl;
		}

		template <typename Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(mesh_name),
				CEREAL_NVP(i_mesh_no),
				CEREAL_NVP(f_mesh_no)
			);
		}

	private:
		std::string mesh_name;
		int i_mesh_no;
		float f_mesh_no;

	};

	class SpriteComponent : public ComponentBase
	{
	public:
		SpriteComponent() = default;
		SpriteComponent(ecs::Registry* registry, ecs::Entity ent, int w, int h, bool f)
			:ComponentBase(registry, ent)
			, width{ w }
			, height{ h }
			, flg{ f }{}

	private:
		int width{};
		int height{};
		bool flg = false;
	};
}