#include <iostream>
#include <fstream>
#include <ctime> // ���� �ð� ����
#include <vector>
#include <string>
#include <map>
using namespace std;

enum Oper { CONSTANT, VARIABLE, PLUS, MINUS, MULTI, DIVD, SIN, COS, TAN, SINH, COSH, TANH, LOG, EXP };

vector<double> parse_line(string line) {
	vector<double> data;
	string word;
	size_t idx = line.find(",");

	while (idx != line.npos) {
		word = line.substr(0, idx);
		line = line.substr(idx + 1);
		data.push_back(stod(word));
		idx = line.find(",");
	}
	data.push_back(stod(line));

	return data;
}

vector<vector<double>> read_csv(string file_route) {
	ifstream input{ file_route };
	vector<vector<double>> data;
	string line;

	while (!input.eof()) {
		input >> line;
		data.push_back(parse_line(line));
	}

	input.close();
	return data;
}

struct Node {
	Oper node_type;
	double value;
	Node* left;
	Node* right;
};

class Expression {
private:
	Node* root;
public:
	Expression() { this->root = new Node{Oper::CONSTANT, 0, nullptr, nullptr}; }
};

class GP {
private:
	vector<vector<double>> data;
	vector<Expression> pool;
public:
	GP() {}
};

int main()
{
	// ���� �����
	ios::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

	string file_route =  "dataset.csv";
	ofstream output{ "GP.out" };

	// �ڵ� ���� �ð� ���� : https://scarlettb.tistory.com/5
	double clock_start = clock();

	vector<vector<double>> data;

	data = read_csv(file_route);

	for (int i = 0; i < 5; i++) {
		cout << data[i][i] << "\n";
	}

	output.close();
	cout << "\n���α׷� ���� �ð� : " << (double(clock()) - clock_start) / CLOCKS_PER_SEC << "s\n";

	return 0;
}