#ifndef SAMPLER_MAPPING_H
#define SAMPLER_MAPPING_H

#define SSlot_OnDemand0			0
#define SSlot_OnDemand1			1
#define SSlot_OnDemand2			2
#define SSlot_OnDemand3			3
#define SSlot_Count_OnDemand	(SSlot_OnDemand3 + 1)

// Persistent samplers
// デフォルトでセットされているサンプラー
// (ゲーム中は変更されることはない)
#define SSlot_Linear_Clamp		4
#define SSLot_Linear_Wrap		5
#define SSlot_Mirror			6
#define SSlot_Point_Clamp		7
#define SSlot_Point_Wrap		8
#define SSlot_Point_Mirror		9
#define SSlot_Aniso_Clamp		10
#define SSLot_Aniso_Wrap		11
#define SSlot_Aniso_Mirror		12
#define SSlot_Count_Persistent  (SSlot_Aniso_Mirror + 1 - SSlot_Count_OnDemand)

#define SSlot_Count  (SSlot_Count_Persistent + SSlot_Count_OnDemand)

#endif //SAMPLER_MAPPING_H