#pragma once
#include "component_base.h"
#include "event.h"

namespace cumulonimbus::component
{
	class ColliderMessageSenderComponent;
} // cumulonimbus::component

namespace cumulonimbus::component
{
	class ColliderMessageReceiverComponent final : public ComponentBase
	{
	public:
		using ComponentBase::ComponentBase;

		explicit ColliderMessageReceiverComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit ColliderMessageReceiverComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const ColliderMessageReceiverComponent& copy_comp);
		explicit ColliderMessageReceiverComponent() = default; // for cereal
		~ColliderMessageReceiverComponent() override = default;

		template<class Archive>
		void load(Archive && archive, uint32_t version);

		template<class Archive>
		void save(Archive && archive, uint32_t version) const;

		void Load(ecs::Registry * registry) override;

		void GameUpdate(float dt) override;

		void RegisterReceivedMessage(mapping::rename_type::Entity ent, std::function<void(ColliderMessageSenderComponent&)> callback);

		void Receive(ColliderMessageSenderComponent& sender);

	private:
		bool is_received{ false };
		system::Event<void, ColliderMessageSenderComponent&> on_received_message_event{};
	};
} // cumulonimbus::component
