#pragma once

#include <string>
#include <vector>

#include "comp308.hpp"

class Spline {
	public:
		Spline();
		Spline(const Spline&);
		~Spline();

		void addPoint(const comp308::vec3&);
		comp308::vec3 getInterpolatedPoint(float t);
		comp308::vec3 getUniformPoint(float t);
		int size();
		comp308::vec3 getControlPoint(uint n);
		void clear();
		float arcLength();

		void getNormalizedSpline(Spline&);

		static comp308::vec3 solve(float t, const comp308::vec3& p1, 
			const comp308::vec3& p2, const comp308::vec3& p3, const comp308::vec3& p4);
	private:
		std::vector<comp308::vec3> points;
		float dt;
		float m_arcLength;
		int m_interpolationResolution = 200;

		//Chord length parameterization
		struct clp {
			float u;
			float s;
		};

		std::vector<clp> m_conversionTable;

		void rebuildTable();
};