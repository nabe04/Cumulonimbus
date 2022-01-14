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
	class TimeScale final
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

		void SetScale(const float s)
		{
			scale = s;
		}

	private:
		float scale{ 1.0f };
	};
} // cumulonimbus::system

CEREAL_CLASS_VERSION(cumulonimbus::system::TimeScale, 0)
