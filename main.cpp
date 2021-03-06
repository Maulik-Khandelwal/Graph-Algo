
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Graph.h"
#include "MSTAlgorithm.h"
#include "MSTAlgorithmFactory.h"
#include "PathfindingAlgorithm.h"
#include "PathfindingAlgorithmFactory.h"
#include <iostream>
#include <math.h>
#include <memory>
#include <random>
#include <set>

#define PI 3.14159265f
#define ORIGIN_X ScreenWidth()/2
#define ORIGIN_Y ScreenHeight()/2
#define RADIUS 40
#define CIRCLE_RADIUS 4* ORIGIN_Y / 5
#define STRING_OFFSET 5

#define USER_ANIMATION_CONTROL 0
#define DRAW_MAZE 1

#if DRAW_MAZE
#define TIME_BETWEEN_FRAMES 0.2f
#define MAZE_GRAPH_VERTICES 30
#define VERTEX_SCALE 12
#define START_VERTEX MAZE_GRAPH_VERTICES*MAZE_GRAPH_VERTICES+1
#define END_VERTEX START_VERTEX+1
#else
#define TIME_BETWEEN_FRAMES 1.0f
#endif

typedef MSTAlgorithmFactory::Algorithm Algorithm;
typedef PathfindingAlgorithmFactory::Pathfinding Pathfinding;

