#pragma once
#include "PathfindingAlgorithm.h"
class Dijkstra : public PathfindingAlgorithm
{
public:
	Dijkstra(Graph* graph) : PathfindingAlgorithm(graph) {}
	std::vector<size_t> FindPath(size_t startVertex, size_t endVertex, std::vector<Graph::Edge>& edgesExplored) const override;
};

