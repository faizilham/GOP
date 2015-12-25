#ifndef GOP_HPP
#define GOP_HPP

#include <vector>

namespace GOP {
	class NodeSet{
	private:
		float *scores; // scores[node_id][score_elmt_id]
	public:
		int num_nodes;
		int num_score_elmts;
		NodeSet(int _num_node, int _num_score_elmt);
		~NodeSet();
		float getScore(int node_id, int score_elmt_id) const;
		void setScore(int node_id, int score_elmt_id, float score);
	};

	class EdgeSet{
	private:
		int num_nodes;
		float* edges;
	public:
		EdgeSet(int _num_nodes);
		~EdgeSet();
		void addEdge(int node1, int node2, float length);
		float getLength(int node1, int node2) const;
	};

	class Solution{
	public:
		static int distance_budget;

		float score, distance;
		std::vector<int> path;

		Solution();
		~Solution();
		Solution(const Solution& sol);
		void operator= (const Solution& sol);
		void process_gop(int par_i, int par_t, const NodeSet& nodes, const EdgeSet& edges, int start, int end);
		float getScore(const NodeSet& nodes);
	private:

	};

	Solution two_param_iterative_gop(int par_i, int par_t, int distance_budget, const NodeSet& nodes, const EdgeSet& edges, int start, int end);
	

}

#endif