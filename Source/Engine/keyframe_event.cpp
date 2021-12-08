#include "keyframe_event.h"

#include "model_component.h"

namespace cumulonimbus::system
{
	template <class Archive>
	void KeyframeEvent::KeyEvent::load(Archive&& archive, uint32_t version)
	{
	}

	template <class Archive>
	void KeyframeEvent::KeyEvent::save(Archive&& archive, uint32_t version) const
	{
	}

	template <class Archive>
	void KeyframeEvent::load(Archive&& archive, uint32_t version)
	{
	}

	template <class Archive>
	void KeyframeEvent::save(Archive&& archive, uint32_t version) const
	{
	}

	void KeyframeEvent::Update(
		ecs::Registry* registry,
		const mapping::rename_type::Entity& ent,
		const std::string& key_name)
	{
		if (!key_events.contains(key_name))
			return;

		if (component::ModelComponent* model_comp = registry->TryGetComponent<component::ModelComponent>(ent);
			model_comp)
		{
			auto& key_event = key_events.at(key_name);

			if (const auto current_keyframe = static_cast<u_int>(model_comp->CurrentKeyframe());
				current_keyframe >= key_event.min &&
				current_keyframe <= key_event.max)
			{
				key_event.is_in_range = true;
			}
			else
			{
				key_event.is_in_range = false;
			}

			if(key_event.is_in_range)
			{
				if(key_event.is_old_in_range)
				{//
					key_event.key_state = KeyState::OnKeyRangeStay;
				}
				else
				{
					key_event.key_state = KeyState::OnKeyRangeEnter;
				}
			}
			else
			{
				if(key_event.is_old_in_range)
				{
					key_event.key_state = KeyState::OnKeyRangeExit;
				}
				else
				{
					key_event.key_state = KeyState::None;
				}
			}

			key_event.is_old_in_range = key_event.is_in_range;
		}
	}

	void KeyframeEvent::RegistryEvent(const std::string& key_name, const KeyEvent& key_event)
	{
		if(key_events.contains(key_name))
		{
			key_events.at(key_name) = key_event;
			return;
		}

		key_events.emplace(key_name, key_event);
	}

	void KeyframeEvent::RegistryEvent(
		const std::string& key_name,
		const u_int min, const u_int max)
	{
		if(key_events.contains(key_name))
		{
			auto& key_event = key_events.at(key_name);
			key_event.min = min;
			key_event.max = max;
			return;
		}

		KeyEvent key_event{};
		key_event.min = min;
		key_event.max = max;

		key_events.emplace(key_name, key_event);
	}

	KeyframeEvent::KeyEvent& KeyframeEvent::GetKeyEvent(const std::string& key_name)
	{
		if (!key_events.contains(key_name))
			assert(!"Don't have key event(KeyframeEvent::GetKeyEvent)");

		return key_events.at(key_name);
	}

	void KeyframeEvent::SetKeyEvent(const std::string& key_name, u_int min, u_int max)
	{
		if (!key_events.contains(key_name))
			assert(!"Don't have key event(KeyframeEvent::GetKeyEvent)");

		auto& key_event = key_events.at(key_name);
		key_event.min = min;
		key_event.max = max;
	}

} // cumulonimbus::system