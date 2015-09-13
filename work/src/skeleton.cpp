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

#include <string.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "comp308.hpp"
#include "skeleton.hpp"

using namespace std;
using namespace comp308;

namespace {
	void displayText( float x, float y, int r, int g, int b, const char *string ) {
		int j = strlen( string );
	 
		glColor3f( r, g, b );
		glRasterPos2f( x, y );
		for( int i = 0; i < j; i++ ) {
			glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, string[i] );
		}
	}
}

Skeleton::Skeleton(string filename) {
	bone b = bone();
	b.id = 1;
	b.name = "root";
	b.freedom |= dof_rx;
	b.freedom |= dof_ry;
	b.freedom |= dof_rz;
	b.freedom |= dof_root;
	m_bones.push_back(b);
	readASF(filename);
}

// [Assignment2] you may need to revise this function
void Skeleton::renderSkeleton() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	//Create a new GLUquadric object; to allow you to draw cylinders
	GLUquadric *quad = gluNewQuadric();
	if (quad == 0) {
		cerr << "Not enough memory to allocate space to draw" << endl;
		exit(EXIT_FAILURE);
	}

	//World xyz lines
	glBegin(GL_LINES);
		//x
		glColor3f(1.0f, 0.0f, 0.0f);
		for(uint i = 0; i < 1; i++) {
			glVertex3f(0.0f, 0.0f, float(i));
			glVertex3f(100.0f, 0.0f, float(i));
		}

		//y
		glColor3f(0.0f, 1.0f, 0.0f);
		for(uint i = 0; i < 1; i++) {
			glVertex3f(float(i), 0.0f, 0.0f);
			glVertex3f(float(i), 100.0f, 0.0f);
		}

		//z
		glColor3f(0.0f, 0.0f, 1.0f);
		for(uint i = 0; i < 1; i++) {
			glVertex3f(0.0f, float(i), 0.0f);
			glVertex3f(0.0f, float(i), 100.0f);
		}
	glEnd();

	glPushMatrix();
		//move root into position
		vec3 trans;
		if(m_applyRootMotion) {
			trans = m_position;
		} else {
			// trans = m_bones[0].translation;	
			trans = m_bones[0].xf.p + m_bones[0].translation;	
		} 
		glTranslatef(trans.x, trans.y, trans.z);

		//Actually draw the skeleton
		renderBone(&m_bones[0], quad);

		// cout << m_bones[0].translation << endl;

	glPopMatrix();

	// Clean up
	gluDeleteQuadric(quad);
	glPopMatrix();

	// displayText(1, 1, 1, 1, 1, "Hello World");

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( 0, 640, 0, 480 );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	// glRasterPos2i( 10, 1014 );  // move in 10 pixels from the left and bottom edges
	// for ( int i = 0; i < len; ++i ) {
	//     glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, menu[i]);
	// }
	if(m_selectedBone != -1) {
		bone b = m_bones[m_selectedBone];
		// stringstream s;
		// s << b.name << " " << b.rotation << endl;
		string s = string(b.name) + " (" + to_string(int(b.rotation.x)) + "," + to_string(int(b.rotation.y)) + "," + to_string(int(b.rotation.z)) + ")";
		displayText(10, 10, 1, 1, 1, s.c_str());
	}

	glPopMatrix();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
}


