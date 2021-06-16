#pragma once
namespace cumulonimbus::mapping::texture_resource
{
	using u_int = unsigned int;

	constexpr u_int TexSlot_Depth		= 0;
	constexpr u_int TexSlot_LinearDepth = 1;

	constexpr u_int TexSlot_GBuffer0 = 2;
	constexpr u_int TexSlot_GBuffer1 = 3;
	constexpr u_int TexSlot_GBuffer2 = 4;
	constexpr u_int TexSlot_GBuffer3 = 5;
	constexpr u_int TexSlot_GBuffer4 = 6;
	constexpr u_int TexSlot_GBuffer5 = 7;
	constexpr u_int TexSlot_GBuffer6 = 8;
	constexpr u_int TexSlot_GBuffer7 = 9;
	constexpr u_int TexSlot_GBuffer8 = 10;


	constexpr u_int TexSlot_OnDemand0		= 20;
	constexpr u_int TexSlot_OnDemand1		= 21;
	constexpr u_int TexSlot_OnDemand2		= 22;
	constexpr u_int TexSlot_OnDemand3		= 23;
	constexpr u_int TexSlot_OnDemand4		= 24;
	constexpr u_int TexSlot_OnDemand5		= 25;
	constexpr u_int TexSlot_OnDemand6		= 26;
	constexpr u_int TexSlot_OnDemand7		= 27;
	constexpr u_int TexSlot_OnDemand8		= 28;
	constexpr u_int TexSlot_OnDemand9		= 29;
	constexpr u_int TexSlot_OnDemand10		= 30;
	constexpr u_int TexSlot_OnDemand11		= 31;
	constexpr u_int TexSlot_OnDemand12		= 32;
	constexpr u_int TexSlot_OnDemand13		= 33;
	constexpr u_int TexSlot_OnDemand14		= 34;
	constexpr u_int TexSlot_OnDemand15		= 35;
	constexpr u_int TexSlot_OnDemand16		= 36;
	constexpr u_int TexSlot_OnDemand17		= 37;
	constexpr u_int TexSlot_OnDemand18		= 38;
	constexpr u_int TexSlot_OnDemand19		= 39;
	constexpr u_int TexSlot_OnDemand20		= 40;
	constexpr u_int TexSlot_OnDemand_Count	= (TexSlot_OnDemand20 - TexSlot_OnDemand0 + 1);

	constexpr u_int TexSlot_BaseColorMap	= TexSlot_OnDemand0;
	constexpr u_int TexSlot_NormalMap		= TexSlot_OnDemand1;
	constexpr u_int TexSlot_SurfaceMap		= TexSlot_OnDemand2;
	constexpr u_int TexSlot_EmissiveMap		= TexSlot_OnDemand3;
	constexpr u_int TexSlot_DisplacementMap = TexSlot_OnDemand4;
	constexpr u_int TexSlot_OcclusionMap	= TexSlot_OnDemand5;
	constexpr u_int TexSlot_TransmissionMap = TexSlot_OnDemand6;
	constexpr u_int TexSlot_RoughnessMap	= TexSlot_OnDemand7;
	constexpr u_int TexSlot_SpecularMap		= TexSlot_OnDemand8;
}