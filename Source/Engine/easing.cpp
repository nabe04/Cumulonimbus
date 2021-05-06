
#include "easing.h"

#include <DirectXMath.h>
using namespace DirectX;


Linear	eLinear;
Sine	eSine;
Quad	eQuad;
Cubic	eCubic;
Quart	eQuart;
Quint	eQuint;
Expo	eExpo;
Circ	eCirc;
Back	eBack;
Elastic eElastic;
Bounce	eBounce;

EasingSheet* easingSheet[] =
{
	&eLinear,
	&eSine,
	&eQuad,
	&eCubic,
	&eQuart,
	&eQuint,
	&eExpo,
	&eCirc,
	&eBack,
	&eElastic,
	&eBounce,
};

EasingSheet* eSheet;

float Easing::GetEasingVal(float t, float b, float c, float d, e_EasingName eName,e_EasingType eType)
{
	switch (eType)
	{
	case ESIN:
		return easingSheet[eName]->EaseIn(t, b, c, d);
		break;
	case ESOUT:
		return easingSheet[eName]->EaseOut(t, b, c, d);
		break;
	case ESINOUT:
		return easingSheet[eName]->EaseInOut(t, b, c, d);
		break;
	}

	return 0;
}

////-- Linear --////
float Linear::EaseIn(float t, float b, float c, float d)
{
	return c*t / d + b;
}

float Linear::EaseOut(float t, float b, float c, float d)
{
	return c*t / d + b;
}

float Linear::EaseInOut(float t, float b, float c, float d)
{
	return c*t / d + b;
}


////-- Sine --////
float Sine::EaseIn(float t, float b, float c, float d)
{
	const float cosVal = t / d * (XM_PI / 2);
	return -c * cosf(cosVal) + c + b;
}

float Sine::EaseOut(float t, float b, float c, float d)
{
	const float sinVal = t / d * (XM_PI / 2);
	return c * sinf(sinVal) + b;
}

float Sine::EaseInOut(float t, float b, float c, float d)
{
	return -c / 2 * (cosf(XM_PI *t / d) - 1) + b;
}


////-- Quad --////
float Quad::EaseIn(float t, float b, float c, float d)
{
	return c*(t /= d)*t + b;
}

float Quad::EaseOut(float t, float b, float c, float d)
{
	return -c *(t /= d)*(t - 2) + b;
}

float Quad::EaseInOut(float t, float b, float c, float d)
{
	if ((t /= d / 2) < 1) return ((c / 2)*(t*t)) + b;
	return -c / 2 * (((t - 2)*(--t)) - 1) + b;
}

////-- Cubic --////
float Cubic::EaseIn(float t, float b, float c, float d)
{
	return c*(t /= d)*t*t + b;
}

float Cubic::EaseOut(float t, float b, float c, float d)
{
	return c*((t = t / d - 1)*t*t + 1) + b;
}

float Cubic::EaseInOut(float t, float b, float c, float d)
{
	if ((t /= d / 2) < 1) return c / 2 * t*t*t + b;
	return c / 2 * ((t -= 2)*t*t + 2) + b;
}

////-- Quart --////
float Quart::EaseIn(float t, float b, float c, float d)
{
	return c*(t /= d)*t*t*t + b;
}

float Quart::EaseOut(float t, float b, float c, float d)
{
	return -c * ((t = t / d - 1)*t*t*t - 1) + b;
}

float Quart::EaseInOut(float t, float b, float c, float d)
{
	if ((t /= d / 2) < 1) return c / 2 * t*t*t*t + b;
	return -c / 2 * ((t -= 2)*t*t*t - 2) + b;
}

////-- Quint --////
float Quint::EaseIn(float t, float b, float c, float d)
{
	return c*(t /= d)*t*t*t*t + b;
}

float Quint::EaseOut(float t, float b, float c, float d)
{
	return c*((t = t / d - 1)*t*t*t*t + 1) + b;
}

float Quint::EaseInOut(float t, float b, float c, float d)
{
	if ((t /= d / 2) < 1) return c / 2 * t*t*t*t*t + b;
	return c / 2 * ((t -= 2)*t*t*t*t + 2) + b;
}

////-- Expo --////
float Expo::EaseIn(float t, float b, float c, float d)
{
	const float val = t / d - 1.0f;
	return (t == 0) ? b : c * powf(2.f, 10.f * val) + b;
}

float Expo::EaseOut(float t, float b, float c, float d)
{
	return (t == d) ? b + c : c * (-powf(2, -10 * t / d) + 1) + b;
}

