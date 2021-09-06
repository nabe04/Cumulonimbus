#pragma once
#include <map>
#include <string>
#include <memory>

#include "rename_type_mapping.h"
#include "component_tag_mapping.h"
#include "component_base.h"

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
		explicit Inspector();
		~Inspector() = default;

		/**
		 * @brief			: �C���X�y�N�^�[�r���[�̕`��
		 * @param registry	: ���W�X�g���[
		 * @param ent		: �C���X�y�N�^�[��\���������G���e�B�e�B
		 */
		void Render(ecs::Registry* registry, mapping::rename_type::Entity ent);

	private:
		std::map< std::string/*�uAdd Component�v���ŕ\������閼�O*/
				, std::pair<std::string/*���ۂ̃R���|�[�l���g��*/, std::unique_ptr<component::ComponentBase>>> component_map;

		/**
		 * @brief : �uAdd Component�v�{�^���̎���
		 */
		void AddComponentButton(ecs::Registry* registry, mapping::rename_type::Entity ent);

		/**
		 * @brief : �uAdd Component�v���̃��j���[�J�e�S���̎���
		 * @param registry	: ���W�X�g���[
		 * @param ent		: ���ݑI�𒆂̃G���e�B�e�B
		 * @param menu_name	: ���j���[��
		 * @param tag		: ��r�p�^�O
		 */
		void ComponentMenu(
			ecs::Registry* registry		, mapping::rename_type::Entity ent,
			const std::string& menu_name, mapping::component_tag::ComponentTag tag);

		/**
		 * @brief			: �uAdd Component�v�{�^�����̃R���|�[�l���g�̓o�^
		 * @details			: �o�^���ꂽ�R���|�[�l���g�́uAdd Component�v�{�^�����Ŏ�������ǉ��ł���悤�ɂȂ�
		 * @details			: �L�[�l���d�������ꍇ��O�������N����
		 * @param comp_name	: component_map���̃L�[�l�Ƃ��Ďg�p
		 */
		template <typename T>
		void RegisterComponent(const std::string& comp_name, mapping::component_tag::ComponentTag tag);
	};
} // cumulonimbus::editor
