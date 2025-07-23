#pragma once

#include <math.h>

#define WORLD_SIZE 64

class SearchCell {
public:
	SearchCell() :
		parent(nullptr) {}
	SearchCell(int x, int z, SearchCell *_parent = nullptr) :
		m_xcoord(x), m_zcoord(z), parent(_parent), m_id(z * WORLD_SIZE + x), G(0), H(0) {}

	float GetF() { return G + H; };
	float GetManhattanDistance(SearchCell* nodeEnd) {
		float x = (float)abs(m_xcoord - nodeEnd->m_xcoord);
		float z = (float)abs(m_zcoord - nodeEnd->m_zcoord);
		return x + z;
	}

public:
	int m_xcoord;
	int m_zcoord;
	int m_id;
	SearchCell *parent;
	float G;
	float H;
};