#pragma once
namespace cumulonimbus::mapping::sampler
{
	using u_int = unsigned int;

	// On demand samplers:
	// �K�v�ɉ����ăZ�b�g�����
	// (���̃T���v���[���Z�b�g�����܂Ő���������)
	constexpr u_int SSlot_OnDemand0			= 0;
	constexpr u_int SSlot_OnDemand1			= 1;
	constexpr u_int SSlot_OnDemand2			= 2;
	constexpr u_int SSlot_OnDemand3			= 3;
	constexpr u_int SSlot_Count_OnDemand	= (SSlot_OnDemand3 + 1);

	// Persistent samplers
	// �f�t�H���g�ŃZ�b�g����Ă���T���v���[
	// (�Q�[�����͕ύX����邱�Ƃ͂Ȃ�)
	constexpr u_int SSlot_Linear_Clamp		= 4;
	constexpr u_int SSLot_Linear_Wrap		= 5;
	constexpr u_int SSlot_Mirror			= 6;
	constexpr u_int SSlot_Point_Clamp		= 7;
	constexpr u_int SSlot_Point_Wrap		= 8;
	constexpr u_int SSlot_Point_Mirror		= 9;
	constexpr u_int SSlot_Aniso_Clamp		= 10;
	constexpr u_int SSLot_Aniso_Wrap		= 11;
	constexpr u_int SSlot_Aniso_Mirror		= 12;
	constexpr u_int SSlot_Count_Persistent	= (SSlot_Aniso_Mirror + 1 - SSlot_Count_OnDemand);

	constexpr u_int SSlot_Count				= (SSlot_Count_Persistent + SSlot_Count_OnDemand);

}
