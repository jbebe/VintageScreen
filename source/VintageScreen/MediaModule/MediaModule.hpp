#pragma once

#include <memory>

#include <Misc\PixelData\PixelData.hpp>
#include <Misc\Tools\PPM.hpp>
#include <Misc\Debug\Assert.hpp>

class MediaModule {

private:

public:
	MediaModule(PixelData &pd) {
		using namespace PPM;
		
		std::unique_ptr<PPMImage> ppmImg(
			readPPM("..\\..\\resource\\pal_test-screen.ppm")
		);
		RT_ASSERT(ppmImg->x == pd.width && ppmImg->y == pd.height, "image size does not match screen");

		for (unsigned h = 0; h < ppmImg->y; h++) {
			for (unsigned w = 0; w < ppmImg->x; w++) {
				pd[h][w].r = float(ppmImg->data[((ppmImg->y)-h)*(ppmImg->x) + w].red)/256.f;
				pd[h][w].g = float(ppmImg->data[((ppmImg->y)-h)*(ppmImg->x) + w].green)/256.f;
				pd[h][w].b = float(ppmImg->data[((ppmImg->y)-h)*(ppmImg->x) + w].blue)/256.f;
			}
		}

	}

};