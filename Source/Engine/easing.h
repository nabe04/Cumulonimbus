#pragma once

#include <math.h>
#include <memory>

enum e_EasingName
{
	LINEAR,
	SINE,
	QUARD,
	CUBIC,
	QUART,
	QUINT,
	EXPO,
	CIRC,
	BACK,
	ELASTIC,
	BOUNCE,

	NAME_END,
};

enum e_EasingType
{
	ESIN,
	ESOUT,
	ESINOUT,

	TYPE_END,
};



class Easing
{
public:
	/**
	 * @brief : easing関数の値取得
	 * @param t : 時間(進行度)
	 * @param b : 開始の値(開始時の座標やスケールなど)
	 * @param c : 開始と終了の値の差分
	 * @param d : Tween(トゥイーン)の合計時間
	 * @param e_name : イージング名
	 * @param e_type : 種類
	 */
	static float GetEasingVal(float t, float b, float c, float d, e_EasingName e_name,e_EasingType e_type);
};

class EasingSheet
{
public:
	virtual float EaseIn   (float t, float b, float c, float d) { return 0; }
	virtual float EaseOut  (float t, float b, float c, float d) { return 0; }
	virtual float EaseInOut(float t, float b, float c, float d) { return 0; }
};

class Linear :public EasingSheet
{
public:
	float EaseIn   (float t, float b, float c, float d) override;
	float EaseOut  (float t, float b, float c, float d) override;
	float EaseInOut(float t, float b, float c, float d) override;
};

class Sine : public EasingSheet
{
public:
	float EaseIn   (float t, float b, float c, float d) override;
	float EaseOut  (float t, float b, float c, float d) override;
	float EaseInOut(float t, float b, float c, float d) override;
};

class Quad : public EasingSheet
{
public:
	float EaseIn   (float t, float b, float c, float d) override;
	float EaseOut  (float t, float b, float c, float d) override;
	float EaseInOut(float t, float b, float c, float d) override;
};

class Cubic : public EasingSheet
{
public:
	float EaseIn   (float t, float b, float c, float d) override;
	float EaseOut  (float t, float b, float c, float d) override;
	float EaseInOut(float t, float b, float c, float d) override;
};

class Quart : public EasingSheet
{
public:
	float EaseIn   (float t, float b, float c, float d) override;
	float EaseOut  (float t, float b, float c, float d) override;
	float EaseInOut(float t, float b, float c, float d) override;
};

class Quint : public EasingSheet
{
public:
	float EaseIn   (float t, float b, float c, float d) override;
	float EaseOut  (float t, float b, float c, float d) override;
	float EaseInOut(float t, float b, float c, float d) override;
};

class Expo : public EasingSheet
{
public:
	float EaseIn(float t, float b, float c, float d) override;
	float EaseOut(float t, float b, float c, float d) override;
	float EaseInOut(float t, float b, float c, float d) override;
};

class Circ : public EasingSheet
{
public:
	float EaseIn   (float t, float b, float c, float d) override;
	float EaseOut  (float t, float b, float c, float d) override;
	float EaseInOut(float t, float b, float c, float d) override;
};

class Back : public EasingSheet
{
public:
	float EaseIn   (float t, float b, float c, float d) override;
	float EaseOut  (float t, float b, float c, float d) override;
	float EaseInOut(float t, float b, float c, float d) override;
};

class Elastic : public EasingSheet
{
public:
	float EaseIn   (float t, float b, float c, float d) override;
	float EaseOut  (float t, float b, float c, float d) override;
	float EaseInOut(float t, float b, float c, float d) override;
};

class Bounce : public EasingSheet
{
public:
	float EaseIn   (float t, float b, float c, float d) override;
	float EaseOut  (float t, float b, float c, float d) override;
	float EaseInOut(float t, float b, float c, float d) override;
};

extern Linear	eLinear;
extern Sine		eSine;
extern Quad		eQuad;
extern Cubic	eCubic;
extern Quart	eQuart;
extern Quint	eQuint;
extern Expo		eExpo;
extern Circ		eCirc;
extern Back		eBack;
extern Elastic	eElastic;
extern Bounce	eBounce;
