#pragma once

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	}// ecs
} // cumulonimbus

namespace cumulonimbus::system
{
	class Time final
	{
	public:

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void RenderImGui(ecs::Registry* registry);

		[[nodiscard]]
		float GetScale() const
		{
			return scale;
		}

		[[nodiscard]]
		float GetUnscaledDeltaTime() const
		{
			return unscaled_delta_time;
		}

		void SetScale(const float s)
		{
			scale = s;
		}

		void SetUnscaledDeltaTime(const float dt)
		{
			unscaled_delta_time = dt;
		}

	private:
		float scale{ 1.0f };
		float unscaled_delta_time{};
	};
} // cumulonimbus::system

CEREAL_CLASS_VERSION(cumulonimbus::system::Time, 0)