//-------------------------------------------------------------
// [Assignment 2] :
// Should render each bone by drawing the axes, and the bone
// then translating to the end of the bone and drawing each
// of it's children. Remember to use the basis rotations
// before drawing the axes (and for Completion, rotation).
// The actual bone is NOT drawn with the basis rotation.
//
// Should not draw the root bone (because it has zero length)
// but should go on to draw it's children
//-------------------------------------------------------------
void Skeleton::renderBone(bone *b, GLUquadric* q) {
	// YOUR CODE GOES HERE
	// ...

	// if(b->freedom & dof_root) {
	// 	cout << "Root" << endl;
	// }

	float r =  0.012f;

	glPushMatrix();

		// cout << b->name << endl;
		// cout << b->rotation << endl;
		
		//basis		
		glRotatef(b->basisRot.z, 0.f, 0.f, 1.f);
		glRotatef(b->basisRot.y, 0.f, 1.f, 0.f);
		glRotatef(b->basisRot.x, 1.f, 0.f, 0.f);

		//draw sphare
		if(b->selected) {
			glColor3f(0.8f, 0.0f, 0.8f);
		} else {
			glColor3f(0.0f, 1.0f, 1.0f);
		}
		gluSphere(q, 1.2f * r, 16, 16);

		if(b->name == "head") {
			glColor3f(1.0f, 1.0f, 0.0f);
			gluSphere(q, 10.0f * r, 16, 16);
		}

		//draw gimbal
		renderGimbal(q, r, b->hl);

		//rotation
		glRotatef(b->xf.r.z + b->rotation.z, 0.f, 0.f, 1.f);
		glRotatef(b->xf.r.y + b->rotation.y, 0.f, 1.f, 0.f);
		glRotatef(b->xf.r.x + b->rotation.x, 1.f, 0.f, 0.f);
		// glRotatef(b->rotation.z, 0.f, 0.f, 1.f);
		// glRotatef(b->rotation.y, 0.f, 1.f, 0.f);
		// glRotatef(b->rotation.x, 1.f, 0.f, 0.f);

		//inverse basis
		glRotatef(-b->basisRot.x, 1.f, 0.f, 0.f);
		glRotatef(-b->basisRot.y, 0.f, 1.f, 0.f);
		glRotatef(-b->basisRot.z, 0.f, 0.f, 1.f);

		//draw bone
		// if((b->freedom & dof_root) == 0)
		glPushMatrix();
			if(b->selected) {
				glColor3f(1.0f, 1.0f, 0.0f);
			} else {
				glColor3f(0.65f, 0.65f, 0.65f);
			}

			//calculate rotation axis and angle
			vec3 axis = cross(vec3(0,0,1), b->boneDir);
			float angle = acos(dot(vec3(0,0,1), b->boneDir)/length(b->boneDir));

			//rotate
			glRotatef(angle * 180.0f / 3.14159265359f, axis.x, axis.y, axis.z);
		
			gluCylinder(q, r, r/3.0, b->length, 16, 16);
		glPopMatrix();

		//translate along bone
		vec3 trans = b->boneDir * b->length;
		glTranslatef(trans.x, trans.y, trans.z);

		//render children
		for(uint i = 0; i < b->children.size(); i++) {
			renderBone(b->children[i], q);
		}
	glPopMatrix();

}

void Skeleton::renderGimbal(GLUquadric* q, double r, highlight h) {
	//draw z axis
	if(h.z) {
		glColor3f(0.5f, 0.5f, 1.0f);
	} else {
		glColor3f(0.0f, 0.0f, 1.0f);
	}
	renderAxisArrow(q, r);

	//draw x axis
	glPushMatrix();
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		if(h.x) {
			glColor3f(1.0f, 0.5f, 0.5f);
		} else {
			glColor3f(1.0f, 0.0f, 0.0f);
		}
		renderAxisArrow(q, r);
	glPopMatrix();

	//draw y axis
	glPushMatrix();
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		if(h.y) {
			glColor3f(0.5f, 1.0f, 0.5f);
		} else {
			glColor3f(0.0f, 1.0f, 0.0f);
		}
		renderAxisArrow(q, r);
	glPopMatrix();
}

void Skeleton::renderAxisArrow(GLUquadric* q, double r) {
	gluCylinder(q, r*0.3, r*0.3, r*4, 16, 16);
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, float(r*4));
		gluCylinder(q, r*0.6, 0.0, r*1.2, 16, 16);
	glPopMatrix();
}


// Helper method for retreiving and trimming the next line in a file.
// You should not need to modify this method.
namespace {
	string nextLineTrimmed(istream &file) {
		// Pull out line from file
		string line;
		getline(file, line);
		// Remove leading and trailing whitespace and comments
		size_t i = line.find_first_not_of(" \t\r\n");
		if (i != string::npos) {
			if (line[i] != '#') {
				return line.substr(i, line.find_last_not_of(" \t\r\n") - i + 1);
			}
		}
		return "";
	}

	bool isNumber(string s) {
		uint i = 0;
		for(;i < s.length() && isdigit(s[i]); i++);
		return s.length() != 0 && i == s.length();
	}

