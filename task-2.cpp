#include <chrono>
#include <climits>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <stack>
#include <vector>

typedef std::size_t VertexID;
typedef int Cost;
typedef std::pair<VertexID, Cost> Edge;
typedef std::vector<std::vector<Edge>> Graph;

Graph
read_graph(char const *filename)
{
	Graph graph;
	std::size_t num_vertices;
	std::ifstream file;
	file.open(filename);
	if (!file.good()) {
		throw std::exception();
	}
	file >> num_vertices;
	for (std::size_t i = 0; i < num_vertices; ++i) {
		graph.push_back(std::vector<Edge>());
		for (std::size_t j = 0; j < num_vertices; ++j) {
			Cost cost;
			file >> cost;
			if (cost > 0) {
				graph[i].push_back(std::make_pair(j, cost));
			}
		}
	}
	/* File automatically closed when variable `file' goes out of scope. */
	return graph;
}

void
dfs(Graph const &graph, VertexID src, VertexID dst)
{
	std::size_t size = graph.size();
	std::vector<VertexID> parent(size, SIZE_MAX);
	std::vector<Cost> cost(size, INT_MAX);
	std::stack<VertexID> branches;
	std::size_t pops = 0, pushes = 0, hops, length;
	VertexID cur;

	auto start_time = std::chrono::steady_clock::now();
	cost[src] = 0;
	branches.push(src);
	++pushes;
	while (!branches.empty()) {
		cur = branches.top();
		branches.pop();
		++pops;
		if (cur != dst) {
			for (auto edge : graph[cur]) {
				if ((cost[edge.first] >
				     (cost[cur] + edge.second)) &&
				    ((cost[cur] + edge.second) < cost[dst])) {
					cost[edge.first] = cost[cur] + edge.second;
					parent[edge.first] = cur;
					branches.push(edge.first);
					++pushes;
				}
			}
		}
	}
	auto end_time = std::chrono::steady_clock::now();
	std::chrono::duration<double> duration = end_time - start_time;
	hops = 0; length = cost[dst];
	cur = dst;
	while (cur != src) {
		++hops;
		std::cout << cur << " ";
		cur = parent[cur];

	}
	std::cout << src << std::endl;
	std::cout << "Number of hops: " << hops << std::endl;
	std::cout << "Path length:    " << length << std::endl;
	std::cout << "Pushes:         " << pushes << std::endl;
	std::cout << "Pops:           " << pops << std::endl;
	std::cout << "Time taken:     " << duration.count() << std::endl;
}

void
bfs(Graph const &graph, VertexID src, VertexID dst)
{
	std::size_t size = graph.size();
	std::vector<VertexID> parent(size, SIZE_MAX);
	std::vector<Cost> cost(size, INT_MAX);
	std::queue<VertexID> branches;
	std::size_t pops = 0, pushes = 0, hops, length;
	VertexID cur;

	auto start_time = std::chrono::steady_clock::now();
	cost[src] = 0;
	branches.push(src);
	++pushes;
	while (!branches.empty()) {
		cur = branches.front();
		branches.pop();
		++pops;
		if (cur != dst) {
			for (auto edge : graph[cur]) {
				if ((cost[edge.first] >
				     (cost[cur] + edge.second)) &&
				    ((cost[cur] + edge.second) < cost[dst])) {
					cost[edge.first] = cost[cur] + edge.second;
					parent[edge.first] = cur;
					branches.push(edge.first);
					++pushes;
				}
			}
		}
	}
	auto end_time = std::chrono::steady_clock::now();
	std::chrono::duration<double> duration = end_time - start_time;
	hops = 0; length = cost[dst];
	cur = dst;
	while (cur != src) {
		++hops;
		std::cout << cur << " ";
		cur = parent[cur];

	}
	std::cout << src << std::endl;
	std::cout << "Number of hops: " << hops << std::endl;
	std::cout << "Path length:    " << length << std::endl;
	std::cout << "Pushes:         " << pushes << std::endl;
	std::cout << "Pops:           " << pops << std::endl;
	std::cout << "Time taken:     " << duration.count() << std::endl;
}

struct EdgeCompare {
	bool operator()(Edge const &a, Edge const &b) const {
		return a.second > b.second;
	}
};

void
ucs(Graph const &graph, VertexID src, VertexID dst)
{
	std::size_t size = graph.size();
	std::vector<VertexID> parent(size, SIZE_MAX);
	std::vector<Cost> cost(size, INT_MAX);
	std::priority_queue<Edge, std::vector<Edge>, EdgeCompare> branches;
	std::size_t pops = 0, pushes = 0, hops, length;
	Edge cur;

	auto start_time = std::chrono::steady_clock::now();
	cost[src] = 0;
	branches.push(std::make_pair(src, 0));
	++pushes;
	while (!branches.empty()) {
		cur = branches.top();
		branches.pop();
		++pops;
		if (cur.first != dst) {
			for (auto edge : graph[cur.first]) {
				if ((cost[edge.first] >
				     (cost[cur.first] + edge.second)) &&
				    ((cost[cur.first] + edge.second) < cost[dst])) {
					cost[edge.first] = cost[cur.first] + edge.second;
					parent[edge.first] = cur.first;
					branches.push(edge);
					++pushes;
				}
			}
		}
	}
	auto end_time = std::chrono::steady_clock::now();
	std::chrono::duration<double> duration = end_time - start_time;
	hops = 0; length = cost[dst];
	while (dst != src) {
		++hops;
		std::cout << dst << " ";
		dst = parent[dst];

	}
	std::cout << src << std::endl;
	std::cout << "Number of hops: " << hops << std::endl;
	std::cout << "Path length:    " << length << std::endl;
	std::cout << "Pushes:         " << pushes << std::endl;
	std::cout << "Pops:           " << pops << std::endl;
	std::cout << "Time taken:     " << duration.count() << std::endl;
}

int
main(int argc, char *argv[])
{
	std::string input, command;
	VertexID src, dst;
	if (argc != 2) {
		std::cerr << "Command line usage: task-2 filename" << std::endl;
		return -1;
	}
	try {
		auto graph = read_graph(argv[1]);
		for (;;) {
			std::cout << "> ";
			std::getline(std::cin, input);
			auto sstream = std::istringstream(input);
			sstream >> command;
			sstream >> src;
			sstream >> dst;
			if ((src >= graph.size()) ||
			    (dst >= graph.size())) {
				std::cerr << "vertices out of range" << std::endl;
				continue;
			}
			if (command == "dfs") {
				dfs(graph, src, dst);
			} else if (command == "bfs") {
				bfs(graph, src, dst);
			} else if (command == "ucs") {
				ucs(graph, src, dst);
			} else if (command == "exit") {
				break;
			} else {
				std::cerr << "command not understood" << std::endl;
			}
		}
	} catch (std::exception const &e) {
		std::cerr << "Could not open file" << std::endl;
		return -1;
	}
	return 0;
}
