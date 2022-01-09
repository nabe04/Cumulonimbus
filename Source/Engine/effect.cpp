#include "effect.h"

#include "locator.h"

namespace cumulonimbus::asset
{
	Effect::Effect(const std::string& file_path)
	{
		auto* effekseer_manager = locator::Locator::GetEffeckseerManager();

		char16_t utf16_filename[256];
		Effekseer::ConvertUtf8ToUtf16(utf16_filename, 256, file_path.c_str());

		effect			= Effekseer::Effect::Create(const_cast<Effekseer::Manager*>(effekseer_manager->GetManager()), static_cast<EFK_CHAR*>(utf16_filename));
		this->file_path = file_path;
	}

	Effect::~Effect()
	{
		ES_SAFE_RELEASE(effect);
	}
} // cumulonimbus::asset
