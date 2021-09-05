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
		 * @brief			: インスペクタービューの描画
		 * @param registry	: レジストリー
		 * @param ent		: インスペクターを表示したいエンティティ
		 */
		void Render(ecs::Registry* registry, mapping::rename_type::Entity ent);
	};
} // cumulonimbus::editor
