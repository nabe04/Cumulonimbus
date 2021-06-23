#ifndef CONSTANT_BUFFER_MAPPING_H
#define CONSTANT_BUFFER_MAPPING_H

// 全てのシェーダーに共通して使用する
// 5〜9番は今後共通して使用する可能性を考えて空けておく
#define CBSlot_Renderer_Frame	0
#define CBSlot_Camera			1
#define CBSlot_Light			2
#define CBSlot_Transform		3
#define CBSlot_Material			4

// オンデマンド
#define CBSlot_OnDemand0		10
#define CBSlot_OnDemand1		11
#define CBSlot_OnDemand2		12
#define CBSlot_OnDemand3		13
#define CBSlot_OnDemand4		14

// それぞれのシェーダー独自のCBuffer
#define CBSlot_Phong				CBSlot_OnDemand0
#define CBSlot_Metal				CBSlot_OnDemand0
#define CBSlot_Toon					CBSlot_OnDemand0
#define CBSlot_ReflectionMapping	CBSlot_OnDemand0
#define CBSlot_RefractionMapping	CBSlot_OnDemand0
#define CBSlot_SingleColor			CBSlot_OnDemand0

#endif // CONSTANT_BUFFER_MAPPING_H