	//http://stackoverflow.com/questions/236129/split-a-string-in-c
	std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    	std::stringstream ss(s);
    	std::string item;
    	while (std::getline(ss, item, delim)) {
        	elems.push_back(item);
    	}
    	return elems;
	}

    std::vector<std::string> split(const std::string &s, char delim) {
    	std::vector<std::string> elems;
    	split(s, delim, elems);
    	return elems;
	}
}


int Skeleton::findBone(string name) {
	for (size_t i = 0; i < m_bones.size(); i++)
		if (m_bones[i].name == name)
			return i;
	return -1;
}


void Skeleton::readASF(string filename) {

	ifstream file(filename);

	if (!file.is_open()) {
		cerr << "Failed to open file " <<  filename << endl;
		throw runtime_error("Error :: could not open file.");
	}

	cout << "Reading file" << filename << endl;

	// good() means that failbit, badbit and eofbit are all not set
	while (file.good()) {

		// Pull out line from file
		string line = nextLineTrimmed(file);

		// Check if it is a comment or just empty
		if (line.empty() || line[0] == '#')
			continue;
		else if (line[0] == ':') {
			// Line starts with a ':' character so it must be a header
			readHeading(line, file);
		} else {
			// Would normally error here, but becuase we don't parse
			// every header entirely we will leave this blank.
		}
	}

	cout << "Completed reading skeleton file" << endl;
	// cout << "bone ids" << endl;

	// for(uint i = 0; i < m_bones.size(); i++) {
	// 	cout << m_bones[i].id << endl;
	// }
}


void Skeleton::readHeading(string headerline, ifstream &file) {

	string head;
	istringstream lineStream(headerline);
	lineStream >> head; // get the first token from the stream

	// remove the ':' from the header name
	if (head[0] == ':')
		head = head.substr(1);

	if (lineStream.fail() || head.empty()) {
		cerr << "Could not get heading name from\"" << headerline << "\", all is lost" << endl;
		throw runtime_error("Error :: could not parse .asf file.");
	}

	if (head == "version") {
		//version string - must be 1.10
		string version;
		lineStream >> version;
		if (lineStream.fail() || version != "1.10") {
			cerr << "Invalid version: \"" << version << "\" must be 1.10" << endl;
			throw runtime_error("Error :: invalid .asf version.");
		}
	}
	else if (head == "name") {
		// This allows the skeleton to be called something
		// other than the file name. We don't actually care
		// what the name is, so can ignore this whole section
	}
	else if (head == "documentation") {
		// Documentation section has no meaningful information
		// only of use if you want to copy the file. So we skip it
	}
	else if (head == "units") {
		// Has factors for the units to be able to model the
		// real person, these must be parsed correctly. Only
		// really need to check if deg or rad, but that is 
		// not needed for this assignment.

		// We are going to assume that the units:length feild
		// is 0.45, and that the angles are in degrees
	}
	else if (head == "root") {
		// Read in information about root. Let's just assume
		// it'll be at the origin for this assignment.
	}
	else if (head == "bonedata") {
		// Read in each bone until we get to the
		// end of the file or a new header
		string line = nextLineTrimmed(file);
		while (file.good() && !line.empty()) {
			if (line[0] == ':') {
				// finished our reading of bones
				// read next header and return
				return readHeading(line, file);
			}
			else if (line == "begin") {
				// Read the bone data
				readBone(file);
			}
			else {
				cerr << "Expected 'begin' in bone data, found \"" << line << "\"";
				throw runtime_error("Error :: could not parse .asf file.");
			}
			line = nextLineTrimmed(file);
		}
	}
	else if (head == "hierarchy") {
		// Description of how the bones fit together
		// Read in each line until we get to the
		// end of the file or a new header
		string line = nextLineTrimmed(file);
		while (file.good() && !line.empty()) {
			if (line[0] == ':') {
				// finished our reading of bones
				// read next header and return
				return readHeading(line, file);
			}
			else if (line == "begin") {
				// Read the entire hierarchy
				readHierarchy(file);
			}
			else {
				cerr << "Expected 'begin' in hierarchy, found \"" << line << "\"";
				throw runtime_error("Error :: could not parse .asf file.");
			}
			line = nextLineTrimmed(file);
		}
	}
	else {
		// Would normally error here, but becuase we don't parse
		// every header entirely we will leave this blank.
	}
}


