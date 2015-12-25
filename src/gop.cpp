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

float GOP::NodeSet::getScoreElement(int node_id, int score_elmt_id) const{
	return scores[node_id*num_score_elmts + score_elmt_id];
}

float GOP::NodeSet::getScore(int node_id) const{
	float sum = 0;
	for (int i = 0; i < num_score_elmts; ++i){
		sum += getScoreElement(node_id, i) / num_score_elmts;	
	}

	return sum;
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

GOP::Solution::Solution(int budget, NodeSet* _nodes, EdgeSet* _edges)
: score(0), distance(0), distance_budget(budget), nodes(_nodes), edges(_edges){}

GOP::Solution::~Solution(){}
GOP::Solution::Solution(const Solution& sol){
	copy(sol);
}

void GOP::Solution::operator= (const Solution& sol){
	copy(sol);
}

void GOP::Solution::copy (const Solution& sol){
	score = sol.score;
	distance = sol.distance;
	nodes = sol.nodes;
	edges = sol.edges;
	path.clear();
	for (int node : sol.path){
		path.push_back(node);
	}
}


float GOP::Solution::countScore(){
	// for debugging only
	float sum = 0;
	for (int node : path){
		sum += nodes->getScore(node);
	}
	//score = sum;

	return sum;
}

float GOP::Solution::countDistance(){
	// for debugging only
	float sum = 0;
	int n = path.size();
	for (int i = 0; i < n - 1; ++i){
		sum += edges->getLength(path[i], path[i+1]);
	}

	return sum;
}



void GOP::Solution::two_opt(){
	/* (2-opt algorithm) While there exist edges (a,b), (c,d) in S s.t. d(a,b) + d(c,d) > d(a,c) + d(b,d), 
		remove edges (a,b) and (c,d) from S and add edges (a,c) and (b,d) to S. */

	int a, b, c, d; float dab, dcd, dac, dbd; bool changed; int n = path.size();
	do {
		changed = false;
		for (int i = 0; i < n - 2; ++i){
			for (int j = i + 1; j < n - 1; ++j){
				a = path[i]; b = path[i+1];
				c = path[j]; d = path[j+1];

				dab = edges->getLength(a, b); dcd = edges->getLength(c, d);
				dac = edges->getLength(a, c); dbd = edges->getLength(b, d);

				if (dab + dcd > dac + dbd){
					distance = distance - dab - dcd + dac + dbd;

					int n = j - i; // size from b and c
					for (int k = 0; k < n / 2; ++k){ // flip
						int n1 = i + 1 + k;
						int n2 = j - k;

						std::iter_swap(path.begin() + n1, path.begin() + n2);
					}

					changed = true;
					break;
				}
			}

			if (changed) break;
		}

	} while(changed);
}

void GOP::Solution::process_gop(int par_i, int par_t, int start, int end){
	/*
	Input: Parameters i and t, graph G = (V,E), distance matrix d for which dab is
	the distance between vertices a and b, start node s, destination node e, distance
	limit l, and score(S), a function that returns the score of a solution S.
	*/

	/** INITIALIZATION PHASE **/
	score = 0.0; distance = 0.0; path.clear(); std::set<int> R, L; bool used[nodes->num_nodes];
	memset(used, 0, sizeof(bool) * nodes->num_nodes);

	// 2. Initialize solution S to contain the single node s
	path.push_back(start); used[start] = true;
	float last_distance = 0.0; float last_score = nodes->getScore(start);
	score = last_score;
	int available_nodes = nodes->num_nodes - 1;

	// 3. While adding node e to the end of S would not cause the length of S to exceed the distance limit l.
	while (distance + edges->getLength(path.back(), end) <= distance_budget){
		int node;
		/* (a) Randomly select i nodes (with repeats allowed), s.t. each is not in S and each is not e.
		 	Store these i nodes in set L. If all nodes except e have been added	to S, then add e to the end and return the final solution.*/
		if (available_nodes == 1){
			path.push_back(end);
			distance += edges->getLength(path.back(), end);
			return;
		} else {
			L.clear();
			for (int i = 0; i < par_i; ++i){
				do{
					node = rand() % nodes->num_nodes;
				} while((node == end) || (std::find(path.begin(), path.end(), node) != path.end()));

				L.insert(node);
			}
		}

		/* (b) If z is the last vertex in S, then select b in L s.t. for all q in L, d(z,b) + d(b,e) <= d(z,q) + d(q,e). */
		auto b = L.begin(); node = *b;
		float min_dis = edges->getLength(path.back(), node) + edges->getLength(node, end); ++b;
		while (b != L.end()){
			float dis = edges->getLength(path.back(), *b) + edges->getLength(*b, end);
			if (dis < min_dis){
				min_dis = dis;
				node = *b;
			}
			++b;
		}

		/* (c) Add b to the end of S. */
		last_score = nodes->getScore(node); score += last_score;
		last_distance = edges->getLength(path.back(), node);
		path.push_back(node); used[node] = true;
		distance += last_distance;
		--available_nodes;
	}

	// 4. Replace the last vertex in S with e.
	path.pop_back(); used[end] = true;
	score = score - last_score + nodes->getScore(end);
	distance = distance - last_distance + edges->getLength(path.back(), end);
	path.push_back(end); used[end] = true;

	/* 5. 2-opt algorithm */
	two_opt();

	/** PATH TIGHTENING PHASE **/

	// 6. Build unused list
	std::vector<int> unused_nodes;
	buildUnused(unused_nodes, used);

	// 7-8 Path tightening
	pathTightening(unused_nodes, used);


	/** PERTURBATION AND IMPROVEMENT **/

	/* 9. Flag current solution S as the best solution discovered and set y, the number of
	iterations since the last improvement in the best solution, to be 0. */
	Solution best(*this);
	int y = 0;

	// 10. While y <= t
	while(y <= par_t){
		/* (a) Randomly select i unique nodes in S, each of which is not s or e, and store them in set R.
		*/
		R.clear();
		for (int i = 0; i < par_i; ++i){
			int node;
			do{
				node = path[rand() % path.size()];
			} while (node == start || node == end || (R.find(node) != R.end()));
			R.insert(node);
		}

		/*(b) For each a in R, let b(S,a) be the node in S before a and let a(S,a) be the
			node in S after a. Remove edges (b(S,a),a) and (a,a(S,a)) and add edge (b(S,a),a(S,a))
			*/

		int bef, aft;
		for (int node : R){
			auto pos = std::find(path.begin(), path.end(), node); // TODO optimize with memoization from above
			bef = *(pos - 1); aft = *(pos + 1);
			path.erase(pos);
			
			used[node] = false;
			score -= nodes->getScore(node);

			distance += edges->getLength(bef, aft) - edges->getLength(bef, node) - edges->getLength(node, aft);

			unused_nodes.push_back(node);
		}
		
		/*	(c) Place the vertices not in S and not in R in a list L, such that Lm is the mth
			element of the list. Define function sp(S,k) as in Step 6. Insert the elements
			into L such that sp(S,Lm) < sp(S,Lo) implies m > o.

			note: this should be unused_nodes, so let's move to

			(d) Add the contents of R in arbitrary order to the end of L.

			note: joined in previous loop
		*/

		// (e) Repeat Steps 7 through 8 with L to complete modified path tightening.
		pathTightening(unused_nodes, used);

		/* (f) 2-opt algorithm */
		two_opt();

		// (g) Repeat Steps 6 through 8 to complete unmodified path tightening.

		buildUnused(unused_nodes, used);
		pathTightening(unused_nodes, used);

		/* (h) If score(S) is higher than the score of the best solution yet discovered, flag
		current solution S as the best solution discovered and set y = 0. Otherwise, set y = y + 1. */

		if (score > best.score){
			best.copy(*this);
			y = 0;
		} else {
			++y;	
		}
	}

}

void GOP::Solution::pathTightening(std::vector<int>& unused_nodes, bool* used){
	/* 7. Build T */
	// since T is redefined every iteration, no need to save all and only take the 1st
	int T = buildT(unused_nodes, used);
	
	// 8. While |T| > 0
	while(T != -1){
		// (a) Select Lb in T s.t. b ≤ j for all Lj in T.
		int lb = T;

		// (b) Select edge (v,w) in S s.t. dvLb +dLbw − dvw ≤ dxLb +dLby − dxy for all (x,y) in S
		int n = path.size(); int a, b; float dal, dlb, dab, dmin = 0; int imin;

		for (int i = 0; i < n - 1; ++i){
			a = path[i]; b = path[i+1];
			dab = edges->getLength(a,b); dal = edges->getLength(a,lb); dlb = edges->getLength(lb,b);
			float dnew = dal + dlb - dab;

			if (dmin > dnew || i == 0){
				dmin = dnew; imin = i + 1;
			}
		}

		// (c) Remove edge (v,w) from S and add edges (v,Lb) and (Lb,w) to S.
		path.insert(path.begin() + imin, lb);
		distance += dmin;
		score += nodes->getScore(lb); 

		// (d) Redefine T as in Step 7.
		T = buildT(unused_nodes, used);
	}
}

void GOP::Solution::buildUnused(std::vector<int>& unused_nodes, bool* used){
	/* 6. Place the vertices not in S in a list L, such that Lm is the mth element of the list.
		Define function sp(S,k) = score(T), where T is S with vertex k inserted at arbitrary location. 
		Insert the elements into L such that sp(S,Lm) < sp(S,Lo), implies m > o. */
	int max = 0, max_id = -1;

	unused_nodes.clear();

	for (int i = 0; i < nodes->num_nodes; ++i) {
		if (!used[i]) {
			unused_nodes.push_back(i);
			int sp = nodes->getScore(i);
			if (sp > max){
				max = sp; max_id = unused_nodes.size() - 1;
			}
		}
	}

	if (max_id > -1)
		std::iter_swap(unused_nodes.begin(), unused_nodes.begin() + max_id); // TODO try with sort, maybe not so efficient
}

int GOP::Solution::buildT(std::vector<int>& unused_nodes, bool* used){
	/* Define set T = {Lm in L,Lm not in S : there exist (a,b) in S : the length of S is less than budget if
		edge (a,b) is removed from S and edges (a,Lm) and (Lm,b) are added to S}.
	*/

	int n = path.size(); int a, b, lm; float dab, dal, dlb;
	for (auto it = unused_nodes.begin(); it != unused_nodes.end(); ++it){
		lm = *it;
		for (int i = 0; i < n - 1; ++i){
			a = path[i]; b = path[i+1];
			dab = edges->getLength(a,b); dal = edges->getLength(a,lm); dlb = edges->getLength(lm,b);

			if (distance - dab + dal + dlb < distance_budget){
				unused_nodes.erase(it);
				used[lm] = true;
				return lm;
			}
		}
	}

	return -1;
}

GOP::Solution GOP::two_param_iterative_gop(int par_i, int par_t, int distance_budget, NodeSet& nodes, EdgeSet& edges, int start, int end){
	srand(time(NULL));
	Solution old(distance_budget, &nodes, &edges), current(distance_budget, &nodes, &edges);

	do{
		old = current;
		current.process_gop(par_i, par_t, start, end);
	}while (old.score < current.score);

	return old;
}