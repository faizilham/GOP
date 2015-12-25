#include "gop.hpp"
#include <iostream>
#include <cstdio>
#include <fstream>
#include <sstream>

using namespace GOP;
using namespace std;

int main(){
	string temp;

	ifstream file("testcase/4china27.gop");
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
	
	Solution s = two_param_iterative_gop(3, 100, 5000, nodes, edges, start, end);
	cout << s.distance << " " << s.score << endl;
	for (auto i = s.path.begin(); i != s.path.end(); ++i){
		cout << *i << " ";
	}
	cout << endl;
	return 0;
}