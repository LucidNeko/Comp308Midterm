#include "control.hpp"
#include "spline.hpp"

using namespace std;
using namespace comp308;

namespace {
	void drawShape(float x, float y, float radius, float nSides) {
		//Value 1.5 - Draws Triangle
		//Value 2 - Draws Square
		//Value 3 - Draws Hexagon
		//Value 4 - Draws Octagon
		//Value 5 - Draws Decagon
		//credit DSB @ opengl.org
		#define PI 3.1415926535897932384626433832795
		glBegin(GL_POLYGON);
		for(double i = 0; i < 2 * PI; i += PI / (nSides/2)) //<-- Change this Value
				glVertex3f(x + cos(i) * radius, y + sin(i) * radius, 0.5f);
		glEnd();
	}
}

struct rect {
	float x;
	float y;
	float w;
	float h;

	bool contains(float _x, float _y) {
		return _x > x && _x < x+w && _y > y && y < y+h;
	}

	vec3 normalized(const vec3 &v) {
		return vec3((v.x-x)/w, (v.y-y)/h, v.z);
	}
};

rect m_speedBox;
rect m_pathBox;

Control::Control(int width, int height) {
	m_width = width;
	m_height = height;
	// m_speedBox = rect(0, m_height/2.f, m_width/2.f, m_height/2.f);
	// m_pathBox = rect(0, 0, m_width, m_height);
	m_speedBox.x = 0;
	m_speedBox.y = m_height/2.f;
	m_speedBox.w = m_width/2.f;
	m_speedBox.h = m_height/2.f;

	m_pathBox.x = 0;
	m_pathBox.y = 0;
	m_pathBox.w = m_width;
	m_pathBox.h = m_height;
}

Control::~Control() {
	delete m_splinePath;
	delete m_splineSpeed;
}

void Control::clear() {
	m_splinePath->clear();
	m_splineSpeed->clear();
	m_normalizedSplineSpeed->clear();
}

void Control::onClick(int x, int y) {
	if(m_speedBox.contains(x, y)) {
		//in speed
		if(m_splineSpeed->size() == 0) {
			m_splineSpeed->addPoint(vec3(x, y, 0));

			//extract the normalized spline
			m_splineSpeed->getNormalizedSpline(*m_normalizedSplineSpeed);
		} else {
			vec3 last = m_splineSpeed->getControlPoint(m_splineSpeed->size()-1);
			if(last.x < x && last.y < y) {
				//add
				m_splineSpeed->addPoint(vec3(x, y, 0));

				//extract the normalized spline
				m_splineSpeed->getNormalizedSpline(*m_normalizedSplineSpeed);
			}
		}

		// m_splineSpeed->addPoint(vec3(x, y, 0));

		// //extract the normalized spline
		// m_splineSpeed->getNormalizedSpline(*m_normalizedSplineSpeed);

		// cout << "x:" << x << " y:" << y << " speed" << endl;
	} else if(m_pathBox.contains(x, y)) {
		//in controller
		m_splinePath->addPoint(vec3(x, y, 0));
		// cout << "x:" << x << " y:" << y << " path" << endl;
	} else {
		//not in controller
	}
}

void Control::render() {
	//enter 2D mode
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, m_width, 0, m_height, -1, 1);

	//enter render mode
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//render
	//speed
	renderSpline(m_splineSpeed, vec3(0.000f, 0.749f, 1.000f), vec3(1.000f, 0.000f, 0.0f));

	//path
	renderSpline(m_splinePath, vec3(0.000f, 0.749f, 1.000f), vec3(1.000f, 0.000f, 0.0f));

	//bounds
	glColor3f(0,1,0);
	glBegin(GL_LINES);
		//whole
		glVertex3f(0,0,0);
		glVertex3f(m_width,0,0);

		glVertex3f(m_width,0,0);
		glVertex3f(m_width, m_height,0);

		glVertex3f(m_width, m_height,0);
		glVertex3f(0,m_height,0);

		glVertex3f(0,m_height,0);
		glVertex3f(0,0,0);
		//speed
		glVertex3f(0,m_height/2,0);
		glVertex3f(m_width/2,m_height/2,0);

		glVertex3f(m_width/2,m_height/2,0);
		glVertex3f(m_width/2, m_height,0);

		glVertex3f(m_width/2, m_height,0);
		glVertex3f(0,m_height,0);

		glVertex3f(0,m_height,0);
		glVertex3f(0,m_height/2,0);
	glEnd();

	glPopMatrix();

	//exit 2D mode
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void Control::renderSpline(Spline *s, const vec3 &splineColor, const vec3 &controlColor) {
	glColor3f(splineColor.x, splineColor.y, splineColor.z);
	vec3 a = s->getInterpolatedPoint(0);
	glBegin(GL_LINES);
		for(float t = 0.001f; t <= 1; t += 0.001f) {
			vec3 b = s->getInterpolatedPoint(t);
			glVertex3f(a.x, a.y, a.z);
			glVertex3f(b.x, b.y, b.z);
			a = b;
		}
	glEnd();

	//draw control points
	glColor3f(controlColor.x, controlColor.y, controlColor.z);
	for(int i = 0; i < s->size(); i++) {
		vec3 v = s->getControlPoint(i);
		drawShape(v.x, v.y, 6, 8);
	}

	// cout << "draw paramas" << endl;

	//draw paramas

	

	int points = int(s->arcLength()/40);

	// cout << points << endl;

	for(int i = 0; i < points; i++) {
		glColor3f(0.f, 1.f, 0.f);
		vec3 v = s->getInterpolatedPoint(float(i)/points);
		drawShape(v.x, v.y, 3, 4);

		// //draw even intervals
		// glColor3f(0.f, 0.f, 1.f);
		// v = s->getUniformPoint(float(i)/points);
		// drawShape(v.x, v.y, 5, 3);

		//draw with speed influence
		glColor3f(0.f, 0.f, 1.f);
		v = s->getUniformPoint(float(i)/points);
		drawShape(v.x, v.y, 5, 3);
	}
}

vec3 Control::getUniformPoint(float t) {
	if(m_splinePath->size() == 0) {
		return vec3();
	}

	// if(m_splineSpeed->size() != 0) {
	// 	Spline *s = m_splineSpeed->getNormalizedSpline();
	// 	t = s->getUniformPoint(t).y;
	// 	delete s;
	// }

	return m_splinePath->getUniformPoint(m_normalizedSplineSpeed->getInterpolatedPoint(t).y);

	// return m_splinePath->getUniformPoint(t);
}

Spline* Control::projectOnXZPlane() {
	Spline *out = new Spline();
	Spline *s = new Spline();
	m_splinePath->getNormalizedSpline(*s);
	for(int i = 0; i < s->size(); i++) {
		vec3 v = s->getControlPoint(i);
		out->addPoint(vec3(v.x*2, 0.f, -v.y*2)); //*2 scale
	}
	delete s;
	return out;
}

float Control::modWithSpeed(float t) {
	return m_normalizedSplineSpeed->getInterpolatedPoint(t).y;
}