void Skeleton::readBone(ifstream &file) {
	// Create the bone to add the data to
	bone b;
	b.id = m_bones.size()+1;

	string line = nextLineTrimmed(file);
	while (file.good()) {
		if (line == "end") {
			// End of the data for this bone
			// Push the bone into the vector
			m_bones.push_back(b);
			return;
		}
		else {
			
			string head;
			istringstream lineStream(line);
			lineStream >> head; // Get the first token

			if (head == "name") {
				// Name of the bone
				lineStream >> b.name;
			}
			else if (head == "direction") {
				// Direction of the bone
				lineStream >> b.boneDir.x >> b.boneDir.y >> b.boneDir.z;
				b.boneDir = normalize(b.boneDir); // Normalize here for consistency
			}
			else if (head == "length") {
				// Length of the bone
				float length;
				lineStream >> length;
				length *= (1.0/0.45);  // scale by 1/0.45 to get actual measurements
				length *= 0.0254;      // convert from inches to meters
				b.length = length;
			}
			else if (head == "dof") {
				// Degrees of Freedom of the joint (rotation)
				while (lineStream.good()) {
					string dofString;
					lineStream >> dofString;
					if (!dofString.empty()) {
						// Parse each dof string
						if      (dofString == "rx") b.freedom |= dof_rx;
						else if (dofString == "ry") b.freedom |= dof_ry;
						else if (dofString == "rz") b.freedom |= dof_rz;
						else throw runtime_error("Error :: could not parse .asf file.");
					}
				}
			}
			else if (head == "axis") {
				// Basis rotations 
				lineStream >> b.basisRot.x >> b.basisRot.y >> b.basisRot.z;
			}
			else if (head == "limits") {
				// Limits for each of the DOF
				// Assumes dof has been read first
				// You can optionally fill this method out
			}

			// Because we've tried to parse numerical values
			// check if we've failed at any point
			if (lineStream.fail()) {
				cerr << "Unable to parse \"" << line << "\"";
				throw runtime_error("Error :: could not parse .asf file.");
			}
		}

		// Get the next line
		line = nextLineTrimmed(file);
	}

	cerr << "Expected end in bonedata, found \"" << line << "\"";
	throw runtime_error("Error :: could not parse .asf file.");
}


void Skeleton::readHierarchy(ifstream &file) {
	string line = nextLineTrimmed(file);
	while (file.good()) {
		if (line == "end") {
			// End of hierarchy
			return;
		}
		else if (!line.empty()) {
			// Read the parent node
			string parentName;
			istringstream lineStream(line);
			lineStream >> parentName;

			// Find the parent bone and have a pointer to it
			int parentIndex = findBone(parentName);

			if (parentIndex < 0) {
				cerr << "Expected a valid parent bone name, found \"" << parentName << "\"" << endl;
				throw runtime_error("Error :: could not parse .asf file.");
			}

			//Read the connections
			string childName;
			lineStream >> childName;
			while (!lineStream.fail() && !childName.empty()) {

				int childIndex = findBone(childName);

				if (childIndex < 0) {
					cerr << "Expected a valid child bone name, found \"" << childName << "\"" << endl;
					throw runtime_error("Error :: could not parse .asf file.");
				}

				// Set a POINTER to the child to be recorded in the parents
				m_bones[parentIndex].children.push_back(&m_bones[childIndex]);
				
				// Get the next child
				lineStream >> childName;
			}
		}
		line = nextLineTrimmed(file);
	}
	cerr << "Expected end in bonedata, found \"" << line << "\"";
	throw runtime_error("Error :: could not parse .asf file.");
}



