#include<iostream>
#include<fstream>
#include <sstream>
#include <vector>
#include<chrono>;
#include <limits>

#define MAX numeric_limits<int>::max()

using namespace std;

// Split a string by a delimiter
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

// Function to initialize G with G[0] = H0*s[0] + K1 and G[t] = MAX forall t=1,...,n
void initialize_G(int* G, int n, int H0, int* s, int K1) {
	G[0] = H0 * s[0] + K1;

	for (int i = 1; i < n; i++)
	{
		G[i] = MAX;
	}
}

// Recurrence Function that computes G[n]
int compute_G(unsigned int n, int* d, int* f, int* p, int* h, int* P, int* G, int* kappa) {
	if (n == 0)
	{
		return G[0];
	}

	int* l = new int[n];

	for (int k = 1; k < n + 1; k++)
	{
		if (G[k - 1] == MAX)
		{
			G[k - 1] = compute_G(k - 1, d, f, p, h, P, G, kappa);
		}

		int val = G[k - 1];
		int sum_d = 0;

		for (int q = k - 1; q < n; q++)
		{
			sum_d += d[q];
		}
			val += (f[k - 1] + P[k - 1] * sum_d);
			l[k - 1] = val;
		}
		
		int min_val = l[0];

		for (int i = 0; i < n; i++)
		{
			if (l[i] < min_val) {
				min_val = l[i];
			}
		}

	return min_val;
}

int main(int argc, char* argv[])
{
	auto start = chrono::high_resolution_clock::now();

#pragma region Problem data
	ifstream file;

	// Read a file from a specified path and name
	string file_path = "../data/";
	string file_name = "instance_0500.dat";

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

	// Define P[t]
	int* P = new int[n];
	for (int t = 0; t < n; t++)
	{
		P[t] = p[t];
		for (int q = t; q < n; q++) {
			P[t] += h[q];
		}
	}

	// Define H0
	int H0 = 0;
	for (int t = 0; t < n; t++)
	{
		H0 += h[t];
	}

	// Define K1
	int K1 = 0;
	for (int t = 0; t < n; t++)
	{
		for (int q = 0; q < t + 1; q++) {
			K1 -= (h[t] * d[q]);
		}
	}

	// Define s
	int* s = new int[n];
	s[0] = 0;

	// Define and initialize G
	int* G = new int[n];
	initialize_G(G, n, H0, s, K1);

	// Define kappa
	int* kappa = new int[n];

#pragma endregion

#pragma region Solve
	G[n] = compute_G(n, d, f, p, h, P, G, kappa);

	cout << G[n] << endl;

	auto end = chrono::high_resolution_clock::now();
	auto Elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
	cout << "Elapsed Time(ms): " << Elapsed.count() << endl;

#pragma endregion
}