//---------------------------------------------------------------------------
//
// Copyright (c) 2015 Taehyun Rhee, Joshua Scott, Ben Allen
//
// This software is provided 'as-is' for assignment of COMP308 in ECS,
// Victoria University of Wellington, without any express or implied warranty. 
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "comp308.hpp"
#include "skeleton.hpp"
#include "spline.hpp"
#include "control.hpp"

using namespace std;
using namespace comp308;


// Global variables
// 
GLuint g_winWidth  = 640;
GLuint g_winHeight = 480;
GLuint g_mainWindow = 0;

//Keys
//
bool g_isCtrl = false;
bool g_isShift = false;
bool g_isAlt = false;


// Projection values
// 
float g_fovy = 20.0;
float g_znear = 0.1;
float g_zfar = 1000.0;


// Mouse controlled Camera values
//
bool g_mouseDown = false;
vec2 g_mousePos;

bool g_rightMouseDown = false;
vec2 g_rightMousePos;

float g_yRotation = 0;
float g_xRotation = 0;
float g_zoomFactor = 1.0;

//Menu
//
enum MENU {
	PLAY = 0,
	PAUSE = 1,
	STOP = 2,
	REWIND = 4,
	FAST_FORWARD = 8,
	FAST_REWIND = 16,
};

MENU g_menuState = STOP;

// Animation
//
// Spline *spline = nullptr;
float g_time = 0.f;
float g_endTime = 4.f;

bool g_isRunning = false;

Control *g_control = nullptr;

Skeleton *g_skeleton = nullptr;

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

	bool checkPixel(int x, int y, unsigned char r, 
				unsigned char g, unsigned char b) {
		unsigned char pixel[3];

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel);

		return (pixel[0] == r && pixel[1] == g && pixel[2] == b);
	}

	static uint colorSeed = 1;
	static uint getColorID() {
		return colorSeed++;
	}

	static unsigned char getR(uint color) {
		return color >> 16 & 0xFF;
	}

	static unsigned char getG(uint color) {
		return color >> 8 & 0xFF;
	}

	static unsigned char getB(uint color) {
		return color & 0xFF;
	}

	bool checkPixel(int x, int y, uint color) {
		return checkPixel(x, y, getR(color), getG(color), getB(color));
	}

	uint getPixel(int x, int y) {
		unsigned char pixel[3];

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel);

		uint color = 0;
		color |= pixel[2];
		color |= (pixel[1] << 8);
		color |= (pixel[2] << 16);
		return color;
	}
}

// Sets up where and what the light is
// Called once on start up
// 
void initLight() {
	float direction[]	  = {0.0f, 0.0f, 1.0f, 0.0f};
	float diffintensity[] = {0.7f, 0.7f, 0.7f, 1.0f};
	float ambient[]       = {0.2f, 0.2f, 0.2f, 1.0f};

	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffintensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);	
	
	
	glEnable(GL_LIGHT0);
}


// Sets up where the camera is in the scene
// Called every frame
// 
void setUpCamera() {
	// Set up the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(g_fovy, float(g_winWidth) / float(g_winHeight), g_znear, g_zfar);

	// glOrtho(0, g_winWidth, 0, g_winHeight, -1, 1);

	// Set up the view part of the model view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Load camera transforms

	glTranslatef(0.f, 0.f, -5 * g_zoomFactor);
	glRotatef(g_xRotation, 1, 0, 0);
	glRotatef(g_yRotation, 0, 1, 0);
	// glTranslatef(g_pan.x, -g_pan.y, -5 * g_zoomFactor);
}


