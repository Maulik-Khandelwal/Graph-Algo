#include "PathfindingAlgorithmFactory.h"

std::unique_ptr<PathfindingAlgorithm> PathfindingAlgorithmFactory::GetPathfindingAlgorithm(Pathfinding type, Graph* maze)
{
	switch (type)
	{
	case Pathfinding::ASTAR:
		return std::make_unique<AStar>(maze);
	case Pathfinding::BFS:
		return std::make_unique<BFS>(maze);
	case Pathfinding::DFS:
		return std::make_unique<DFS>(maze);
	case Pathfinding::DIJKSTRA:
		return std::make_unique<Dijkstra>(maze);
	default:
		return std::make_unique<Dijkstra>(maze);
	}
}
