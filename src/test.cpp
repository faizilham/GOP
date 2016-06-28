#include "gop.hpp"
#include <iostream>
#include <cstdio>
#include <fstream>
#include <sstream>

using namespace GOP;
using namespace std;

float scorefunc (const NodeSet& nodes, const std::vector<int>& path){
	float sum  = 0;
	for (int node : path){
		sum += nodes.getScore(node);
	}

	return sum;
}

float spfunc (const NodeSet& nodes, const std::vector<int>& path, int newNode){
	float sum  = 0;
	for (int node : path){
		sum += nodes.getScore(node);
	}

	sum += nodes.getScore(newNode);

	return sum;
}

int main(){
	string temp;

	ifstream file("testcase/1att48.gop");//4china27
	getline(file, temp);
	stringstream init(temp);
	//cout<<"test"<< temp << endl;
	int num_nodes, num_score_elmts, start, end;
	init >> num_nodes >> num_score_elmts >> start >> end;


	NodeSet nodes(num_nodes, num_score_elmts);
	EdgeSet edges(num_nodes);

	float val;
	for (int i = 0; i < num_nodes; ++i){
		for (int j = 0; j < num_nodes; ++j){
			file >> val;
			edges.addEdge(i, j, val);
		}
	}

	for (int i = 0; i < num_nodes; ++i){
		for (int j = 0; j < num_score_elmts; ++j){
			file >> val;
			nodes.setScore(i, j, val);
		}
	}

	file.close();
	
	Solution s = two_param_iterative_gop(3, 2000, 5000, nodes, edges, start, end, scorefunc, spfunc);
	cout << s.distance << " " << s.score << endl;
	for (auto i = s.path.begin(); i != s.path.end(); ++i){
		cout << *i << " ";
	}
	cout << endl;
	return 0;
}