#pragma once
#include "rename_type_mapping.h"

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	} // ecs
} // cumulonimbus

namespace cumulonimbus::editor
{
	class Inspector final
	{
	public:
		explicit Inspector() = default;
		~Inspector()		 = default;

		/**
		 * @brief			: �C���X�y�N�^�[�r���[�̕`��
		 * @param registry	: ���W�X�g���[
		 * @param ent		: �C���X�y�N�^�[��\���������G���e�B�e�B
		 */
		void Render(ecs::Registry* registry, mapping::rename_type::Entity ent);
	};
} // cumulonimbus::editor
