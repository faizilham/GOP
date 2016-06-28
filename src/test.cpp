#include "gop.hpp"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <sstream>

using namespace GOP;
using namespace std;

static float exponent = 1;
static int numscores = 0;
static float* weight = NULL;

float scorefunc (const NodeSet& nodes, const std::vector<int>& path){
	float sum  = 0, partial;

	for (int i = 0; i < numscores; ++i){
		partial = 0;
		for (int node : path){
			partial += pow(nodes.getScoreElement(node, i), exponent);
		}

		sum += weight[i] * pow(partial, 1/exponent);
	}

	return sum;
}

float spfunc (const NodeSet& nodes, const std::vector<int>& path, int newNode){

	float sum  = 0, partial;

	for (int i = 0; i < numscores; ++i){
		partial = 0;
		for (int node : path){
			partial += pow(nodes.getScoreElement(node, i), exponent);
		}

		partial += pow(nodes.getScoreElement(newNode, i), exponent);

		sum += weight[i] * pow(partial, 1/exponent);
	}

	return sum;
}

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
	numscores = num_score_elmts;
	weight = new float[num_score_elmts];

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

	int exps[] = {1, 3, 4, 5, 10};
	float adist, ascore; int repeat = 3;

	for (int e : exps){
		exponent = e;
		for (int w = 0; w < 5; ++w){

			ascore = 0; adist = 0;

			int wt = w - 1;

			for (int i = 0; i < num_score_elmts; ++i){
				if (wt < 0){
					weight[i] = 0.25f;
				} else {
					weight[i] = 0.;
				}
			}

			if (wt >= 0) weight[wt] = 1;

			cout << "k = " << e << ", Wt = " << w << ": " << flush;
			for (int i = 0; i < repeat; ++i){
				Solution s = two_param_iterative_gop(4, 4500, 5000, nodes, edges, start, end, scorefunc, spfunc);	
				ascore += s.score;
				adist += s.distance;
			}
			
			cout << ascore / repeat<< endl;
			
		}	
	}

	delete[] weight;

	return 0;
}