// Draw function
//
void draw() {
	// cout << getColorID() << endl;

	// Set up camera every frame
	setUpCamera();

	// Black background
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable flags for normal rendering
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);


	// Set the current material (for all objects) to red
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); 
	glColor3f(1.0f,0.0f,0.0f);

	// glColor3f(1.f, 0.f, 0.f);
	// glBegin(GL_TRIANGLES);
	// 	glVertex3f(-1.f, -1.f, 0.f);
	// 	glVertex3f(1.f, -1.f, 0.f);
	// 	glVertex3f(0.f, 1.f, 0.f);
	// glEnd();

	g_control->render();

	switch(g_menuState) {
		case PLAY :
			g_skeleton->tick(0.016f);	
			break;
		case PAUSE : break;
		case STOP : break;
		case REWIND : 
			g_skeleton->tick(-0.016f);
			break;
		case FAST_FORWARD : 
			g_skeleton->tick(0.048f);	
			break;
		case FAST_REWIND : 
			g_skeleton->tick(-0.048f);	
			break;
	}

	float t = g_time/g_endTime;
	if(t > 1) {
		t = 1;
		g_time = 0;
	}
	g_time += 0.01f;

	//get path
	Spline *s = g_control->projectOnXZPlane();

	//draw path
	glColor3f(0.89f, 0.f, 0.f);
	glBegin(GL_LINES);
	vec3 a = s->getUniformPoint(0);
	for(int i = 1; i <= 1000; i++) {
		vec3 b = s->getUniformPoint(float(i)/1000);
		glVertex3f(a.x, a.y, a.z);
		glVertex3f(b.x, b.y, b.z);
		a = b;
	}
	glEnd();

	//get player point on spline
	float mod_t = g_control->modWithSpeed(t);
	vec3 v = s->getUniformPoint(mod_t);

	float sum = v.x + v.y;
	if(sum == sum) { //checknan
		// if(g_control->getSplinePath()->size() >= 2) {
		// 	g_skeleton->setPosition(v.x, v.y, v.z);
		// } else {
			g_skeleton->lerpPosition(v.x, v.y, v.z);
		// }
	}

	// float mod_t2 = g_control->modWithSpeed(t+0.01f);
	vec3 v2 = s->getUniformPoint(mod_t + 0.01f);
	float sum2 = v2.x + v2.y;
	if(sum2 == sum2) {
		if(mod_t + 0.01f > 1) {
			//if look at would be off the end
			g_skeleton->lookAt(v); 
		} else {
			g_skeleton->lookAt(v2);
		}
	}

	//cleanup
	delete s;

	g_skeleton->renderSkeleton();

	Spline* c_path = g_control->getSplinePath();
	Spline* c_spath = g_control->getSpeedSpline();
	Spline* c_snpath = g_control->getNormalizedSpeedSpline();

	float dt = t;

	vec3 c_spath_point = c_spath->getInterpolatedPoint(dt);
	vec3 c_snpath_point = c_snpath->getInterpolatedPoint(dt);
	vec3 c_path_point = c_path->getUniformPoint(c_snpath_point.y);
	vec3 c_path_point_plus = c_path->getUniformPoint(c_snpath_point.y+0.01f);
	vec3 c_asd = c_spath->getUniformPoint(c_snpath_point.y);

	//enter 2D mode
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, g_winWidth, 0, g_winHeight, -1, 1);

	//enter render mode
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(0.2f, 0.8f, 0.2f);
	drawShape(c_spath_point.x, c_spath_point.y, 6, 4);
	glColor3f(0.5f, 0.8f, 0.5f);
	drawShape(c_path_point.x, c_path_point.y, 12, 8);
	drawShape(c_path_point_plus.x, c_path_point_plus.y, 12, 8);
	drawShape(c_asd.x, c_asd.y, 12, 8);

	// if(c_spath->size() > 1) {
	// 	float minX = c_spath->getControlPoint(0).x;
	// 	float maxX = c_spath->getControlPoint(c_spath->size()-1).x;

	// 	float x = dt*maxX + (1-dt)*minX;

	// 	glBegin(GL_LINES);
	// 		glVertex2f(x, 0);
	// 		glVertex2f(x, g_winHeight);
	// 	glEnd();
	// }

	glPopMatrix();

	//exit 2D mode
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Disable flags for cleanup (optional)
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_NORMALIZE);
	glDisable(GL_COLOR_MATERIAL);

	glutSwapBuffers();

	// Queue the next frame to be drawn straight away
	glutPostRedisplay();
}


// Reshape function
// 
void reshape(int w, int h) {
    if (h == 0) h = 1;

	g_winWidth = w;
	g_winHeight = h;
    
    // Sets the openGL rendering window to match the window size
    glViewport(0, 0, g_winWidth, g_winHeight);  
}


