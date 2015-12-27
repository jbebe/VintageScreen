#include <string>

#include <gtest\gtest.h>

#include <TestFixture.hpp>

#include <Misc\PixelData\Color.hpp>

class ColorClassTest : public TestFixture {};

static float colorLength(const Color a) {
	return std::sqrtf(a.r*a.r + a.g*a.g + a.b*a.b);
}
static float colorDiff(const Color a, const Color b) {
	return colorLength(a-b);
}
static std::string &debugString(const Color a, const Color b) {
	static std::string out(256, ' ');
	out.replace(0, 255, "Diff: " + std::to_string(colorDiff(a, b)));
	return out;
}

TEST_F(ColorClassTest, ColorBoundaryTest) {

	// boundary tests
	{ // check upper bound
		Color c;
		c += 2.0f;
		EXPECT_TRUE(c == Color(1, 1, 1));
	}
	{ // check lower bound
		Color c;
		c -= 2.0f;
		EXPECT_TRUE(c == Color(0, 0, 0));
	}
}

TEST_F(ColorClassTest, HSLTest) {
	float EPS_FLOAT_VAL = Color::EPS_FLOAT;
	Color::EPS_FLOAT = 1e-2f;

	{ // black hsl stays black
		Color black(0, 0, 0);

		Color c1(0.0_h, 0.0_s, 0.0_l);
		EXPECT_TRUE(c1 == black) 
			<< debugString(c1, black);

		Color c2(0.5_h, 0.0_s, 0.0_l);
		EXPECT_TRUE(c2 == black) 
			<< debugString(c2, black);

		Color c3(1.0_h, 0.0_s, 0.0_l);
		EXPECT_TRUE(c3 == black) 
			<< debugString(c3, black);
	}
	{ // strong colors in hsl
		Color red(0.0_h, 1.0_s, 0.5_l);
		Color refRed(1, 0, 0);
		EXPECT_TRUE(red == refRed)
			<< debugString(red, refRed);

		Color green(Hue(1./3.), 1.0_s, 0.5_l);
		Color refGreen(0, 1, 0);
		EXPECT_TRUE(green == refGreen)
			<< debugString(green, refGreen);

		Color blue(Hue(2./3.), 1.0_s, 0.5_l);
		Color refBlue(0, 0, 1);
		EXPECT_TRUE(blue == refBlue)
			<< debugString(blue, refBlue);
	}
	{ // random colors
		Color c1(0.125_h, 0.28_s, 0.87_l);
		Color ref1(231./256., 226./256., 212./256.);
		EXPECT_TRUE(c1 == ref1)
			<< debugString(c1, ref1);

		Color c2(0.7528_h, 0.64_s, 0.15_l);
		Color ref2(39./256., 13./256., 62./256.);
		EXPECT_TRUE(c2 == ref2)
			<< debugString(c2, ref2);

		Color c3(0.236_h, 0.45_s, 0.33_l);
		Color ref3(90./256., 122./256., 46./256.);
		EXPECT_TRUE(c3 == ref3)
			<< debugString(c3, ref3);
	}

	Color::EPS_FLOAT = EPS_FLOAT_VAL;
}