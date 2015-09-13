#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

#include "comp308.hpp"
#include "spline.hpp"

using namespace std;
using namespace comp308;

//

Spline::Spline() {

}

Spline::Spline(const Spline& s) {
	for(uint i = 0; i < s.points.size(); i++) {
		points.push_back(s.points[i]);		
	}
	dt = s.dt;
	rebuildTable();
}

Spline::~Spline() { }

void Spline::clear() {
	points.clear();
	dt = 0;
	m_arcLength = 0;
}

void Spline::addPoint(const vec3 &v) {
	points.push_back(v);
	dt = float(1) / float(points.size()-1);

	rebuildTable();

	// for(int i = 0; i < m_conversionTable.size(); i++) {
	// 	cout << "u:" << m_conversionTable[i].u << "s:" << m_conversionTable[i].s << endl;
	// }
}

void Spline::rebuildTable() {
	m_conversionTable.clear();

	//add n points
	for(int i = 0; i <= m_interpolationResolution; i++) {
		m_conversionTable.push_back(clp());
	}

	m_arcLength = 0;
	vec3 a = getInterpolatedPoint(0);
	m_conversionTable[0].s = 0;
	for(int i = 1; i <= m_interpolationResolution; i++) {
		vec3 b = getInterpolatedPoint(float(i)/float(m_interpolationResolution));
		m_arcLength += length(b-a);
		m_conversionTable[i].s = m_arcLength;
		a = b;
	}

	for(int i = 0; i <= m_interpolationResolution; i++) {
		m_conversionTable[i].u = 1.f/float(m_interpolationResolution) * i;
		if(points.size() < 2) {
			m_conversionTable[i].s = 0;
		} else {
			m_conversionTable[i].s /= m_arcLength;
		}
	}
}

//Get control point at index i
vec3 Spline::getControlPoint(uint n) {
	return points[n];
}

//get a spline point
vec3 Spline::getInterpolatedPoint(float t) {
	if(size() == 0) {
		// cout << "Spline has no points, cannot getInterpolatedPoint" << endl;
		return vec3();
	}

	int p = int(t/dt);

	#define BOUNDS(pp) { if (pp < 0) pp = 0; else if (pp >= (int)points.size()-1) pp = points.size() - 1; }
	int p0 = p - 1; BOUNDS(p0);
	int p1 = p;     BOUNDS(p1);
	int p2 = p + 1; BOUNDS(p2);
	int p3 = p + 2; BOUNDS(p3);

	float lt = (t - dt*float(p)) / dt;

	// cout << p0 << " " << p1 << " " << p2 << " " << p3 << " " << size() << "lt:" << lt << endl;

	return Spline::solve(lt, points[p0], points[p1], points[p2], points[p3]);
}

//get a spline point based on spline length. 0.5f gets middle of spline.
vec3 Spline::getUniformPoint(float t) {
	if(points.size() == 0) {
		// cout << "Spline has no points, cannot getUniformPoint" << endl;
		return vec3();
	}

	int i0 = 0;
	int i1 = 0;

	for(uint i = 0; i < m_conversionTable.size(); i++) {
		if(m_conversionTable[i].s >= t) {
			i1 = i;
			i0 = i-1; //segfault? -1?
			break;
		}
	}

	// cout << "i0:" << i0 << " i1:" << i1 << endl;

	clp p0 = m_conversionTable[i0];
	clp p1 = m_conversionTable[i1];

	float u = p0.u + (t-p0.s)/(p1.s-p0.s)*(p1.u-p0.u); 

	return getInterpolatedPoint(u);
}

//get this spline as a normalized version.
//spline is surrounded by a bounding box and then normalized with
//bottom left as (0,0) and top right as (1,1)
void Spline::getNormalizedSpline(Spline& s) {
	//initial values for min max
	vec3 p0 = getInterpolatedPoint(0);
	float minX =  p0.x;
	float minY =  p0.y;
	float maxX = p0.x;
	float maxY = p0.y;

	//find bounding box around spline
	for(int i = 0; i < m_interpolationResolution; i++) {
		vec3 p = getInterpolatedPoint(float(i)/float(m_interpolationResolution));
		if(p.x < minX) { minX = p.x; }
		if(p.y < minY) { minY = p.y; }
		if(p.x > maxX) { maxX = p.x; }
		if(p.y > maxY) { maxY = p.y; }	
	}

	float x = minX;
	float y = minY;
	float w = maxX-minX;
	float h = maxY-minY;

	//build normalized spline
	s.clear();
	for(int i = 0; i < size(); i++) {
		vec3 v = getControlPoint(i);
		s.addPoint(vec3((v.x-x)/w, (v.y-y)/h, v.z));
	}
}

int Spline::size() {
	return points.size();
}

float Spline::arcLength() {
	return m_arcLength;
}


vec3 Spline::solve(float t, const vec3& p1, 
	const vec3& p2, const vec3& p3, const vec3& p4) {
		float t2 = t * t;
		float t3 = t2 * t;

		float b1 = .5 * (  -t3 + 2*t2 - t);
		float b2 = .5 * ( 3*t3 - 5*t2 + 2);
		float b3 = .5 * (-3*t3 + 4*t2 + t);
		float b4 = .5 * (   t3 -   t2    );

		return (p1*b1 + p2*b2 + p3*b3 + p4*b4);
}