//-------------------------------------------------------------
// [Assignment 2] :
// Complete the following method to load data from an *.amc file
//-------------------------------------------------------------
void Skeleton::readAMC(string filename) {

	ifstream file(filename);

	if (!file.is_open()) {
		cerr << "Failed to open file " <<  filename << endl;
		// throw runtime_error("Error :: could not open file.");
		return; //exit if can't load
	}

	//wipe previous frames
	m_frames.clear();
	m_frame = 0;
	m_position = vec3(0,0,0);
	m_applyRootMotion = false;

	cout << "Reading file" << filename << endl;

	vector<frame> frames;

	// good() means that failbit, badbit and eofbit are all not set
	while (file.good()) {

		string line = nextLineTrimmed(file);

		// cout << line << " " << isNumber(line) << endl;

		//empty or comment
		if(line.empty() || line[0] == '#') {
			continue;
		} else if(line[0] == ':') {
			//header
		} else {
			//build frame
			if(!isNumber(line)) {
				cout << "Expected a frame number. Got: " << line << endl;
				continue;
			}

			while(isNumber(line = readFrame(line, file)));
			cout << "Frames read: " << m_frames.size() << endl;
		}

	}

	// cout << "Setting root offset" << endl;
	// if(!m_frames.empty()) {
	// 	frame f = m_frames[0];
	// 	for(uint i = 0; i < f.names.size(); i++) {
	// 		if(f.names[i] == "root") {
	// 			m_rootOffset = vec3(f.values[i][0], f.values[i][1], f.values[i][2]);
	// 			m_rootOffset *= m_scaleFactor;
	// 		}
	// 	}
	// }

	cout << "Completed reading skeleton file" << endl;
}

string Skeleton::readFrame(string name, ifstream &file) {
	frame f;
	f.name = name;

	string line;
	while(file.good() && !isNumber(line = nextLineTrimmed(file))) {
		if(line.empty()) {
			continue;
		}

		vector<string> elems = split(line, ' ');

		//add name
		f.names.push_back(elems[0]);

		//add values
		vector<float> values;
		for(uint i = 1; i < elems.size(); i++) {
			values.push_back(stof(elems[i]));
		}

		f.values.push_back(values);
	}
	m_frames.push_back(f);
	
	// cout << "ended read frame on: " << line << endl;
	return line;
}

void Skeleton::readConfig(string fname) {
	ifstream file(fname);

	if (!file.is_open()) {
		cerr << "Failed to open file " <<  fname << endl;
		// throw runtime_error("Error :: could not open file.");
		return; //exit if can't load
	}

	//wipe previous frames
	m_frames.clear();
	m_frame = 0;
	m_position = vec3(0,0,0);
	m_applyRootMotion = false;

	cout << "Reading file" << fname << endl;

	int i = 0;

	// good() means that failbit, badbit and eofbit are all not set
	while (file.good()) {

		string frameName = nextLineTrimmed(file);

		ifstream frameStream(frameName);

		if (!frameStream.is_open()) {
			cerr << "Failed to open file " <<  frameName << endl;
			// throw runtime_error("Error :: could not open file.");
			return; //exit if can't load
		} else {
			cout << "Reading frame " << frameName << endl;
		}

		string line = nextLineTrimmed(frameStream);

		// if(!isNumber(line)) {
		// 	cout << "Expected a frame number. Got: " << line << endl;
		// 	continue;
		// }

		readFrame(to_string(i), frameStream);

		i++;
	}

	cout << "Frames read: " << i << endl;
	cout << "Completed reading config file" << endl;
}

/*********
 * Poses *
 *********/

void Skeleton::toggleRootMotion() {
	m_applyRootMotion = !m_applyRootMotion;
	m_position = m_bones[findBone("root")].translation;
}

bool Skeleton::hasAnimation() {
	return !m_frames.empty();
}

void Skeleton::resetAnimation() {
	m_frame = 0;
	m_delta = 0;
	displayFrame(m_frame);
}

