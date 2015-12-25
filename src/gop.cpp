#include "gop.hpp"
#include <cstring>
#include <ctime>
#include <algorithm> 
#include <set>
#include <cstdio>

GOP::NodeSet::NodeSet(int _num_nodes, int _num_score_elmts): num_nodes(_num_nodes), num_score_elmts(_num_score_elmts){
	scores = new float[_num_nodes*_num_score_elmts];
}

GOP::NodeSet::~NodeSet(){
	delete[] scores;
}

float GOP::NodeSet::getScore(int node_id, int score_elmt_id) const{
	return scores[node_id*num_score_elmts + score_elmt_id];
}
void GOP::NodeSet::setScore(int node_id, int score_elmt_id, float score){
	scores[node_id*num_score_elmts + score_elmt_id] = score;
}

GOP::EdgeSet::EdgeSet(int _num_nodes): num_nodes(_num_nodes){
	edges = new float[_num_nodes * _num_nodes];
}

GOP::EdgeSet::~EdgeSet(){
	delete[] edges;
}

void GOP::EdgeSet::addEdge(int node1, int node2, float length){
	edges[node1 * num_nodes + node2] = length;
}

float GOP::EdgeSet::getLength(int node1, int node2) const{
	return edges[node1 * num_nodes + node2];
}

int GOP::Solution::distance_budget;

GOP::Solution::Solution(): score(0), distance(0){}

GOP::Solution::~Solution(){}
GOP::Solution::Solution(const Solution& sol){
	score = sol.score;
	distance = sol.distance;
	for (int node : sol.path){
		path.push_back(node);
	}
}

void GOP::Solution::operator= (const Solution& sol){
	score = sol.score;
	distance = sol.distance;
	path.clear();
	for (int node : sol.path){
		path.push_back(node);
	}
}

float GOP::Solution::getScore(const NodeSet& nodes){
	// TODO set score function
	float sum = 0;
	for (int node : path){
		for (int i = 0; i < nodes.num_score_elmts; ++i){
			sum += nodes.getScore(node, i) / nodes.num_score_elmts;	
		}
	}
	score = sum;

	return sum;
}

void GOP::Solution::two_opt(const EdgeSet& edges){
	/* (2-opt algorithm) While there are edges (a,b), (c,d) in S s.t. d(a,b) + d(c,d) > d(a,c) + d(b,d), 
		remove edges (a,b) and (c,d) from S and add edges (a,c) and (b,d) to S. */

	int a, b, c, d; float dab, dcd, dac, dbd; bool changed; int n = path.size();
	do {
		changed = false;
		for (int i = 0; i < n - 2; ++i){
			for (int j = i + 1; j < n - 1; ++j){
				a = path[i]; b = path[i+1];
				c = path[j]; d = path[j+1];

				dab = edges.getLength(a, b); dcd = edges.getLength(c, d);
				dac = edges.getLength(a, c); dbd = edges.getLength(b, d);

				if (dab + dcd > dac + dbd){
					distance = distance - dab - dcd + dac + dbd;
					path[i+1] = c; path[j] = b;
					changed = true;
				}
			}
		}

	} while(changed);
}

void GOP::Solution::process_gop(int par_i, int par_t, const NodeSet& nodes, const EdgeSet& edges, int start, int end){
	/*
	Input: Parameters i and t, graph G = (V,E), distance matrix d for which dab is
	the distance between vertices a and b, start node s, destination node e, distance
	limit l, and score(S), a function that returns the score of a solution S.
	*/

	score = 0.0; distance = 0.0; path.clear(); std::set<int> L;

	// 2. Initialize solution S to contain the single node s
	path.push_back(start);
	float last_distance = 0.0;
	int available_nodes = nodes.num_nodes - 1;

	// 3. While adding node e to the end of S would not cause the length of S to exceed the distance limit l.
	while (distance + edges.getLength(path.back(), end) <= distance_budget){
		int node;
		/* (a) Randomly select i nodes (with repeats allowed), s.t. each is not in S and each is not e.
		 	Store these i nodes in set L. If all nodes except e have been added	to S, then add e to the end and return the final solution.*/
		if (available_nodes == 1){
			path.push_back(end);
			distance += edges.getLength(path.back(), end);
			return;
		} else {
			L.clear();
			for (int i = 0; i < par_i; ++i){
				do{
					node = rand() % nodes.num_nodes;
				} while((node == end) || (std::find(path.begin(), path.end(), node) != path.end()));

				L.insert(node);
			}
		}

		/* (b) If z is the last vertex in S, then select b in L s.t. for all q in L, d(z,b) + d(b,e) <= d(z,q) + d(q,e). */
		auto b = L.begin(); node = *b;
		last_distance = edges.getLength(path.back(), node) + edges.getLength(node, end); ++b;
		while (b != L.end()){
			float dis = edges.getLength(path.back(), *b) + edges.getLength(*b, end);
			if (dis < last_distance){
				last_distance = dis;
				node = *b;
			}
			++b;
		}

		/* (c) Add b to the end of S. */
		path.push_back(node);
		distance += last_distance;
		--available_nodes;
	}

	// 4. Replace the last vertex in S with e.
	path.pop_back(); 
	distance = distance - last_distance + edges.getLength(path.back(), end);
	path.push_back(end);

	/* 5. 2-opt algorithm */
	two_opt(edges);
}


GOP::Solution GOP::two_param_iterative_gop(int par_i, int par_t, int distance_budget, const NodeSet& nodes, const EdgeSet& edges, int start, int end){
	srand(time(NULL));
	Solution::distance_budget = distance_budget;
	Solution old, current;

	do{
		old = current;
		current.process_gop(par_i, par_t, nodes, edges, start, end);
		current.getScore(nodes);
	}while (old.score < current.score);

	return old;
}