#pragma once

#undef max
#undef min

#include <algorithm>

#define GenHSLLiteral(name, suffix) \
	struct name { \
		float value; \
		name(float value): value(value) {} \
	}; \
	name operator "" suffix (long double h) { \
		return name(static_cast<float>(h)); \
	}

GenHSLLiteral(Hue, _h);
GenHSLLiteral(Saturation, _s);
GenHSLLiteral(Lightness, _l);

#undef GenHSLStruct

struct HSLColor {
	float h, s, l;
	HSLColor(float h, float s, float l)
		: h(h), s(s), l(l)
	{}
	HSLColor(const HSLColor &rhs)
		: h(rhs.h), s(rhs.s), l(rhs.l)
	{}
};

struct RGBColor {
	float r, g, b;
	RGBColor(float r, float g, float b)
		: r(r), g(g), b(b)
	{}
};

// Complatible with raw float r,g,b structs
struct Color {
	static float EPS_FLOAT;

	float r;
	float g;
	float b;

	Color (float r = 0, float g = 0, float b = 0) 
		: r(r), g(g), b(b)
	{
		correctRGB();
	}

	Color (Hue h, Saturation s, Lightness l) {
		RGBColor rgbc(HSLtoRGB(h.value, s.value, l.value));
		r = rgbc.r;
		g = rgbc.g;
		b = rgbc.b;

		correctRGB();
	}

	Color(const Color &rhs)
		: r(rhs.r), g(rhs.g), b(rhs.b)
	{}

	HSLColor hsl() const {
		return RGBtoHSL(r, g, b);
	}

	static RGBColor HSLtoRGB(float h, float s, float l) {
		float r, g, b;

		if(s == 0){
			r = g = b = l; // achromatic
		}
		else {
			auto hue2rgb = [](float p, float q, float t){
				if(t < 0.f) t += 1.f;
				if(t > 1.f) t -= 1.f;
				if(t < 1.f/6.f) return p + (q - p)*6.f * t;
				if(t < 1.f/2.f) return q;
				if(t < 2.f/3.f) return p + (q - p)*(2.f/3.f - t)*6.f;
				return p;
			};

			float q = l < 0.5f ? l*(1.f + s) : l + s - l*s;
			float p = 2.f*l - q;
			r = hue2rgb(p, q, h + 1.f/3.f);
			g = hue2rgb(p, q, h);
			b = hue2rgb(p, q, h - 1.f/3.f);
		}
		
		return RGBColor(r, g, b);
	}

	static HSLColor RGBtoHSL(float r, float g, float b) {
		float max = std::max(r, std::max(g, b)), min = std::min(r, std::max(g, b));
		float h, s, l = (max + min)/2.f;

		if(max == min) {
			h = s = 0; // achromatic
		}
		else {
			float d = max - min;
			s = l > 0.5f ? d/(2.f - max - min) : d/(max + min);
			if (max == r) {
				h = (g - b)/d + (g < b ? 6.f : 0.f);
			}
			else if (max == g) {
				h = (b - r)/d + 2.f;
			}
			else {
				h = (r - g)/d + 4.f;
			}
			h /= 6.f;
		}

		return HSLColor(h, s, l);
	}

	// correct values to stay in [0, 1]
	void correctRGB() {
		auto correctValue = [](float &val) {
			if (val < 0.f) val = 0;
			else if (val > 1.f) val = 1;
		};
		correctValue(r);
		correctValue(g);
		correctValue(b);
	}

	bool operator == (const Color rhs) {
		return 	(std::fabs(r-rhs.r) < EPS_FLOAT) 
			&&	(std::fabs(g-rhs.g) < EPS_FLOAT) 
			&&	(std::fabs(b-rhs.b) < EPS_FLOAT);
	}

	Color &operator = (const Color rhs){
		r = rhs.r; g = rhs.g; b = rhs.b;
		correctRGB();
		return *this;
	}

	Color &operator *= (float a){
		r *= a; g *= a; b *= a;
		correctRGB();
		return *this;
	}
	
	Color &operator*=(const Color c){
		r *= c.r; g *= c.g; b *= c.b;
		correctRGB();
		return *this;
	}
	
	Color operator*(const Color c) const {
		return Color(r*c.r, g*c.g, b*c.b);
	}
	
	Color operator/(const Color c) const {
		return Color(r/c.r, g/c.g, b/c.b);
	}
	
	Color operator*(float a) const {
		return Color(r*a, g*a, b*a);
	}
	
	friend Color operator*(float a, const Color c){
		return Color(c.r*a, c.g*a, c.b*a);
	}
	
	Color &operator += (float a){
		r += a; g += a; b += a;
		correctRGB();
		return *this;
	}
	
	Color operator + (float a) const {
		return Color(r + a, g + a, b + a);
	}
	
	Color &operator += (const Color rhs){
		r += rhs.r; g += rhs.g; b += rhs.b;
		correctRGB();
		return *this;
	}
	
	Color operator + (const Color c) const {
		return Color(r + c.r, g + c.g, b + c.b);
	}
	
	Color operator - (const Color c) const {
		return Color(r - c.r, g - c.g, b - c.b);
	}
	
	Color &operator -= (float a){
		r -= a; g -= a; b -= a;
		correctRGB();
		return *this;
	}
	
	Color operator - (float a) const {
		return Color(r - a, g - a, b - a);
	}

};


float Color::EPS_FLOAT = 1e-5f;