#include "MSTAlgorithmFactory.h"

std::unique_ptr<MSTAlgorithm> MSTAlgorithmFactory::GetMSTAlgorithm(MSTAlgorithmFactory::Algorithm type, Graph * graph)
{
	switch (type)
	{
	case MSTAlgorithmFactory::Algorithm::PRIM_JARNIK:
		return std::make_unique<PrimJarnik>(graph);
	case MSTAlgorithmFactory::Algorithm::KRUSKAL:
		return std::make_unique<Kruskal>(graph);
	case MSTAlgorithmFactory::Algorithm::BORUVKA:
		return std::make_unique<Boruvka>(graph);
	default:
		return std::make_unique<Kruskal>(graph);
	}
}