void Skeleton::tick(float delta) {
	if(m_frames.size() == 0) {
		return; //no frames
	}

	//1-m_delta to get delta going from b to a
	bool reverse = false;
	if(delta < 0) {
		reverse = true;
		if(!m_reverse) {
			m_reverse = true;
			m_delta = 1-m_delta;
		}
		delta = -delta;
	} else if(m_reverse) {
		m_reverse = false;
		m_delta = 1-m_delta;
	}

	m_delta += delta;
	if(m_delta >= m_frameLength) {
		m_delta = 1.f;
	}

	// frame *f0 = &m_frames[m_frame == 0 ? m_frames.size()-1 : m_frame-1];
	// frame *f1 = &m_frames[m_frame];

	frame *f0 = &m_frames[m_frame];
	frame *f1 = &m_frames[(m_frame+1) % m_frames.size()];

	frame f;
	f.name = f0->name;

	for(uint i = 0; i < f0->names.size(); i++) {
		string f0_name = f0->names[i];
		vector<float> f0_v = f0->values[i];
		vector<float> f1_v = f1->values[i];
		
		if(reverse) {
			vector<float> tmp = f0_v;
			f0_v = f1_v;
			f1_v = tmp;
		}
		
		f.names.push_back(f0_name);

		vector<float> values;

		for(uint j = 0; j < f0_v.size(); j++) {
			values.push_back(m_delta*f1_v[j] + (1-m_delta)*f0_v[j]);
		}

		f.values.push_back(values);
	}

	renderFrame(&f);

	if(m_delta == 1.f) {
		if(reverse) {
			if(m_frame > 0) {
				m_frame = m_frame-1;
			} else {
				m_frame = m_frames.size()-1;
			}
		} else {
			m_frame = (m_frame+1) % m_frames.size();
		}
		m_delta = 0.f;	
	}
	

	// if(step > 0) {
	// 	//count up with wrap around
	// 	m_frame = (m_frame+step) % m_frames.size();
	// } else if (step < 0) {
	// 	//count down with wrap around
	// 	m_frame = (m_frame + m_frames.size()) % (m_frames.size() +1);
	// }
	// vec3 before = m_bones[findBone("root")].translation;
	// displayFrame(m_frame);
	// vec3 after = m_bones[findBone("root")].translation;
	// if(m_frame != 0 && m_frame != 1 && m_frame != m_frames.size()-1) {
	// 	m_position += after - before;
	// }
	// cout << m_position << endl;
	// cout << m_frame << "/" << m_frames.size() << endl;
}

void Skeleton::dumpCurrentFrame(string fname) {
	ofstream file(fname); //, fstream::app);
	if(!file.is_open()) {
		cout << "Couldn't create " << fname << endl;
	} else {
		cout << "Writing frame to " << fname << endl;
		// if(m_frames.size() != 0) {
		// 	frame f = m_frames[m_frame];

		// 	file << f.name << endl;
		// 	for(uint i = 0; i < f.names.size(); i++) {
		// 		file << f.names[i] << " ";

		// 		vector<float> values = f.values[i];
		// 		for(uint j = 0; j < values.size(); j++) {
		// 			file << values[j] << " ";
		// 		}

		// 		file << endl;
		// 	}
		// 	file.close();
		// } else {
			//save from bones

			file << 0 << endl; //frame number

			writeBone(&file, &m_bones[0]);
		// }
	}
	
}

void Skeleton::writeBone(ofstream *file, bone *b) {
	// (*file) << "Hello World" << endl;
	vec3 t = b->translation;
	vec3 r = b->rotation;

	cout << b->name << " " << int(b->freedom)<<  " " << int(dof_none) << endl;

	if(b->freedom == int(dof_none)) {
		//don't write
		cout << " not writing" << endl;
	} else {
		(*file) << b->name;

		if(b->freedom & dof_root) {
			(*file) << " " << t.x << " " << t.y << " " << t.z <<
					   " " << r.x << " " << r.y << " " << r.z;
		} else {
			if(b->freedom & dof_rx) {
				(*file) << " " << r.x;
			}

			if(b->freedom & dof_ry) {
				(*file) << " " << r.y;
			}

			if(b->freedom & dof_rz) {
				(*file) << " " << r.z;
			}
		}	

		(*file) << endl;
	}

	//write children
	// if(b->freedom & dof_root) {
		// if(b->children.size() != 0) {
		// 	for(int i = b->children.size()-1; i >= 0 ; i--) {
		// 		writeBone(file, b->children[i]);
		// 	}
		// }
	// } else {
		for(uint i = 0; i < b->children.size(); i++) {
			writeBone(file, b->children[i]);
		}
	// }
}

void Skeleton::displayFrame(uint i) {
	cout << i << " " << m_frames.size() << endl;
	if(i < m_frames.size()) {
		renderFrame(&m_frames[i]);
	}
}