class GraphAlgorithms : public olc::PixelGameEngine
{
private:
	Graph* m_Graph;
	float m_Angle;
	std::vector<Graph::Edge> m_Mst;
	std::vector<bool> m_EdgeIncluded;
	Algorithm m_Type = Algorithm::BORUVKA;
	std::unique_ptr<MSTAlgorithm> m_MstAlgorithm;
	size_t m_CurrentIndex = -1;
	std::set<size_t> m_VertexSet;
	float m_Time = TIME_BETWEEN_FRAMES;
	std::unique_ptr<Graph> m_Maze;
#if DRAW_MAZE
	std::vector<size_t> m_MazePath;
	std::vector<Graph::Edge> m_EdgesExplored;
	std::unique_ptr<PathfindingAlgorithm> m_PathfindingAlgorithm;
	Pathfinding m_PathType = Pathfinding::BFS;
	size_t m_PathCurrentIndex = -1;
#endif
private:
	std::pair<size_t, size_t> GetPosition(size_t vertex)
	{
#if DRAW_MAZE
		size_t ROOT = (size_t)sqrt(m_Graph->m_VertexCount-2);
		size_t dW = ScreenWidth() / (ROOT - 1);
		size_t dH = ScreenHeight() / (ROOT - 1);
		size_t startX = 1;
		size_t startY = 1;
		if (vertex == START_VERTEX-1)
			return{ startX, 0 };
		else if (vertex == END_VERTEX-1)
			return{ startX + (MAZE_GRAPH_VERTICES- 1)*dW , ScreenHeight()-1 };
		else
			return{ startX + (vertex%ROOT)*dW, startY + (vertex /ROOT)*dH };
#else
		float angle = vertex * m_Angle;
		float newX = ORIGIN_X + CIRCLE_RADIUS * cosf(angle);
		float newY = ORIGIN_Y + CIRCLE_RADIUS * sinf(angle);
		return{ (size_t)newX, (size_t)newY };
#endif
	}
	void DrawEdge(const Graph::Edge& edge, olc::Pixel lineColor, olc::Pixel textColor=olc::WHITE)
	{
		auto posA = GetPosition(edge.vertexA - 1);
		auto posB = GetPosition(edge.vertexB - 1);
#if DRAW_MAZE
		DrawLine(posA.first, posA.second, posB.first, posB.second, lineColor);
#else
		DrawLine(posA.first, posA.second-1, posB.first, posB.second-1, lineColor);
		DrawLine(posA.first, posA.second, posB.first, posB.second, lineColor);
		DrawLine(posA.first, posA.second+1, posB.first, posB.second+1, lineColor);
		DrawString((posA.first + posB.first) / 2, (posA.second + posB.second) / 2, std::to_string(edge.cost), textColor, 2);
#endif
	}
public:
	GraphAlgorithms(Graph *graph) : m_Graph(graph)
	{
		m_Angle = (360.0f / graph->m_VertexCount) * 0.0174532925f;
		sAppName = "Graph Algorithms Visualizer";
		m_MstAlgorithm = MSTAlgorithmFactory::GetMSTAlgorithm(m_Type, m_Graph);
	}
#if DRAW_MAZE
	void UpdateGraphics()
	{
		bool animationFinished = m_CurrentIndex == m_Mst.size() - 1;
		Clear(olc::BLACK);
		// Draw MST
		for (size_t i = 0; i <= m_CurrentIndex; i++)
		{
			if (m_CurrentIndex == -1) break;
			DrawEdge(m_Mst[i], m_EdgeIncluded[i] ? olc::WHITE : olc::BLACK);
		}
		if (m_CurrentIndex != -1)
		{
			DrawEdge(m_Mst[m_CurrentIndex], m_EdgeIncluded[m_CurrentIndex] ? animationFinished ? olc::WHITE : olc::BLUE : olc::BLACK);
			m_VertexSet.emplace(m_Mst[m_CurrentIndex].vertexA-1);
			m_VertexSet.emplace(m_Mst[m_CurrentIndex].vertexB-1);
		}
		// Draw vertices
		for (size_t vertex : m_VertexSet)
		{
			auto position = GetPosition(vertex);
			Draw(position.first, position.second, vertex >= START_VERTEX-1 ? olc::GREEN : olc::WHITE);
		}
		if (animationFinished)
		{
			// Draw explored edges
			for (size_t i = 0; i <= m_PathCurrentIndex; i++)
			{
				if (m_PathCurrentIndex == -1) break;
				DrawEdge(m_EdgesExplored[i], olc::BLUE);
			}
			if (m_PathCurrentIndex == m_EdgesExplored.size()-1)
			{
				// Draw path
				for (size_t i = m_MazePath.size() - 1; i > 0; i--)
				{
					size_t first = m_MazePath[i];
					size_t second = m_MazePath[i - 1];
					DrawEdge(Graph::Edge(first, second, 1), olc::GREEN);
				}
			}
		}
	}
#else
	void UpdateGraphics()
	{
		bool animationFinished = m_CurrentIndex == m_Mst.size() - 1;
		Clear(olc::BLACK);
#if USER_ANIMATION_CONTROL
		if (!animationFinished)
			DrawString(STRING_OFFSET, STRING_OFFSET, "Press <<Enter>> for next step", olc::WHITE, 2);
#endif
		// Draw edges
		if (!animationFinished)
		{
			for (const auto& edge : m_Graph->m_Edges)
			{
				DrawEdge(edge, olc::GREY, olc::WHITE);
			}
		}
		// Draw MST
		for (size_t i = 0; i <= m_CurrentIndex; i++)
		{
			if (m_CurrentIndex == -1) break;
			DrawEdge(m_Mst[i], m_EdgeIncluded[i] ? olc::GREEN : animationFinished ? olc::BLACK : olc::RED, m_EdgeIncluded[i] ? olc::WHITE : animationFinished ? olc::BLACK : olc::WHITE);
			std::string edgeAdded = std::to_string(m_Mst[m_CurrentIndex].vertexA) + " -> " + std::to_string(m_Mst[m_CurrentIndex].vertexB);
			if (!animationFinished)
				DrawString(ScreenWidth() - 120, STRING_OFFSET, edgeAdded, m_EdgeIncluded[m_CurrentIndex] ? olc::GREEN : olc::RED, 2);
		}
		if (m_CurrentIndex != -1)
		{
			m_VertexSet.emplace(m_Mst[m_CurrentIndex].vertexA - 1);
			m_VertexSet.emplace(m_Mst[m_CurrentIndex].vertexB - 1);
		}
		// Draw vertices
		for (size_t vertex = 0; vertex < m_Graph->m_VertexCount; vertex++)
		{
			auto position = GetPosition(vertex);
			bool mstVertex = m_VertexSet.find(vertex) != m_VertexSet.end();
			FillCircle(position.first, position.second, RADIUS, mstVertex ? olc::DARK_GREEN : olc::BLUE);
			DrawString(position.first - STRING_OFFSET, position.second - STRING_OFFSET, std::to_string(vertex + 1), olc::WHITE, 2);
		}
	}
#endif
public:
	bool OnUserCreate() override
	{
		m_Maze = m_MstAlgorithm->MST(m_Mst, m_EdgeIncluded, (bool)DRAW_MAZE);
#if DRAW_MAZE
		m_PathfindingAlgorithm = PathfindingAlgorithmFactory::GetPathfindingAlgorithm(m_PathType, m_Maze.get());
		m_MazePath = m_PathfindingAlgorithm->FindPath(START_VERTEX, END_VERTEX, m_EdgesExplored);
#endif
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		m_Time -= fElapsedTime;
#if USER_ANIMATION_CONTROL
#if DRAW_MAZE
		if ((GetKey(olc::Key::ENTER).bPressed || GetKey(olc::Key::ENTER).bHeld) && (m_CurrentIndex < m_Mst.size() - 1 || m_CurrentIndex == -1)) m_CurrentIndex++;
		if ((GetKey(olc::Key::ENTER).bPressed || GetKey(olc::Key::ENTER).bHeld) && m_CurrentIndex == m_Mst.size() - 1 && (m_PathCurrentIndex < m_EdgesExplored.size()-1 || m_PathCurrentIndex == -1)) m_PathCurrentIndex++;
#else
		if (GetKey(olc::Key::ENTER).bPressed && (m_CurrentIndex < m_Mst.size() - 1 || m_CurrentIndex == -1)) m_CurrentIndex++;
#endif
#else
		if (m_Time <= 0 && (m_CurrentIndex < m_Mst.size() - 1 || m_CurrentIndex == -1))
		{
			m_Time = TIME_BETWEEN_FRAMES;
			m_CurrentIndex++;
		}
#if DRAW_MAZE
		if (m_Time <= 0 && m_CurrentIndex == m_Mst.size() - 1 && (m_PathCurrentIndex < m_EdgesExplored.size() - 1 || m_PathCurrentIndex == -1))
		{
			m_Time = TIME_BETWEEN_FRAMES/10;
			m_PathCurrentIndex++;
		}
#endif
#endif
		UpdateGraphics();
		return true;
	}
};

