#pragma once
#ifndef UTILS
#define UTILS
namespace Utils {

	float InvLerp(float a, float b, float v) {
		return (v - a) / (b - a);
	}

	float Lerp(float a, float b, float t) {
		return (1.0f - t) * a + b * t;
	}

	float Remap(float iMin, float iMax, float oMin, float oMax, float v) {
		float t = InvLerp(iMin, iMax, v);
		return Lerp(oMin, oMax, t);
	}
}
#endif