//-------------------------------------------------------------
// [Assignment 2] :
// Modify the keyboardCallback function and additional files,
// to make your priman pose when the 'p' key is pressed.
//-------------------------------------------------------------

// Keyboard callback
// Called once per button state change
//

void keyboardCallback(unsigned char key, int x, int y) {
	// cout << "Keyboard Callback :: key=" << key << ", x,y=(" << x << "," << y << ")" << endl;
	// YOUR CODE GOES HERE
	// ...

	switch(key) {
		case 'a' :
			g_menuState = g_menuState == PLAY ? PAUSE : PLAY;
			break;
		case 'c' :
			g_control->clear();
			break;
		case 'x' :
			g_skeleton->cycleHighlight();
			break;
		case 's' :
			string fname;
			cout << "Enter filename: ";
			cin >> fname;
			g_skeleton->dumpCurrentFrame(fname);
			break;
	}
}


// Special Keyboard callback
// Called once per button state change
//
void specialCallback(int key, int x, int y) {
	cout << "Special Callback :: key=" << key << ", x,y=(" << x << "," << y << ")" << endl;
	// Not needed for this assignment, but useful to have later on
	switch(key) {
		case 114 : //left ctrl
			g_isCtrl = true;
			break;
		case 112 : //left shift
			g_isShift = true;
			break;
		case 116 : //left alt
			g_isAlt = true;
			break;
	}
}

void specialUpCallback(int key, int x, int y) {
	switch(key) {
		case 114 : //left ctrl
			g_isCtrl = false;
			break;
		case 112 : //left shift
			g_isShift = false;
			break;
		case 116 : //left alt
			g_isAlt = false;
			break;
	}
}


// Mouse Button Callback function
// (x,y) :: (0,0) is top left and (g_winWidth, g_winHeight) is bottom right
// state :: 0 is down, 1 is up
// 
// Called once per button state change
// 
void mouseCallback(int button, int state, int x, int y) {
	if(g_isShift) {
		switch(button) {
			case 0: //left mb
				if(state == 0) {
					// animation->addPoint(float(x), float(g_winHeight-y));
					// spline->addPoint(vec3(x, g_winHeight-y, 0));
					g_control->onClick(x, g_winHeight-y);
				}
				break;
			case 3: //scroll foward/up
				
				break;
			case 4: //scroll back/down
				
				break;
		}
	} else if(g_isCtrl) {
		switch(button) {
			case 0: //leftmb
				if(state ==0) { //down
					glDisable(GL_TEXTURE_2D);
      				glDisable(GL_FOG);
      				glDisable(GL_LIGHTING);

      				setUpCamera();

      				g_skeleton->pickRenderSkeleton();

      				vector<bone> bones = g_skeleton->getBones();

      				for(uint i = 0; i < bones.size(); i++) {
      					if(checkPixel(x, y, bones[i].id)) {
      						g_skeleton->toggleBone(bones[i].name);
      						cout << "hit:" << bones[i].name << endl;
      					}
      				}

      				// if(checkPixel(x, y, ))

      				// uint color = getPixel(x, y);
      				// bone *b = g_skeleton->pickBone(color);
      				// if(b != nullptr) {
      				// 	cout << "hit:" << b->name << endl;
      				// }

     //  				uint color = getColorID();
     //  				cout<< color << " " << getB(color) << endl;

     //  				glColor3f(float(getR(color))/255, float(getG(color))/255, float(getB(color))/255);
					// glBegin(GL_TRIANGLES);
					// 	glVertex3f(-1.f, -1.f, 0.f);
					// 	glVertex3f(1.f, -1.f, 0.f);
					// 	glVertex3f(0.f, 1.f, 0.f);
					// glEnd();

					// unsigned char pixel[3];

					// GLint viewport[4];
					// glGetIntegerv(GL_VIEWPORT, viewport);

					// glReadPixels(x, viewport[3] - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel);

					// unsigned char c[3];
					// c[0] = 0; c[1] = 0; c[2] = 255;



					// if(pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 255) {
					// if(checkPixel(x, y, color)) {	
					// 	cout << "hit" << endl;
					// }
					// } else {
					// 	cout << "miss:" << static_cast<unsigned>(pixel[0]) << " " << static_cast<unsigned>(pixel[1]) << " " << static_cast<unsigned>(pixel[2]) << endl;
					// }
				}
				break;
		}
	} else {
		// //**camera controls**
		// //cout << "Mouse Callback :: button=" << button << ", state=" << state << ", (" << x << "," << y << ")" << endl;
		switch(button){

			case 0: //left mouse button
				g_mouseDown = (state==0);
				g_mousePos = vec2(x, y);
				break;
			case GLUT_RIGHT_BUTTON :
				g_rightMouseDown = (state==0);
				g_rightMousePos = vec2(x, y);
				break;
			case 3: //scroll foward/up
				g_zoomFactor /= 1.1;
				break;

			case 4: //scroll back/down
				g_zoomFactor *= 1.1;
				break;
		}
	}
}