float Expo::EaseInOut(float t, float b, float c, float d)
{
	if (t == 0) return b;
	if (t == d) return b + c;
	if ((t /= d / 2) < 1) return c / 2 * powf(2.f, 10.f * static_cast<float>((t - 1.f))) + b;
	return c / 2 * (-powf(2, -10.f * --t) + 2) + b;
}

////-- Circ --////
float Circ::EaseIn(float t, float b, float c, float d)
{
	return -c * (sqrtf(1.f - (t /= d)*t) - 1) + b;
}

float Circ::EaseOut(float t, float b, float c, float d)
{
	const float val = (t / d - 1)* t;
	return c * sqrtf(1 - (t = t / d - 1)*t) + b;
}

float Circ::EaseInOut(float t, float b, float c, float d)
{
	if ((t /= d / 2) < 1) return -c / 2 * (sqrtf(1.f - t*t) - 1.f) + b;
	return c / 2 * (sqrtf(1.f - t*(t -= 2.f)) + 1) + b;
}

////-- Back --////
float Back::EaseIn(float t, float b, float c, float d)
{
	float s = 1.70158f;
	float postFix = t /= d;
	return c*(postFix)*t*((s + 1)*t - s) + b;
}

float Back::EaseOut(float t, float b, float c, float d)
{
	float s = 1.70158f;
	return c*((t = t / d - 1)*t*((s + 1)*t + s) + 1) + b;
}

float Back::EaseInOut(float t, float b, float c, float d)
{
	float s = 1.70158f;
	if ((t /= d / 2) < 1) return c / 2 * (t*t*(((s *= (1.525f)) + 1)*t - s)) + b;
	float postFix = t -= 2;
	return c / 2 * ((postFix)*t*(((s *= (1.525f)) + 1)*t + s) + 2) + b;
}

////-- Elastic --////
float Elastic::EaseIn(float t, float b, float c, float d)
{
	if (t == 0) return b;  if ((t /= d) == 1) return b + c;
	float p = d*.3f;
	float a = c;
	float s = p / 4;
	float postFix = a*powf(2.f, 10.f * (t -= 1.f)); // this is a fix, again, with post-increment operators
	return -(postFix * sinf((t*d - s)*(2.f * XM_PI) / p)) + b;
}

float Elastic::EaseOut(float t, float b, float c, float d)
{
	if (t == 0) return b;  if ((t /= d) == 1) return b + c;
	float p = d*.3f;
	float a = c;
	float s = p / 4;
	const float val = -10 * t;
	return (a*powf(2.f, val) * sinf((t*d - s)*(2 * XM_PI) / p) + c + b);
}

float Elastic::EaseInOut(float t, float b, float c, float d)
{
	if (t == 0) return b;  if ((t /= d / 2) == 2) return b + c;
	float p = d*(.3f*1.5f);
	float a = c;
	float s = p / 4;

	if (t < 1)
	{
		float postFix = a*powf(2.f, 10.f * static_cast<float>((t -= 1.f))); // postIncrement is evil
		return -.5f*(postFix* sinf((t*d - s)*(2 * XM_PI) / p)) + b;
	}
	float postFix = a*powf(2, static_cast<float>(-10.f * (t -= 1.f))); // postIncrement is evil
	return postFix * sinf((t*d - s)*(2.f * XM_PI) / p)*.5f + c + b;
}


////-- Bounce --////
float Bounce::EaseIn(float t, float b, float c, float d)
{
	return c - EaseOut(d - t, 0, c, d) + b;
}

float Bounce::EaseOut(float t, float b, float c, float d)
{
	if ((t /= d) < (1 / 2.75f))
	{
		return c*(7.5625f*t*t) + b;
	}
	else if (t < (2 / 2.75f))
	{
		float postFix = t -= (1.5f / 2.75f);
		return c*(7.5625f*(postFix)*t + .75f) + b;
	}
	else if (t < (2.5 / 2.75))
	{
		float postFix = t -= (2.25f / 2.75f);
		return c*(7.5625f*(postFix)*t + .9375f) + b;
	}
	else
	{
		float postFix = t -= (2.625f / 2.75f);
		return c*(7.5625f*(postFix)*t + .984375f) + b;
	}
}

float Bounce::EaseInOut(float t, float b, float c, float d)
{
	if (t < d / 2) return EaseIn(t * 2, 0, c, d) * .5f + b;
	else return EaseOut(t * 2 - d, 0, c, d) * .5f + c*.5f + b;
}

