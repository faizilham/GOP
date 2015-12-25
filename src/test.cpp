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

	for (int i = 0; i < num_nodes; ++i){
		for (int j = 0; j < num_nodes; ++j){
			printf("%.0f ", edges.getLength(i, j));
		}
		printf("\n");
	}


	for (int i = 0; i < num_nodes; ++i){
		for (int j = 0; j < num_score_elmts; ++j){
			printf("%.0f ", nodes.getScore(i, j));
		}
		printf("\n");
	}
	
	

	return 0;
}