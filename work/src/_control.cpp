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
}

void Control::onClick(int x, int y) {
	if(m_speedBox.contains(x, y)) {
		//in speed
		m_splineSpeed->addPoint(vec3(x, y, 0));
		// cout << "x:" << x << " y:" << y << " speed" << endl;
	} else if(m_pathBox.contains(x, y)) {
		//in controller
		m_splinePath->addPoint(vec3(x, y, 0));
		// cout << "x:" << x << " y:" << y << " path" << endl;
	} else {
		//not in controller
	}

	//clear params
	m_paramas.clear();

	//calculate paramas
	m_arcLength = 0;
	vector<float> arcLengths;
	vec3 a = m_splinePath->getInterpolatedPoint(0);
	arcLengths.push_back(0);
	for(int i = 1; i <= steps; i++) {
		vec3 b = m_splinePath->getInterpolatedPoint(float(i)/float(steps));
		m_arcLength += length(b-a);
		arcLengths.push_back(m_arcLength);
		a = b;
	}

	// cout << "arcLen: " << m_arcLength << endl;

	for(int i = 0; i <= steps; i++) {
		parama p;
		p.u = 1.f/float(steps) * i;
		if(m_splinePath->size() < 2) {
			p.s = 0; //-nan when 1 control point
		} else {
			p.s = arcLengths[i]/m_arcLength;
		}
		// cout << "p.s:" << p.s << endl;
		m_paramas.push_back(p);
	}

	// cout << "arcLength:" << m_arcLength << " paramas" << endl;
	// for(uint i = 0; i < m_paramas.size(); i++) {
	// 	cout << "idx:" << i << " u:" << m_paramas[i].u << " s:" << m_paramas[i].s << endl;
	// }
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
	for(uint i = 0; i < m_paramas.size(); i++) {
		// if(i % 10 == 0) {


			parama p = m_paramas[i];

			//draw u
			glColor3f(0.f, 1.f, 0.f);
			vec3 v = m_splinePath->getInterpolatedPoint(p.u);
			drawShape(v.x, v.y, 3, 4);

			//draw even intervals
			glColor3f(0.f, 0.f, 1.f);
			v = getUniformPoint(float(i)/float(m_paramas.size()));
			drawShape(v.x, v.y, 5, 3);
		// }
	}
}

vec3 Control::getUniformPoint(float t) {
	if(m_splinePath->size() == 0) {
		return vec3();
	}

	if(m_splineSpeed->size() != 0) {
		Spline *speed = getSpeedCurve();
		t = speed->getInterpolatedPoint(t).y;
		delete speed;
	}

	int i0 = 0;
	int i1 = 0;

	for(uint i = 0; i < m_paramas.size(); i++) {
		if(m_paramas[i].s >= t) {
			i1 = i;
			i0 = i-1; //segfault? -1?
			break;
		}
	}

	// cout << "i0:" << i0 << " i1:" << i1 << endl;

	parama p0 = m_paramas[i0];
	parama p1 = m_paramas[i1];

	float u = p0.u + (t-p0.s)/(p1.s-p0.s)*(p1.u-p0.u); 

	return m_splinePath->getInterpolatedPoint(u); //even step
	// return m_splinePath->getInterpolatedPoint(t); //spline step
}

//gets the speed curve in x:0..1 y:0..1 space;
Spline* Control::getSpeedCurve() {
	//initial values for min max
	vec3 p0 = m_splineSpeed->getInterpolatedPoint(0);
	float minX =  p0.x;
	float minY =  p0.y;
	float maxX = p0.x;
	float maxY = p0.y;

	//find bounding box around spline
	for(int i = 0; i < 200; i++) {
		vec3 p = m_splineSpeed->getInterpolatedPoint(float(i)/float(200));
		if(p.x < minX) { minX = p.x; }
		if(p.y < minY) { minY = p.y; }
		if(p.x > maxX) { maxX = p.x; }
		if(p.y > maxY) { maxY = p.y; }	
	}

	rect box;
	box.x = minX;
	box.y = minY;
	box.w = maxX-minX;
	box.h = maxY-minY;

	//build normalized spline
	Spline *s = new Spline();
	for(int i = 0; i < m_splineSpeed->size(); i++) {
		s->addPoint(box.normalized(m_splineSpeed->getControlPoint(i)));
	}
	return s;
}
