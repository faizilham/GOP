#ifndef GOP_DLL_H
#define GOP_DLL_H

#include <vector>

#ifdef BUILD_GOP_DLL
#define GOP2PIADLL_API __declspec(dllexport)
#else
#define GOP2PIADLL_API __declspec(dllimport)
#endif

namespace GOP {
	class GOP2PIADLL_API NodeSet{
	private:
		float *scores; // scores[node_id][score_elmt_id]
	public:
		int num_nodes;
		int num_score_elmts;
		NodeSet(int _num_node, int _num_score_elmt);
		~NodeSet();
		float getScoreElement(int node_id, int score_elmt_id) const;
		float getScore(int node_id) const;
		void setScore(int node_id, int score_elmt_id, float score);
	};

	class GOP2PIADLL_API EdgeSet{
	private:
		int num_nodes;
		float* edges;
	public:
		EdgeSet(int _num_nodes);
		~EdgeSet();
		void addEdge(int node1, int node2, float length);
		float getLength(int node1, int node2) const;
	};

	class GOP2PIADLL_API Solution{
	public:
		float score, distance;
		int distance_budget;
		NodeSet *nodes;
		EdgeSet *edges;
		std::vector<int> path;

		Solution(int budget, NodeSet* _nodes, EdgeSet* _edges);
		~Solution();
		Solution(const Solution& sol);
		void operator= (const Solution& sol);
		void copy (const Solution& sol);
		void process_gop(int par_i, int par_t, int start, int end);

	private:
		float countScore();
		float countDistance();

		void two_opt();
		void setNodeInMiddle(int pos, int node);
		int buildT(std::vector<int>& unused_nodes, bool* used);
		void buildUnused(std::vector<int>& unused_nodes, bool* used);
		void pathTightening(std::vector<int>& unused_nodes, bool* used);
	};

	Solution GOP2PIADLL_API two_param_iterative_gop(int par_i, int par_t, int distance_budget, NodeSet& nodes, EdgeSet& edges, int start, int end);
}

#endif