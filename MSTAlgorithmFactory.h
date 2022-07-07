#include "MSTAlgorithm.h"
#include "Kruskal.h"
#include "PrimJarnik.h"
#include "Boruvka.h"
#include <memory>

#pragma once
class MSTAlgorithmFactory
{
public:
	enum class Algorithm { PRIM_JARNIK, KRUSKAL, BORUVKA };
	static std::unique_ptr<MSTAlgorithm> GetMSTAlgorithm(MSTAlgorithmFactory::Algorithm type, Graph* graph);
};