#if DRAW_MAZE
void BuildGridGraph(Graph* graph)
{
	size_t ROOT = (size_t)sqrt(graph->m_VertexCount-2);
	size_t normalizedCount = graph->m_VertexCount - 2;
	for (size_t vertex = 1; vertex <= graph->m_VertexCount-2; vertex++)
	{
		std::random_device randomDevice;
		std::mt19937 engine(randomDevice());
		std::uniform_int_distribution<> edgeDistribution(2, 9999);
		size_t edgeValue = edgeDistribution(engine);
		// Top-left corner
		if (vertex == 1)
		{
			graph->AddEdge(vertex, vertex + 1, edgeValue);
			graph->AddEdge(vertex, vertex + ROOT, edgeValue);
		}
		// Top-right corner
		else if (vertex == ROOT)
		{
			graph->AddEdge(vertex, vertex - 1, edgeValue);
			graph->AddEdge(vertex, vertex + ROOT, edgeValue);
		}
		// Bottom-left corner
		else if (vertex == normalizedCount - ROOT + 1)
		{
			graph->AddEdge(vertex, vertex + 1, edgeValue);
			graph->AddEdge(vertex, vertex - ROOT, edgeValue);
		}
		// Bottom-right corner
		else if (vertex == normalizedCount)
		{
			graph->AddEdge(vertex, vertex - 1, edgeValue);
			graph->AddEdge(vertex, vertex - ROOT, edgeValue);
		}
		// Top edge
		else if (vertex < ROOT)
		{
			graph->AddEdge(vertex, vertex - 1, edgeValue);
			graph->AddEdge(vertex, vertex + 1, edgeValue);
			graph->AddEdge(vertex, vertex + ROOT, edgeValue);
		}
		// Bottom edge
		else if (vertex > normalizedCount - ROOT + 1 && vertex < normalizedCount)
		{
			graph->AddEdge(vertex, vertex - 1, edgeValue);
			graph->AddEdge(vertex, vertex + 1, edgeValue);
			graph->AddEdge(vertex, vertex - ROOT, edgeValue);
		}
		// Left edge
		else if (vertex % ROOT == 1 && (vertex > ROOT && vertex < normalizedCount - ROOT + 1))
		{
			graph->AddEdge(vertex, vertex + ROOT, edgeValue);
			graph->AddEdge(vertex, vertex - ROOT, edgeValue);
			graph->AddEdge(vertex, vertex + 1, edgeValue);
		}
		// Right edge
		else if (vertex % ROOT == 0 && (vertex > ROOT && vertex < normalizedCount - ROOT + 1))
		{
			graph->AddEdge(vertex, vertex + ROOT, edgeValue);
			graph->AddEdge(vertex, vertex - ROOT, edgeValue);
			graph->AddEdge(vertex, vertex - 1, edgeValue);
		}
		// Mid-piece
		else
		{
			graph->AddEdge(vertex, vertex + ROOT, edgeValue);
			graph->AddEdge(vertex, vertex - ROOT, edgeValue);
			graph->AddEdge(vertex, vertex - 1, edgeValue);
			graph->AddEdge(vertex, vertex + 1, edgeValue);
		}
		graph->AddEdge(START_VERTEX, 1, 1); // Start vertex
		graph->AddEdge(END_VERTEX, MAZE_GRAPH_VERTICES*MAZE_GRAPH_VERTICES, 1); // End vertex
	}

}
#endif

int main()
{
#if DRAW_MAZE
	Graph graph(MAZE_GRAPH_VERTICES*MAZE_GRAPH_VERTICES + 2);
	BuildGridGraph(&graph);
	GraphAlgorithms demo(&graph);
	if (demo.Construct(2 * MAZE_GRAPH_VERTICES + 1, 2 * MAZE_GRAPH_VERTICES + 1, VERTEX_SCALE, VERTEX_SCALE))
		demo.Start();
#else
	Graph graph(9); // Build example graph with 9 vertices
	graph.AddEdge(1, 2, 4);
	graph.AddEdge(1, 8, 8);
	graph.AddEdge(2, 3, 8);
	graph.AddEdge(2, 8, 11);
	graph.AddEdge(3, 4, 7);
	graph.AddEdge(3, 9, 2);
	graph.AddEdge(3, 6, 4);
	graph.AddEdge(4, 5, 9);
	graph.AddEdge(4, 6, 14);
	graph.AddEdge(5, 6, 10);
	graph.AddEdge(6, 7, 2);
	graph.AddEdge(7, 9, 6);
	graph.AddEdge(7, 8, 1);
	graph.AddEdge(8, 9, 7);
	GraphAlgorithms demo(&graph);
	if (demo.Construct(1366, 768, 1, 1))
		demo.Start();
#endif
	return 0;
}