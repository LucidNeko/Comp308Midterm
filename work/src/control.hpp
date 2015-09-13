#pragma once

#include "spline.hpp"
#include "comp308.hpp"

class Control {
	private:
		Spline *m_splinePath = new Spline();
		Spline *m_splineSpeed = new Spline();
		Spline *m_normalizedSplineSpeed = new Spline();

		int m_width;
		int m_height;

		struct parama {
			float u;
			float s;
		};

		int steps = 100;
		float m_arcLength = 0;
		std::vector<parama> m_paramas;


		void renderSpline(Spline*, const comp308::vec3&, const comp308::vec3&);

	public:
		Control(int width, int height);
		~Control();

		void onClick(int x, int y);
		void render();
		void clear();

		Spline* getNormalizedSpeedSpline() {
			return m_normalizedSplineSpeed;
		}

		Spline* getSpeedSpline() {
			return m_splineSpeed;
		}

		Spline* getSplinePath() {
			return m_splinePath;

		}

		float modWithSpeed(float t);

		Spline* projectOnXZPlane();

		comp308::vec3 getUniformPoint(float t);

		bool hasAnimation() {
			return m_splinePath->size() >= 2 && m_splineSpeed->size() >= 2;
		}
};