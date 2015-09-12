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

#pragma once

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "comp308.hpp"



// Needed for Completion/Challenge
// We use bitmasking to work out the Degrees of Freedom
// To work out if a bone b has a y-axis dof, simply:
//     if (b.freedom & dof_ry) {...}
//
// To add and subtract degrees of freedom, respectively:
//     b.freedom |= dof_rx
//     b.freedom ^= dof_rx
using dof_set = unsigned int;

enum dof {
	dof_none = 0,
	dof_rx = 1,
	dof_ry = 2,
	dof_rz = 4,
	dof_root = 8 // Root has 6, 3 translation and 3 rotation
};

struct highlight {
	bool x = false;
	bool y = false;
	bool z = false;
};

// Type to represent a bone
struct bone {
	std::string name;
	float length = 0;             // Length of the bone
	comp308::vec3 boneDir;        // Direction of the bone
	comp308::vec3 basisRot;       // Euler angle rotations for the bone basis
	dof_set freedom = dof_none;   // Degrees of freedom for the joint rotation
	std::vector<bone *> children; // Pointers to bone children

	// Completion and Challenge
	comp308::vec3 rotation;       // Rotation of joint in the basis (degrees)

	// Challenge
	comp308::vec3 translation;    // Translation (Only for the Root)
	comp308::vec3 rotation_max;   // Maximum value for rotation for this joint (degrees)
	comp308::vec3 rotation_min;   // Minimum value for rotation for this joint (degrees)

	uint id;
	bool selected = false;
	highlight hl;
};

//Type to represent an AMC frame
struct frame {
	std::string name;
	// uint size;
	//i maps 1 to 1 in names and values
	std::vector<std::string> names;
	std::vector<std::vector<float>> values;
};

class Skeleton {

private:
	std::vector<bone> m_bones;
	std::vector<frame> m_frames;
	uint m_frame = 0;

	int m_selectedBone = -1;

	// comp308::vec3 m_rootOffset = comp308::vec3(0,0,0);
	float m_scaleFactor = (1.0f/0.45f)*2.54f/100.0f; //0.056f; //scale root translation from AMC data

	comp308::vec3 m_position = comp308::vec3(0,0,0);

	bool m_applyRootMotion = false;

	// Helper method
	int findBone(std::string);
	
	// Reading code TODO change
	void readASF(std::string);
	void readHeading(std::string, std::ifstream&);
	void readBone(std::ifstream&);
	void readHierarchy(std::ifstream&);

	std::string readFrame(std::string, std::ifstream&);

	void renderBone(bone *, GLUquadric*);
	void renderGimbal(GLUquadric*, double r, highlight h);
	void renderAxisArrow(GLUquadric*, double r);

	void renderFrame(frame *);

	void writeBone(std::ofstream *, bone *);
public:
	Skeleton(std::string);
	void renderSkeleton();
	void readAMC(std::string);

	//poses
	void displayFrame(uint);

	bool hasAnimation();
	void resetAnimation();
	void tick(float);
	void dumpCurrentFrame(std::string);

	void toggleRootMotion();

	void pickRenderSkeleton();
	void pickRenderBone(bone *, GLUquadric*);
	bone* pickBone(uint id) {
		for(uint i = 0; i < m_bones.size(); i++) {
			uint me = m_bones[i].id;
			if((me>>16&0xFF) == (id>>16&0xFF) &&
			   (me>> 8&0xFF) == (id>> 8&0xFF) &&
			   (me    &0xFF) == (id    &0xFF)) {
				return &m_bones[i];
			}
		}
		return nullptr;
	}

	void toggleBone(std::string name) {
		int i = findBone(name);
		if(!m_bones[i].selected) {
			for(uint j = 0; j < m_bones.size(); j++) {
				m_bones[j].selected = false;
			};
			m_bones[i].selected = true;
			m_bones[i].hl.x = true;
			m_selectedBone = i;
		} else {
			m_bones[i].selected = false;
			m_bones[i].hl.x = false;
			m_bones[i].hl.y = false;
			m_bones[i].hl.z = false;
			m_selectedBone = -1;
		}
	}

	void cycleHighlight() {
		if(m_selectedBone == -1) {
			return;
		}

		highlight h = m_bones[m_selectedBone].hl;

		dof_set freedom = m_bones[m_selectedBone].freedom;

		std::vector<uint> freedoms;
		int idx = -1;

		if(freedom & dof_rx) { freedoms.push_back(0); if(h.x) { idx = freedoms.size()-1; } }
		if(freedom & dof_ry) { freedoms.push_back(1); if(h.y) { idx = freedoms.size()-1; } }
		if(freedom & dof_rz) { freedoms.push_back(2); if(h.z) { idx = freedoms.size()-1; } }

		if(freedoms.size() == 1) { 
			return; //can't cycle if only 1 dof
		}

		//clear highlights
		m_bones[m_selectedBone].hl.x = false;
		m_bones[m_selectedBone].hl.y = false;
		m_bones[m_selectedBone].hl.z = false;

		if(idx != -1) {
			//we have freedoms
			idx = idx + 1;
			idx = idx % freedoms.size();
			std::cout << idx << std::endl;
			if(idx == 0) { m_bones[m_selectedBone].hl.x = true; }
			if(idx == 1) { m_bones[m_selectedBone].hl.y = true; }
			if(idx == 2) { m_bones[m_selectedBone].hl.z = true; }
		}


		// if(h.x) {
		// 	m_bones[m_selectedBone].hl.x = false;
		// 	m_bones[m_selectedBone].hl.y = true;			
		// } else if(h.y) {
		// 	m_bones[m_selectedBone].hl.y = false;
		// 	m_bones[m_selectedBone].hl.z = true;			
		// } else if(h.z) {
		// 	m_bones[m_selectedBone].hl.z = false;
		// 	m_bones[m_selectedBone].hl.x = true;			
		// }
	}

	void rotateSelectedAxis(float delta) {
		if(m_selectedBone == -1) {
			return;
		}

		highlight h = m_bones[m_selectedBone].hl;

		if(h.x) {
			m_bones[m_selectedBone].rotation.x += delta;
			if(m_bones[m_selectedBone].rotation.x >= 360) {
				m_bones[m_selectedBone].rotation.x = 0;
			} else if(m_bones[m_selectedBone].rotation.x < 0) {
				m_bones[m_selectedBone].rotation.x += 360;
			}
		} else if(h.y) {
			m_bones[m_selectedBone].rotation.y += delta;
			if(m_bones[m_selectedBone].rotation.y >= 360) {
				m_bones[m_selectedBone].rotation.y = 0;
			} else if(m_bones[m_selectedBone].rotation.y < 0) {
				m_bones[m_selectedBone].rotation.y += 360;
			}
		} else if(h.z) {
			m_bones[m_selectedBone].rotation.z += delta;
			if(m_bones[m_selectedBone].rotation.z >= 360) {
				m_bones[m_selectedBone].rotation.z = 0;
			} else if(m_bones[m_selectedBone].rotation.z < 0) {
				m_bones[m_selectedBone].rotation.z += 360;
			}
		} 
	}

	std::vector<bone> getBones() {
		return m_bones;
	}
};
