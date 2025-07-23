#include "./PathFinding.h"

PathFinding::PathFinding() {
	m_initializedStartGoal = false;
	m_foundGoal = false;
}

	void PathFinding::FindPath(glm::vec3 currentPos, glm::vec3 targetPos) {
		if (!m_initializedStartGoal) {
			for (int i = 0; i < m_openList.size(); i++) {
				delete m_openList[i];
			}
			m_openList.clear();

			for (int i = 0; i < m_visitedList.size(); i++) {
				delete m_visitedList[i];
			}
			m_visitedList.clear();

			for (int i = 0; i < m_pathToGoal.size(); i++) {
				delete m_pathToGoal[i];
			}
			m_pathToGoal.clear();

			SearchCell start;
			start.m_xcoord = currentPos.x;
			start.m_zcoord = currentPos.z;

			SearchCell goal;
			goal.m_xcoord = targetPos.x;
			goal.m_zcoord = targetPos.z;

			SetStartAndGoal(start, goal);
			m_initializedStartGoal = true;
		}
		if (m_initializedStartGoal) {
			ContinuePath();
		}
	}

void PathFinding::SetStartAndGoal(SearchCell start, SearchCell goal) {
	m_startCell = new SearchCell(start.m_xcoord, start.m_zcoord, nullptr);
	m_goalCell = new SearchCell(goal.m_xcoord, goal.m_zcoord, &goal);

	m_startCell->G = 0;
	m_startCell->H = m_startCell->GetManhattanDistance(m_goalCell);
	m_startCell->parent = nullptr;

	m_openList.push_back(m_startCell);
}

SearchCell* PathFinding::GetNextCell() {
	float bestF = 999999.0f;
	int cellIndex = -1;
	SearchCell* nextCell = nullptr;

	for (int i = 0; i < m_openList.size(); i++) {
		if (m_openList[i]->GetF() < bestF) {
			bestF = m_openList[i]->GetF();
			cellIndex = i;
		}
	}

	if (cellIndex >= 0) {
		nextCell = m_openList[cellIndex];
		m_visitedList.push_back(nextCell);
		m_openList.erase(m_openList.begin() + cellIndex);
	}

	return nextCell;
}

void PathFinding::PathOpened(int x, int z, float newCost, SearchCell* parent) {
	int id = z * WORLD_SIZE + x;

	// Skip if already visited
	for (int i = 0; i < m_visitedList.size(); i++) {
		if (id == m_visitedList[i]->m_id) {
			return;
		}
	}

	SearchCell* newChild = new SearchCell(x, z, parent);
	newChild->G = newCost;
	newChild->H = parent->GetManhattanDistance(m_goalCell);

	//for (int i = 0; i < m_openList.size(); i++) {
	//	if (id == m_openList[i]->m_id) {
	//		float newF = newChild->G + newCost + m_openList[i]->H;

	//		if (m_openList[i]->GetF() > newF) {
	//			m_openList[i]->G = newChild->G + newCost;
	//			m_openList[i]->parent = newChild;
	//		}
	//		else {
	//			delete newChild;
	//			return;
	//		}
	//	}
	//}

	for (int i = 0; i < m_openList.size(); i++) {
		if (id == m_openList[i]->m_id) {
			// Compare existing F with new F
			if (m_openList[i]->GetF() > newChild->GetF()) {
				// Better path found, update existing node
				m_openList[i]->G = newChild->G;
				m_openList[i]->parent = parent;
			}
			delete newChild;
			return;
		}
	}

	m_openList.push_back(newChild);
}

void PathFinding::ContinuePath() {
	if (m_openList.empty()) {
		return;
	}

	SearchCell* currentCell = GetNextCell();

	if (currentCell->m_id == m_goalCell->m_id) {
		m_goalCell->parent = currentCell->parent;
		SearchCell* getPath;

		for (getPath = m_goalCell; getPath != nullptr; getPath = getPath->parent) {
			m_pathToGoal.push_back(new glm::vec3(getPath->m_xcoord, 0, getPath->m_zcoord));
		}

		m_foundGoal = true;
		return;
	}
	else {
		// right side
		PathOpened(currentCell->m_xcoord + 1, currentCell->m_zcoord, currentCell->G + 1, currentCell);

		// left side
		PathOpened(currentCell->m_xcoord - 1, currentCell->m_zcoord, currentCell->G + 1, currentCell);

		// up side
		PathOpened(currentCell->m_xcoord + 1, currentCell->m_zcoord + 1, currentCell->G + 1, currentCell);

		// bottom side
		PathOpened(currentCell->m_xcoord, currentCell->m_zcoord - 1, currentCell->G + 1, currentCell);

		// left-up diagonal
		PathOpened(currentCell->m_xcoord - 1, currentCell->m_zcoord + 1, currentCell->G + 1.414f, currentCell);

		// right-up diagonal
		PathOpened(currentCell->m_xcoord + 1, currentCell->m_zcoord + 1, currentCell->G + 1.414f, currentCell);

		// left-down diagonal
		PathOpened(currentCell->m_xcoord - 1, currentCell->m_zcoord - 1, currentCell->G + 1.414f, currentCell);

		// right-down diagonal
		PathOpened(currentCell->m_xcoord + 1, currentCell->m_zcoord - 1, currentCell->G + 1.414f, currentCell);

		for (int i = 0; i < m_openList.size(); i++) {
			if (currentCell->m_id == m_openList[i]->m_id) {
				m_openList.erase(m_openList.begin() + i);
			}
		}
	}
}

glm::vec3 PathFinding::NextPathPos(glm::vec3 aiPos, glm::vec3 aiRadius) {
	if (m_pathToGoal.empty()) {
		return aiPos;
	}

	glm::vec3* nextTarget = m_pathToGoal.back();
	glm::vec3 distance = *nextTarget - aiPos;

	if (glm::length(distance) < glm::length(aiRadius)) {
		delete m_pathToGoal.back(); // free memory
		m_pathToGoal.pop_back();
	}

	return *nextTarget;

	/*int index = 1;

	glm::vec3 nextPos;
	nextPos.x = m_pathToGoal[m_pathToGoal.size() - index]->x;
	nextPos.y = 0;
	nextPos.z = m_pathToGoal[m_pathToGoal.size() - index]->z;

	glm::vec3 distance = nextPos - aiPos;

	if (index < m_pathToGoal.size()) {
		if (distance.length() < aiRadius) {
			m_pathToGoal.erase(m_pathToGoal.end() - index);
		}
	}

	return nextPos;*/
}

bool PathFinding::IsPathEmpty() const {
	return m_pathToGoal.empty();
}

void PathFinding::Reset() {
	m_initializedStartGoal = false;
}