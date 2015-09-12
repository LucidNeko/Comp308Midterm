#pragma once

#include <cstdlib>

class ColorUtil {
	public:
		ColorUtil();
		~ColorUtil();

		bool checkPixel(int x, int y, unsigned char r, 
				unsigned char g, unsigned char b);

		uint getColorID();

		unsigned char getR(uint color);
		unsigned char getG(uint color);
		unsigned char getB(uint color);

		bool checkPixel(int x, int y, uint color) {
			return checkPixel(x, y, getR(color), getG(color), getB(color));
		}
	private:
		uint colorSeed = 1;
};