void Skeleton::renderFrame(frame *f) {
	for(uint i = 0; i < f->names.size(); i++) {
		bone *b = &m_bones[findBone(f->names[i])];
		vector<float> values = f->values[i];

		if(b->freedom & dof_root) {
			//assume 6 values;
			// b->translation.x = values[0];// * m_scaleFactor;
			// b->translation.y = values[1];// * m_scaleFactor;
			// b->translation.z = values[2];// * m_scaleFactor;
			//inches to meters
			b->translation *= ((1.0f/0.45f)*2.54f/100.0f);

			b->rotation.x = values[3];
			b->rotation.y = values[4];
			b->rotation.z = values[5];
		} else if(b->freedom & dof_none) {
			continue;
		} else {
			uint index = 0;
			
			if(b->freedom & dof_rx) {
				b->rotation.x = values[index++];
			}

			if(b->freedom & dof_ry) {
				b->rotation.y = values[index++];
			}

			if(b->freedom & dof_rz) {
				b->rotation.z = values[index++];
			}
		}
	}
}













// [Assignment2] you may need to revise this function
void Skeleton::pickRenderSkeleton() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	//Create a new GLUquadric object; to allow you to draw cylinders
	GLUquadric *quad = gluNewQuadric();
	if (quad == 0) {
		cerr << "Not enough memory to allocate space to draw" << endl;
		exit(EXIT_FAILURE);
	}

	glPushMatrix();
		//move root into position
		vec3 trans;
		if(m_applyRootMotion) {
			trans = m_position;
		} else {
			trans = m_bones[0].translation;	
		} 
		glTranslatef(trans.x, trans.y, trans.z);

		//Actually draw the skeleton
		pickRenderBone(&m_bones[0], quad);

		// cout << m_bones[0].translation << endl;

	glPopMatrix();

	// Clean up
	gluDeleteQuadric(quad);
	glPopMatrix();
}


//-------------------------------------------------------------
// [Assignment 2] :
// Should render each bone by drawing the axes, and the bone
// then translating to the end of the bone and drawing each
// of it's children. Remember to use the basis rotations
// before drawing the axes (and for Completion, rotation).
// The actual bone is NOT drawn with the basis rotation.
//
// Should not draw the root bone (because it has zero length)
// but should go on to draw it's children
//-------------------------------------------------------------
void Skeleton::pickRenderBone(bone *b, GLUquadric* q) {
	// YOUR CODE GOES HERE
	// ...

	// if(b->freedom & dof_root) {
	// 	cout << "Root" << endl;
	// }

	float r =  0.012f;

	glPushMatrix();

		// cout << b->name << endl;
		// cout << b->rotation << endl;
		
		//basis		
		glRotatef(b->basisRot.z, 0.f, 0.f, 1.f);
		glRotatef(b->basisRot.y, 0.f, 1.f, 0.f);
		glRotatef(b->basisRot.x, 1.f, 0.f, 0.f);

		//draw sphare
		glColor3f(float(b->id>>16&0xFF)/255, 
				  float(b->id>> 8&0xFF)/255, 
				  float(b->id    &0xFF)/255);
		gluSphere(q, 1.2f * r, 16, 16);

		//rotation
		glRotatef(b->rotation.z, 0.f, 0.f, 1.f);
		glRotatef(b->rotation.y, 0.f, 1.f, 0.f);
		glRotatef(b->rotation.x, 1.f, 0.f, 0.f);

		//inverse basis
		glRotatef(-b->basisRot.x, 1.f, 0.f, 0.f);
		glRotatef(-b->basisRot.y, 0.f, 1.f, 0.f);
		glRotatef(-b->basisRot.z, 0.f, 0.f, 1.f);

		//draw bone
		glPushMatrix();
			//calculate rotation axis and angle
			vec3 axis = cross(vec3(0,0,1), b->boneDir);
			float angle = acos(dot(vec3(0,0,1), b->boneDir)/length(b->boneDir));

			//rotate
			glRotatef(angle * 180.0f / 3.14159265359f, axis.x, axis.y, axis.z);
		
			gluCylinder(q, r, r/3.0, b->length, 16, 16);
		glPopMatrix();

		//translate along bone
		vec3 trans = b->boneDir * b->length;
		glTranslatef(trans.x, trans.y, trans.z);

		//render children
		for(uint i = 0; i < b->children.size(); i++) {
			pickRenderBone(b->children[i], q);
		}
	glPopMatrix();

}