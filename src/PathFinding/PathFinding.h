#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "./SearchCell.h"

class PathFinding {
public:
	PathFinding();

	void FindPath(glm::vec3 currentPos, glm::vec3 targetPos);
	glm::vec3 NextPathPos(glm::vec3 aiPos, glm::vec3 aiRadius);
	bool IsPathEmpty() const;
	void Reset();

	void ClearOpenList() { m_openList.clear(); };
	void ClearVisitedList() { m_visitedList.clear(); };
	void ClearPathToGoal() { m_pathToGoal.clear(); };

private:
	void SetStartAndGoal(SearchCell start, SearchCell goal);
	SearchCell* GetNextCell();
	void PathOpened(int x, int z, float newCost, SearchCell* parent);
	void ContinuePath();

	bool m_initializedStartGoal;
	bool m_foundGoal;
	SearchCell* m_startCell;
	SearchCell* m_goalCell;
	std::vector<SearchCell*> m_openList;
	std::vector<SearchCell*> m_visitedList;
	std::vector<glm::vec3*> m_pathToGoal;
};