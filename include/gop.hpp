#ifndef GOP_HPP
#define GOP_HPP

#include <cstdlib>
#include <vector>

namespace GOP {
	class NodeSet{
	private:
		float *scores = NULL; // scores[node_id][score_elmt_id]
	public:
		int num_nodes;
		int num_score_elmts;
		NodeSet(int _num_node, int _num_score_elmt);
		~NodeSet();
		float getScoreElement(int node_id, int score_elmt_id) const;
		float getScore(int node_id) const;
		void setScore(int node_id, int score_elmt_id, float score);
	};

	class EdgeSet{
	private:
		int num_nodes;
		float* edges = NULL;
	public:
		EdgeSet(int _num_nodes);
		~EdgeSet();
		void addEdge(int node1, int node2, float length);
		float getLength(int node1, int node2) const;
	};

	class Solution{
	public:
		static int distance_budget;
		static NodeSet *nodes;
		static EdgeSet *edges;

		float score, distance;
		std::vector<int> path;

		Solution();
		~Solution();
		Solution(const Solution& sol);
		void operator= (const Solution& sol);
		void copy (const Solution& sol);
		void process_gop(int par_i, int par_t, int start, int end);
		float getScore();
	private:
		void two_opt();
		int buildT(std::vector<int>& unused_nodes, bool* used);
	};

	Solution two_param_iterative_gop(int par_i, int par_t, int distance_budget, NodeSet& nodes, EdgeSet& edges, int start, int end);
	

}

#endif