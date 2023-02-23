#include<iostream>
#include<fstream>
#include <sstream>
#include <vector>
#include"ilcplex/ilocplex.h";
#include<chrono>;

using namespace std;

// Split string by a delimiter
vector<string> split(string s, string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;
	vector<string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

int main(int argc, char* argv[])
{
	auto start = chrono::high_resolution_clock::now();

#pragma region Problem data
	ifstream file;

	// Read a file from a specified path and name
	string file_path = "../data/";
	string file_name = "instance_0100.dat";

	file.open(file_path + file_name);

	if (file.fail()) {
		cout << "File Failed to open" << endl;
		return 1;
	}

	// Read the file line by line and assign each line to a specified variable
	string line;
	string delimiter = " ";

	getline(file, line);
	int n = stoi(line);

	int* d = new int[n];
	int* f = new int[n];
	int* p = new int[n];
	int* h = new int[n];

	getline(file, line);
	vector<string> line_2_splited = split(line, delimiter);

	getline(file, line);
	vector<string> line_3_splited = split(line, delimiter);

	getline(file, line);
	vector<string> line_4_splited = split(line, delimiter);

	getline(file, line);
	vector<string> line_5_splited = split(line, delimiter);


	for (int t = 0; t < n; t++)
	{
		d[t] = stoi(line_2_splited[t]);
		f[t] = stoi(line_3_splited[t]);
		p[t] = stoi(line_4_splited[t]);
		h[t] = stoi(line_5_splited[t]);
	}


	file.close();

	// Defining the big M
	int* M = new int[n];

	for (int t = 0; t < n; t++)
	{
		M[t] = 0;
		for (int k = t; k < n; k++) {
			M[t] += d[k];
		}
	}
#pragma endregion

#pragma region Environnement
	IloEnv env;
	IloModel Model(env);

#pragma endregion

#pragma region Decision Variables

	IloNumVarArray x(env, n, 0, IloInfinity, ILOINT);
	IloNumVarArray s(env, n, 0, IloInfinity, ILOINT);
	IloNumVarArray y(env, n, 0, 1, ILOINT);

#pragma endregion

#pragma region Objective Function
	IloExpr exp0(env);
	for (int t = 0; t < n; t++) {
		exp0 += f[t] * y[t] + p[t] * x[t] + h[t] * s[t];
	}

	Model.add(IloMinimize(env, exp0));
#pragma endregion

#pragma region Constraints

	// Initial demand satisfaction constraint
	Model.add(x[0] == d[0] + s[0]);

	// Demand satisfaction constraint
	for (int t = 1; t < n; t++)
	{
		Model.add(s[t - 1] + x[t] == d[t] + s[t]);
	}

	// Big M constraint
	for (int t = 0; t < n; t++)
	{
		Model.add(x[t] <= M[t] * y[t]);
	}

#pragma endregion

#pragma region Solve
	IloCplex cplex(Model);
	cplex.setOut(env.getNullStream()); // turn off the log file

	if (!cplex.solve()) {
		env.error() << "Failed to optimize the Master problem" << endl;
		throw(-1);
	}

	auto end = chrono::high_resolution_clock::now();
	auto Elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
	cout << "Elapsed Time(ms): " << Elapsed.count() << endl;

	double obj = cplex.getObjValue();

	cout << "The objective value is: " << obj << endl;

	//// Uncomment to cout the results
	//for (int t = 0; t < n; t++)
	//{
	//	int x_val = cplex.getValue(x[t]);
	//	cout << "\tx[" << t << "] = " << x_val << endl;
	//}

	//for (int t = 0; t < n; t++)
	//{
	//	int s_val = cplex.getValue(s[t]);
	//	cout << "\ts[" << t << "] = " << s_val << endl;
	//}

	//for (int t = 0; t < n; t++)
	//{
	//	int y_val = cplex.getValue(y[t]);
	//	cout << "\ty[" << t << "] = " << y_val << endl;
	//}
#pragma endregion
}