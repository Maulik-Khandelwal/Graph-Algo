#pragma once
#include "MSTAlgorithm.h"

class Boruvka : public MSTAlgorithm
{
public:
	Boruvka(Graph* graph) : MSTAlgorithm(graph) {}
	std::unique_ptr<Graph> MST(std::vector<Graph::Edge>& mst, std::vector<bool>& edgeIncluded, bool maze = false) override;
};