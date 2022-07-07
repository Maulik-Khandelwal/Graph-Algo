#pragma once
#include "Graph.h"
#include <memory>

class MSTAlgorithm
{
protected:
	Graph* m_Graph;
	std::vector<Graph::Edge> m_Edges;
public:
	MSTAlgorithm(Graph* graph);
	virtual std::unique_ptr<Graph> MST(std::vector<Graph::Edge>& mst, std::vector<bool>& edgeIncluded, bool maze = false) = 0;
};

