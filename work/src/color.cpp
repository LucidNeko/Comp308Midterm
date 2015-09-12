#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "color.hpp"


using namespace std;
// using namespace comp308;

ColorUtil::ColorUtil() {

}

ColorUtil::~ColorUtil() { }

bool ColorUtil::checkPixel(int x, int y, unsigned char r, 
		unsigned char g, unsigned char b) {
	unsigned char pixel[3];

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel);

	return (pixel[0] == r && pixel[1] == g && pixel[2] == b);
}

unsigned char ColorUtil::getR(uint color) {
	return color >> 16 & 0xFF;
}

unsigned char ColorUtil::getG(uint color) {
	return color >> 8 & 0xFF;
}

unsigned char ColorUtil::getB(uint color) {
	return color & 0xFF;
}

uint ColorUtil::getColorID() {
	return colorSeed++;
}