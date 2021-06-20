#ifndef SHADER_RESOURCE_MAPPING_H
#define SHADER_RESOURCE_MAPPING_H

#define TexSlot_Depth		     0
#define TexSlot_LinearDepth      1

#define TexSlot_GBuffer0         2
#define TexSlot_GBuffer1         3
#define TexSlot_GBuffer2         4
#define TexSlot_GBuffer3         5
#define TexSlot_GBuffer4         6
#define TexSlot_GBuffer5         7
#define TexSlot_GBuffer6         8
#define TexSlot_GBuffer7         9
#define TexSlot_GBuffer8         10

#define TexSlot_OnDemand0		 20
#define TexSlot_OnDemand1		 21
#define TexSlot_OnDemand2		 22
#define TexSlot_OnDemand3		 23
#define TexSlot_OnDemand4		 24
#define TexSlot_OnDemand5		 25
#define TexSlot_OnDemand6		 26
#define TexSlot_OnDemand7		 27
#define TexSlot_OnDemand8		 28
#define TexSlot_OnDemand9		 29
#define TexSlot_OnDemand10		 30
#define TexSlot_OnDemand11		 31
#define TexSlot_OnDemand12		 32
#define TexSlot_OnDemand13		 33
#define TexSlot_OnDemand14		 34
#define TexSlot_OnDemand15		 35
#define TexSlot_OnDemand16		 36
#define TexSlot_OnDemand17		 37
#define TexSlot_OnDemand18		 38
#define TexSlot_OnDemand19		 39
#define TexSlot_OnDemand20		 40
#define TexSlot_OnDemand_Count	 (TexSlot_OnDemand20 - TexSlot_OnDemand0 + 1)

#define TexSlot_BaseColorMap    TexSlot_OnDemand0
#define TexSlot_NormalMap       TexSlot_OnDemand1
#define TexSlot_SurfaceMap	    TexSlot_OnDemand2
#define TexSlot_EmissiveMap     TexSlot_OnDemand3
#define TexSlot_DisplacementMap TexSlot_OnDemand4
#define TexSlot_OcclusionMap    TexSlot_OnDemand5
#define TexSlot_TransmissionMap TexSlot_OnDemand6
#define TexSlot_RoughnessMap    TexSlot_OnDemand7
#define TexSlot_SpecularMap		TexSlot_OnDemand8

#endif //SHADER_RESOURCE_MAPPING_H