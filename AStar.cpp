#include "AStar.h"
#include <set>
#include <unordered_map>
#include <algorithm>
#include <math.h>

#define INFF 9999999

struct HeapEntryAStar
{
	size_t vertex;
	size_t distance;
	HeapEntryAStar() : vertex(999999), distance(INFF) {}
	HeapEntryAStar(size_t v, size_t d) : vertex(v), distance(d) {}
};

std::vector<size_t> AStar::FindPath(size_t startVertex, size_t endVertex, std::vector<Graph::Edge>& edgesExplored) const
{
	int ROOT = (int)sqrt(m_Maze->m_VertexCount - 2);
	std::vector<size_t> path;
	std::unordered_map<size_t, size_t> distanes;
	for (size_t vertex = 1; vertex <= m_Maze->m_VertexCount; vertex++)
	{
		distanes[vertex] = INFF;
	}
	std::unordered_map<size_t, HeapEntryAStar> entryMap;
	std::set<size_t> visitedVertices;
	std::vector<HeapEntryAStar*> unvisitedNodes;
	size_t* parentMap = new size_t[m_Maze->m_VertexCount + 1];
	distanes[startVertex] = 0;
	entryMap[startVertex] = HeapEntryAStar(startVertex, 0);
	unvisitedNodes.push_back(&entryMap[startVertex]);
	while (!unvisitedNodes.empty())
	{
		HeapEntryAStar* minVertex = unvisitedNodes.back();
		unvisitedNodes.pop_back();
		size_t currentVertex = minVertex->vertex;
		if (distanes[currentVertex] == INFF || currentVertex == endVertex) break;
		std::vector<Graph::Edge> incidentEdges;
		m_Maze->GetIncidentEdges(currentVertex, incidentEdges);
		for (auto const& edge : incidentEdges)
		{
			size_t neighbour = edge.vertexB;
			// We assusme that the end vertex is at the bottom right corner of the graph
			size_t remainingDistance = abs(ROOT-1 - ((int)currentVertex-1)%ROOT) + abs(ROOT-1 - (int)(currentVertex-1)/ROOT);
			size_t newDistance = distanes[currentVertex] + edge.cost;
			if (visitedVertices.find(neighbour) == visitedVertices.end())
			{
				if (newDistance < distanes[neighbour])
				{
					edgesExplored.push_back(edge);
					if (entryMap.find(neighbour) == entryMap.end())
					{
						entryMap[neighbour] = HeapEntryAStar(neighbour, newDistance + remainingDistance);
						unvisitedNodes.push_back(&entryMap[neighbour]);
						std::sort(unvisitedNodes.begin(), unvisitedNodes.end(), [](HeapEntryAStar* a, HeapEntryAStar* b) { return a->distance > b->distance; });
					}
					else
					{
						entryMap[neighbour].distance = newDistance + remainingDistance;
						std::sort(unvisitedNodes.begin(), unvisitedNodes.end(), [](HeapEntryAStar* a, HeapEntryAStar* b) { return a->distance > b->distance; });
					}
					distanes[neighbour] = newDistance;
					parentMap[neighbour] = currentVertex;
				}
			}
		}
		visitedVertices.insert(currentVertex);
	}
	size_t current = endVertex;
	while (current != startVertex)
	{
		path.push_back(current);
		current = parentMap[current];
	}
	path.push_back(startVertex);
	delete[] parentMap;
	return path;
}