// Mouse Motion Callback function
// Called once per frame if the mouse has moved and
// at least one mouse button has an active state
// 
void mouseMotionCallback(int x, int y) {
	// //**camera controls**
	// //cout << "Mouse Motion Callback :: (" << x << "," << y << ")" << endl;
	if(g_isShift) {

	} else {
		if (g_mouseDown) {
			vec2 dif = vec2(x,y) - g_mousePos;
			g_mousePos = vec2(x,y);
			g_yRotation += 0.3 * dif.x;
			g_xRotation += 0.3 * dif.y;
		}

		if(g_rightMouseDown) {
			vec2 dif = vec2(x,y) - g_rightMousePos;
			g_rightMousePos = vec2(x,y);
			g_skeleton->rotateSelectedAxis(0.3 * dif.y);
		}
	}
}

void menu(int item) {
	g_menuState = MENU(item);
	
	if(g_menuState == STOP) {
		if(g_skeleton->hasAnimation()) {
			g_skeleton->resetAnimation();
		}
	}
}

//Main program
// 
int main(int argc, char **argv) {
	if (argc < 2 ) {
		cout << "ASF filename expected" << endl << "    ./a2 priman.asf" << endl;
		abort(); // Unrecoverable error
	} else if (argc > 3) {
		cout << "Too many arguments, expected only ASF and AMC filenames" << endl;
		cout << "    ./a2 priman.asf priman.amc" << endl;
		abort(); // Unrecoverable error
	}

	// Initialise GL, GLU and GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);

	// Initialise window size and create window
	glutInitWindowSize(g_winWidth, g_winHeight);
	g_mainWindow = glutCreateWindow("COMP308 Assignment 2");

	// Initilise GLEW
	// must be done after creating GL context (glutCreateWindow in this case)
	GLenum err = glewInit();
	if (GLEW_OK != err) { // Problem: glewInit failed, something is seriously wrong.
		cerr << "Error: " << glewGetErrorString(err) << endl;
		abort(); // Unrecoverable error
	}

	cout << "Using OpenGL " << glGetString(GL_VERSION) << endl;
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;

	// Register functions for callback
	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);
	
	glutKeyboardFunc(keyboardCallback);
	glutSpecialFunc(specialCallback);
	glutSpecialUpFunc(specialUpCallback);

	glutMouseFunc(mouseCallback);
	glutMotionFunc(mouseMotionCallback);

	GLint menu_id = glutCreateMenu(menu);
	glutAddMenuEntry("Play", PLAY);
	glutAddMenuEntry("Pause", PAUSE);
	glutAddMenuEntry("Stop", STOP);
	glutAddMenuEntry("Rewind", REWIND);
	glutAddMenuEntry("Fast Forward", FAST_FORWARD);
	glutAddMenuEntry("Fast Rewind", FAST_REWIND);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);

	// Create a light on the camera
	initLight();

	//init
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	g_control = new Control(g_winWidth, g_winHeight);

	g_skeleton = new Skeleton(argv[1]);

	if(argc == 3) {
		g_skeleton->readConfig(argv[2]);
	}


	// Loop required by OpenGL
	// This will not return until we tell OpenGL to finish
	glutMainLoop();

	// Don't forget to delete all pointers that we made

	return 0;
}