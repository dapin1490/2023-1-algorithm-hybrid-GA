#include <iostream>
#include <fstream>
#include <ctime> // 실행 시간 측정
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
	int n_pool; // initial pool size
	vector<vector<double>> data; // data for prediction
	vector<Expression> pool; // solution pool

private:
	// 초기 pool 크기 설정
	void set_n_pool(int np) { n_pool = np; }
	// 시간 초과 확인
	bool is_timeout(int deadline, bool is_print);
	// 현재 pool에서 가장 좋은 해 반환
	tuple<double, Expression> get_current_best();
	// 해 유효성 확인 및 cost 계산
	double validate(Expression chromosome);
	// 해 생성
	Expression generate();
	// 부모 쌍 선택: 토너먼트 이용
	tuple<Expression, Expression> selection();
	// 교배
	Expression crossover(Expression female, Expression male);
	// 돌연변이
	Expression mutation(Expression& chromosome);
	// 세대 교체
	bool replacement(Expression chromosome, double cost);
	// 최고 해 지역 최적화
	void local_opt(int deadline);

	// pool에 존재하는 모든 해의 cost 출력
	void print_pool(int idx);
public:
	GP() {}
};

int main()
{
	// 빠른 입출력
	ios::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

	string file_route =  "dataset.csv";
	ofstream output{ "GP.out" };

	// 코드 실행 시간 측정 : https://scarlettb.tistory.com/5
	double clock_start = clock();

	vector<vector<double>> data;

	data = read_csv(file_route);

	for (int i = 0; i < 5; i++) {
		cout << data[i][i] << "\n";
	}

	output.close();
	cout << "\n프로그램 실행 시간 : " << (double(clock()) - clock_start) / CLOCKS_PER_SEC << "s\n";

	return 0;
}