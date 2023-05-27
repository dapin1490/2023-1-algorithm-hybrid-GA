#include <iostream>
#include <vector>
#include <ctime> // 실행 시간 측정
#include <climits> // INT_MIN
#include <random> // 균등 난수 참고: https://modoocode.com/304
#include <tuple> // 튜플: https://jjeongil.tistory.com/148
#include <map> // pool 컨테이너
#include <set>
#include <string>
#include <fstream>
#include <cmath> // pow, log
#include <algorithm> // shuffle
using namespace std;

class GA {
	/*
	* 해결해야 할 문제
	* 주어진 그래프를 두 부류로 나누고, 각 부류를 연결하는 간선의 합이 최대가 되게 하라.
	*/
private:
	int idx0 = 1, idx1 = 1, idx2 = 1; // 대륙별 세대 인덱스
	mt19937 gen; // 난수 생성기
	clock_t start_timestamp; // 프로그램 시작 시간
	vector<vector<pair<int, int>>> graph; // 문제 그래프
	/* 유전자 풀: 가중치에 따른 선택을 위해 카운팅 배열 방식으로 저장 */
	map<int, vector<vector<string>>> pool; // continentA[0], continentB[1], total[2]
	vector<tuple<int, int, string>> temp_pool; // 임시 자식 풀: 대륙 번호[0, 1], cost, 유전자
	int thresh; // 부모 쌍 cost 차이 제한
	tuple<int, string> sol; // 반환할 해
	map<string, int> memo; // 지역 최적화에서 생성된 해와 cost 기억
	vector<int> verts; // 지역 최적화 인덱스 셔플 벡터

private:
	// thresh 설정
	void set_thresh(int thr) { thresh = thr; };
	// 시간 초과 확인
	bool is_timeout(int deadline, bool is_print);
	// 현재 pool에서 가장 좋은 해 반환
	tuple<int, string> get_current_best();
	// flatten two continents into one continent
	void flat_pool();
	// 해 유효성 확인 및 cost 계산
	int validate(string chromosome);
	// 해 생성
	string generate();
	// 부모 쌍 선택: 토너먼트 이용
	tuple<string, int, string, int> selection(int contin);
	// 교배
	string crossover(string female, int fcost, string male, int mcost);
	// 돌연변이
	string mutation(string chromosome);
	// 세대 교체
	bool replacement(string chromosome, int cost, int contin);
	// 최고 해 지역 최적화
	void local_opt(int deadline);
	// 대륙별 진화
	void evolution(int due, int contin, int k, double cut_rate);

	// pool에 존재하는 모든 해의 cost 출력
	void print_pool(int idx, int contin);

public:
	GA() {
		random_device rd;
		mt19937 g(rd());
		this->gen = g;
		start_timestamp = clock();
	}
	GA(vector<vector<pair<int, int>>>& graph) {
		this->graph = graph;
		random_device rd;
		mt19937 g(rd());
		this->gen = g;
		start_timestamp = clock();
	}
	GA(vector<vector<pair<int, int>>>& graph, mt19937 gen) {
		this->graph = graph;
		this->gen = gen;
		start_timestamp = clock();
	}
	GA(vector<vector<pair<int, int>>>& graph, clock_t start) {
		this->graph = graph;
		random_device rd;
		mt19937 g(rd());
		this->gen = g;
		start_timestamp = start;
	}
	GA(vector<vector<pair<int, int>>>& graph, mt19937 gen, clock_t start) {
		this->graph = graph;
		this->gen = gen;
		start_timestamp = start;
	}

	// 유전 알고리즘 실행
	tuple<int, string> execute(int due = 30);
	// 해 문자열과 가중치 반환
	tuple<int, string> get_solution();
	// 정답 반환
	string to_string_solution();
};

int main()
{
	// 빠른 입출력
	ios::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

	// 코드 실행 시간 측정 : https://scarlettb.tistory.com/5
	clock_t clock_start, clock_finish;
	double clock_duration = 0;

	clock_start = clock();

	/*// 제출용 입출력
	ifstream input{ "maxcut.in" };
	ofstream output{ "maxcut.out" };*/

	// 노드 50개 테스트
	ifstream input50{ "res/unweighted_50.txt" };
	ofstream output50{ "res/un50test.csv" };

	// 노드 100개 테스트
	ifstream input100{ "res/unweighted_100.txt" };
	ofstream output100{ "res/un100test.csv" };

	// 노드 500개 테스트
	ifstream input500{ "res/weighted_500.txt" };
	ofstream output500{ "res/w500test.csv" };
	
	/*// 노드 750개 테스트
	ifstream input750{ "res/maxcut750.txt" };
	ofstream output750{ "res/un750test.csv" };

	// 노드 1000개 테스트
	ifstream input1000{ "res/maxcut1000.txt" };
	ofstream output1000{ "res/un1000test.csv" };*/

	// 프로그램 실행 시작
	int v, e; // 정점 수 v, 간선 수 e
	int from, to; // 출발점, 도착점
	int w; // 가중치
	vector<vector<pair<int, int>>> graph;
	GA agent;
	int due = 170, iter = 50; // 시간 제한(초), 반복 수

	/*// 제출용 실행 코드
	input >> v >> e; // 그래프 정보 입력

	graph.clear();
	graph.resize(v + 1, vector<pair<int, int>>()); // 그래프 생성

	// 그래프 노드 입력
	for (int i = 0; i < e; i++) {
		input >> from >> to >> w;
		graph[from].emplace_back(to, w);
		graph[to].emplace_back(from, w);
	}

	// 유전 알고리즘 실행 후 결과 출력
	agent = GA(graph);
	tuple<int, string> sol = agent.execute(due);
	output << agent.to_string_solution() << "\n";*/

	// un 50 test
	clock_start = clock();

	input50 >> v >> e; // 그래프 정보 입력

	graph.clear();
	graph.resize(v + 1, vector<pair<int, int>>()); // 그래프 생성

	// 그래프 노드 입력
	for (int i = 0; i < e; i++) {
		input50 >> from >> to >> w;
		graph[from].emplace_back(to, w);
		graph[to].emplace_back(from, w);
	}

	// unweighted_50.txt 테스트
	cout << "\nres/unweighted_50.txt 테스트 \n";
	output50 << ",cost,solution\n";
	for (int i = 1; i <= iter; i++) {
		double iter_start = double(clock());
		cout << "test # " << i << "\n";
		agent = GA(graph);
		tuple<int, string> sol = agent.execute(due);
		cout << "time: " << (double(clock()) - iter_start) / CLOCKS_PER_SEC << "s\n";
		cout << "solution cost: " << get<0>(sol) << "\n\n";
		output50 << i << "," << get<0>(sol) << "," << agent.to_string_solution() << "\n";
	}

	clock_finish = clock();

	clock_duration += (double(clock_finish) - double(clock_start)) / CLOCKS_PER_SEC / 60; // 분 단위로 환산
	cout << "un 50: " << (double(clock_finish) - double(clock_start)) / CLOCKS_PER_SEC / 60 << "min";
	cout << "\n누적 실행 시간 : " << clock_duration << "min\n";

	// un 100 test
	clock_start = clock();

	input100 >> v >> e; // 그래프 정보 입력

	graph.clear();
	graph.resize(v + 1, vector<pair<int, int>>()); // 그래프 생성

	// 그래프 노드 입력
	for (int i = 0; i < e; i++) {
		input100 >> from >> to >> w;
		graph[from].emplace_back(to, w);
		graph[to].emplace_back(from, w);
	}

	// unweighted_100.txt 테스트
	cout << "\nres/unweighted_100.txt test\n";
	output100 << ",cost,solution\n";
	for (int i = 1; i <= iter; i++) {
		double iter_start = double(clock());
		cout << "test # " << i << "\n";
		agent = GA(graph);
		tuple<int, string> sol = agent.execute(due);
		cout << "time: " << (double(clock()) - iter_start) / CLOCKS_PER_SEC << "s\n";
		cout << "solution cost: " << get<0>(sol) << "\n\n";
		output100 << i << "," << get<0>(sol) << "," << agent.to_string_solution() << "\n";
	}

	clock_finish = clock();

	clock_duration += (double(clock_finish) - double(clock_start)) / CLOCKS_PER_SEC / 60; // 분 단위로 환산
	cout << "un 100: " << (double(clock_finish) - double(clock_start)) / CLOCKS_PER_SEC / 60 << "min";
	cout << "\n누적 실행 시간 : " << clock_duration << "min\n";

	// w 500 test
	clock_start = clock();

	input500 >> v >> e; // 그래프 정보 입력

	graph.clear();
	graph.resize(v + 1, vector<pair<int, int>>()); // 그래프 생성

	// 그래프 노드 입력
	for (int i = 0; i < e; i++) {
		input500 >> from >> to >> w;
		graph[from].emplace_back(to, w);
		graph[to].emplace_back(from, w);
	}

	// weighted_500.txt 테스트
	cout << "\nres/weighted_500.txt 테스트\n";
	output500 << ",cost,solution\n";
	for (int i = 1; i <= iter; i++) {
		double iter_start = double(clock());
		cout << "test # " << i << "\n";
		agent = GA(graph);
		tuple<int, string> sol = agent.execute(due);
		cout << "time: " << (double(clock()) - iter_start) / CLOCKS_PER_SEC / 60 << "min\n";
		cout << "solution cost: " << get<0>(sol) << "\n\n";
		output500 << i << "," << get<0>(sol) << "," << agent.to_string_solution() << "\n";
	}

	// 종료 시간 측정
	clock_finish = clock();

	clock_duration += (double(clock_finish) - double(clock_start)) / CLOCKS_PER_SEC / 60; // 분 단위로 환산
	cout << "w 500: " << (double(clock_finish) - double(clock_start)) / CLOCKS_PER_SEC / 60 << "min";
	cout << "\n누적 실행 시간 : " << clock_duration << "min\n";

	/*// un 750 test
	clock_start = clock();

	input750 >> v >> e; // 그래프 정보 입력

	graph.clear();
	graph.resize(v + 1, vector<pair<int, int>>()); // 그래프 생성

	// 그래프 노드 입력
	for (int i = 0; i < e; i++) {
		input750 >> from >> to >> w;
		graph[from].emplace_back(to, w);
		graph[to].emplace_back(from, w);
	}

	// maxcut750.txt 테스트
	cout << "\nres/maxcut750.txt 테스트 \n";
	output750 << ",cost,solution\n";
	for (int i = 1; i <= iter; i++) {
		double iter_start = double(clock());
		cout << "test # " << i << "\n";
		agent = GA(graph);
		tuple<int, string> sol = agent.execute(due);
		cout << "time: " << (double(clock()) - iter_start) / CLOCKS_PER_SEC / 60 << "min\n";
		cout << "solution cost: " << get<0>(sol) << "\n\n";
		output750 << i << "," << get<0>(sol) << "," << agent.to_string_solution() << "\n";
	}

	clock_finish = clock();

	clock_duration += (double(clock_finish) - double(clock_start)) / CLOCKS_PER_SEC / 60; // 분 단위로 환산
	cout << "un 750: " << (double(clock_finish) - double(clock_start)) / CLOCKS_PER_SEC / 60 << "min";
	cout << "\n누적 실행 시간 : " << clock_duration << "min\n";

	// un 1000 test
	clock_start = clock();

	input1000 >> v >> e; // 그래프 정보 입력

	graph.clear();
	graph.resize(v + 1, vector<pair<int, int>>()); // 그래프 생성

	// 그래프 노드 입력
	for (int i = 0; i < e; i++) {
		input1000 >> from >> to >> w;
		graph[from].emplace_back(to, w);
		graph[to].emplace_back(from, w);
	}

	// maxcut1000.txt 테스트
	cout << "\nres/maxcut1000.txt 테스트 \n";
	output1000 << ",cost,solution\n";
	for (int i = 1; i <= iter; i++) {
		double iter_start = double(clock());
		cout << "test # " << i << "\n";
		agent = GA(graph);
		tuple<int, string> sol = agent.execute(due);
		cout << "time: " << (double(clock()) - iter_start) / CLOCKS_PER_SEC / 60 << "min\n";
		cout << "solution cost: " << get<0>(sol) << "\n\n";
		output1000 << i << "," << get<0>(sol) << "," << agent.to_string_solution() << "\n";
	}

	clock_finish = clock();

	clock_duration += (double(clock_finish) - double(clock_start)) / CLOCKS_PER_SEC / 60; // 분 단위로 환산
	cout << "un 1000: " << (double(clock_finish) - double(clock_start)) / CLOCKS_PER_SEC / 60 << "min";
	cout << "\n누적 실행 시간 : " << clock_duration << "min\n";*/

	cout << "\n프로그램 실행 시간 : " << clock_duration << "min\n";

	return 0;
}

// 제한 시간 초과 확인
bool GA::is_timeout(int deadline, bool is_print = false) {
	clock_t running_time = clock();
	double time_len = (double(running_time) - double(start_timestamp)) / CLOCKS_PER_SEC;
	if (is_print)
		cout << "current time: " << time_len << "\n";
	if (time_len >= deadline) {
		if (is_print)
			cout << "time over\n";
		return true;
	}
	return false;
}

// 현재 pool에서 가장 좋은 해 반환
tuple<int, string> GA::get_current_best() {
	for (map<int, vector<vector<string>>>::iterator i = --pool.end(); (i != pool.begin() || i == pool.begin()); --i) {
		if (i->second[2].size() > 0) { // 2차 진화 pool
			this->sol = make_tuple(i->first, i->second[2].front());
			return make_tuple(i->first, i->second[2].front());
		}
		else if (i->second[0].size() > 0) { // 1차 진화 continentA
			this->sol = make_tuple(i->first, i->second[0].front());
			return make_tuple(i->first, i->second[0].front());
		}
		else if (i->second[1].size() > 0) { // 1차 진화 continentB
			this->sol = make_tuple(i->first, i->second[1].front());
			return make_tuple(i->first, i->second[1].front());
		}
	}
	return make_tuple(INT_MIN, "");
}

// flatten two continents into one continent
void GA::flat_pool() {
	map<int, vector<vector<string>>>::iterator iter;

	for (iter = pool.begin(); iter != pool.end(); iter++) {
		if (iter->second[0].size() + iter->second[1].size() < 1)
			continue;
		if (iter->second[0].size() > 0) {
			for (string& chrom : iter->second[0]) {
				pool[iter->first][2].push_back(chrom);
			}
		}
		if (iter->second[1].size() > 0) {
			for (string& chrom : iter->second[1]) {
				pool[iter->first][2].push_back(chrom);
			}
		}
	}

	return;
}

// 해 유효성 검사 및 가중치 계산
int GA::validate(string chromosome) {
	/*
	* 유효한 해의 조건
	* 두 부류는 최소한 1개 이상의 노드를 가져야 한다. 어느 한 부류에 모든 노드가 포함될 수 없다.
	* 두 부류는 최소 1개 이상의 간선으로 서로 연결되어야 한다: 한쪽 부류에 있는 노드가 갖는 모든 간선 중 반대쪽 부류로 이어지는 게 하나라도 있으면 통과
	*/
	set<int> a, b; // 두 부류의 노드 모음
	set<int>& key_pool = a; // 가중치 계산할 때 키가 되는 노드
	int cost = 0; // 계산된 가중치
	bool flag = false; // 두 부류가 최소 하나 이상의 간선으로 연결되었는지
	bool is_ok = false; // 두 부류 사이의 간선이 맞는지

	if (memo.find(chromosome) != memo.end())
		return memo[chromosome];

	// 해의 길이는 그래프 노드 수와 같아야 함
	if (chromosome.length() != graph.size() - 1)
		return INT_MIN;

	// 두 부류 분리
	for (int i = 0; i < chromosome.length(); i++) {
		switch (chromosome.at(i)) {
		case 'A':
			a.insert(i + 1);
			break;
		case 'B':
			b.insert(i + 1);
			break;
		default: return INT_MIN; // 여기로 넘어온다면 해가 완전히 잘못 생성된 것
		}
	}

	// 두 부류는 최소한 1개 이상의 노드를 가져야 한다. 어느 한 부류에 모든 노드가 포함될 수 없다.
	if (a.size() == 0 || b.size() == 0)
		return INT_MIN;

	// 두 부류는 최소 1개 이상의 간선으로 서로 연결되어야 한다: 수가 더 적은 쪽의 노드를 전수조사하여 cost도 동시에 계산
	if (a.size() > b.size()) {
		key_pool = b;
	}
	for (const int& node : key_pool) { // 가중치 계산 키 pool에서 노드별 반복
		for (pair<int, int> e : graph[node]) { // 노드에 연결된 모든 간선마다
			is_ok = find(key_pool.begin(), key_pool.end(), e.first) == key_pool.end(); // 도착점이 같은 pool 내에 존재하지 않으면 OK
			if (flag && is_ok)
				cost += e.second;
			else if (!flag && is_ok) {
				flag = true; // 정상 해 flag
				cost += e.second;
			}
		}
	}

	if (flag) {
		if (memo.find(chromosome) == memo.end())
			memo.emplace(chromosome, cost);
		return cost;
	}
	else // 두 부류가 서로 연결되지 않았다면 무효한 해
		return INT_MIN;
}

// 랜덤 해 생성
string GA::generate() {
	int len = graph.size() - 1; // 해의 길이
	string chromosome = ""; // 생성될 해
	uniform_int_distribution<int> dis(0, 1); // 난수 생성 범위 지정

	for (int i = 0; i < len; i++) { // 50% 확률로 두 문자 중 하나 선택하여 추가
		switch (dis(this->gen)) {
		case 0: chromosome.push_back('A'); break;
		case 1: chromosome.push_back('B'); break;
		default: i--;
		}
	}

	return chromosome;
}

// 부모 선택
tuple<string, int, string, int> GA::selection(int contin) {
	/*
	* 부모 선택 과정
	* 지정된 continent에서 일정량(2의 거듭제곱, 그래프 크기 비례)의 cost 뽑기: 뽑힌 cost에 해당하는 해가 최소 1개 존재해야 함
	* 뽑힌 cost끼리 토너먼트
	* 최종 승자 cost에 해당하는 해 랜덤으로 뽑기 -> female parent

	* 예외 교배 판정 실행: 낮은 확률로 cost 차이가 큰 부모가 생성될 수 있음
	* 예외 교배 판정에 따라 정해진 cost 범위 내에서 male의 cost 뽑기: 뽑힌 cost에 해당하는 해가 최소 1개 존재해야 함
		* 예외: 아무리 뽑아도 해당하는 해가 없다면 female과 cost가 같은 해 중 다시 하나를 뽑아 교배
	* 뽑힌 cost에 해당하는 해 랜덤으로 뽑기 -> male parent
	*/
	tuple<string, int, string, int> parents; // 선택된 부모: female 먼저 선택 후 male 선택
	int n_candis = pow(2, int(round(log(double(graph.size() - 1) * 0.3)))); // 뽑을 후보의 수
	uniform_int_distribution<int> pick_cost(pool.begin()->first, (--pool.end())->first); // cost 뽑기
	uniform_int_distribution<int> pick_chromo(1, 10); // 둘 중 이긴 유전자 뽑기
	uniform_int_distribution<int> special_love(5, 1000); // cost 차이가 큰 쌍이 생성될 확률 0.5%
	int ca, cb, len;
	vector<int> candidates; // 토너먼트에 참가할 female cost 후보
	int break_count = 0;
	bool break_flag = false; // 만약 female의 선택 범위에 male이 존재하지 않는다면 같은 cost 교배

	// female 후보 뽑기: 2^(N*0.3)에 가장 가까운 2의 거듭제곱만큼, 중복은 고려하지 않음
	for (int i = 0; i < n_candis; i++) {
		while (true) { // 유효한 후보가 나올 때까지 뽑기
			ca = pick_cost(this->gen); // cost 선택: 존재하는 모든 cost 중 뽑기 때문에 무조건 하나는 있음
			if (pool.find(ca) != pool.end() && pool[ca][contin].size() != 0) // 선택한 cost를 갖는 해가 있는지 확인
				break;
		}
		candidates.push_back(ca); // 후보 추가
	}

	// 뽑힌 cost로 토너먼트: 승자를 왼쪽에 저장, 최종 승자는 0번에 저장됨
	for (int i = 1; i < n_candis; i *= 2) {
		for (int j = 0; j < n_candis - 1; j += 2 * i) {
			ca = (candidates[j] > candidates[j + i] ? candidates[j] : candidates[j + i]);
			cb = candidates[j] + candidates[j + i] - ca;
			candidates[j] = (pick_chromo(this->gen) >= 6 ? ca : cb);
		}
	}

	// 최종 승자 cost를 갖는 해 중에서 랜덤하게 female 선택
	len = pool[candidates[0]][contin].size(); // 후보 수: 최소 하나 이상 있는 것만 후보로 넣었기 때문에 무조건 있음
	get<0>(parents) = pool[candidates[0]][contin][uniform_int_distribution<int>(0, len - 1)(this->gen)]; // 뽑기
	get<1>(parents) = candidates[0]; // 뽑힌 female의 가중치

	if (special_love(this->gen) > 5) { // 예외 교배 발생 판정: 발생하지 않으면 female의 cost보다 나은 male만을 선택하게 함
		pick_cost = uniform_int_distribution<int>(candidates[0] + 1, candidates[0] + thresh);
	}

	// male 선택
	while (true) { // 뽑히거나, 포기할 때까지 반복
		cb = pick_cost(this->gen); // male의 cost 뽑기
		if (pool.find(cb) != pool.end() && pool[cb][contin].size() != 0) // 해 존재 확인
			break;
		if (break_count > thresh * 2) { // 아무리 뽑아도 해가 없으면
			break_flag = true; // 같은 cost 교배 flag true
			break;
		}
		break_count++;
	}
	if (!break_flag) { // 뽑을 male이 있음
		len = pool[cb][contin].size();
		get<2>(parents) = pool[cb][contin][uniform_int_distribution<int>(0, len - 1)(this->gen)];
		get<3>(parents) = cb;
	}
	else { // male이 없어서 같은 cost 교배
		get<2>(parents) = pool[candidates[0]][contin][uniform_int_distribution<int>(0, len - 1)(this->gen)]; // 뽑기
		get<3>(parents) = candidates[0]; // 뽑힌 female의 가중치
	}

	return parents;
}

// 교배
string GA::crossover(string female, int fcost, string male, int mcost) {
	string child = ""; // 생성될 자식
	uniform_int_distribution<int> dis(1, 10); // 난수 생성 범위 지정
	string& upper = male; // cost가 높은 부모
	string& lower = female; // cost가 낮은 부모

	if (fcost > mcost) { // 상하관계 정리
		upper = female;
		lower = male;
	}

	// 60% 확률로 cost가 더 큰 쪽의 유전자를 받음
	for (int i = 0; i < graph.size() - 1; i++) {
		if (dis(this->gen) <= 6)
			child.push_back(upper.at(i));
		else
			child.push_back(lower.at(i));
	}
	return mutation(child);
}

// 돌연변이
string GA::mutation(string chromosome) {
	uniform_int_distribution<int> is_mutate(1, 100000); // 돌연변이 발생 확률 조절: 그래프 크기에 비례해 총 발생 확률이 0.8%가 되게 조정
	uniform_int_distribution<int> where_mutate(0, graph.size() - 2); // 돌연변이 발생 자리 선택: 복원 추출
	uniform_int_distribution<int> choose(0, 1); // 돌연변이 발생시 문자 재선택: 돌연변이가 발생해도 원본과 똑같을 수 있음
	int cnt = (graph.size() - 1) / 50 + 1; // 돌연변이 발생 횟수: 1개부터 시작해서 50개 단위로 1개씩 증가
	int p = (1 - pow(0.992, 1 / double(cnt))) * 100000; // 개별 돌연변이 발생 확률

	for (int i = 0; i < cnt; i++) {
		if (is_mutate(this->gen) <= p) {
			chromosome.replace(where_mutate(this->gen), 1, (choose(this->gen) == 0 ? "A" : "B"));
		}
	}
	return chromosome;
}

// 세대 교체
bool GA::replacement(string chromosome, int cost, int contin) {
	uniform_int_distribution<int> gen_cost(1, thresh); // 자식과 교체 대상의 cost 차이 생성
	int r_cost; // 교체 대상의 cost
	int break_count = 0; // 교체 실패 count
	int s; // 교체 대상 해의 수

	while (true) { // 교체 대상의 cost 뽑기: 유효한 cost가 나오거나 포기할 때까지 반복
		r_cost = cost - gen_cost(this->gen);
		if ((pool.find(r_cost) != pool.end() && pool[r_cost][contin].size() != 0) || break_count > thresh * 2)
			break;
		break_count++;
	}

	if (break_count > thresh * 2) { // 대체할 cost를 선택하지 못했으면 대체하지 않고 패스
		if (cost > get<0>(get_current_best())) { // 예외: 신기록 경신하면 바로 pool에 추가
			if (pool.find(cost) == pool.end()) { // 추가할 자식의 cost가 pool에 없으면 추가
				pool.emplace(cost, vector<vector<string>>(3));
			}
			pool[cost][contin].push_back(chromosome); // 자식 추가
			return true;
		}
		return false;
	}

	s = pool[r_cost][contin].size(); // 교체 가능 대상의 수

	s = uniform_int_distribution<int>(0, s - 1)(this->gen); // 교체 대상의 인덱스 뽑기
	pool[r_cost][contin].erase(pool[r_cost][contin].begin() + s); // 교체 대상 삭제

	if (pool.find(cost) == pool.end()) { // 추가할 자식의 cost가 pool에 없으면 추가
		pool.emplace(cost, vector<vector<string>>(3));
	}
	pool[cost][contin].push_back(chromosome); // 자식 추가
	return true; // 교체 성공
}

// 최고 해 지역 최적화
void GA::local_opt(int deadline) {
	this->sol = get_current_best();
	string ans_before = get<1>(sol), ans_after = get<1>(sol);
	int cost_before = get<0>(sol), cost_after = get<0>(sol);
	pair<int, string> branch_before, branch_after;
	bool improved = true;
	int stop_count = 0;
	random_device rd;
	default_random_engine rng(rd());

	if (memo.find(ans_before) == memo.end()) {
		memo.emplace(ans_before, cost_before);
	}

	while (improved) {
		improved = false;
		shuffle(verts.begin(), verts.end(), rng); // 셔플 참고: https://www.delftstack.com/ko/howto/cpp/shuffle-vector-cpp/

		for (int& i : verts) {
			if (is_timeout(deadline) || stop_count >= graph.size() * graph.size()) {
				this->sol = make_tuple(cost_after, ans_after);
				if (pool.find(cost_after) == pool.end()) { // 추가할 자식의 cost가 pool에 없으면 추가
					pool.emplace(cost_after, vector<vector<string>>(3));
				}
				pool[cost_after][0].push_back(ans_after); // 자식 추가
				pool[cost_after][1].push_back(ans_after); // 자식 추가
				pool[cost_after][2].push_back(ans_after); // 자식 추가
				return;
			}

			ans_after = ans_before;
			cost_after = cost_before;

			switch (ans_after.at(i)) {
			case 'A': ans_after.replace(i, 1, "B"); break;
			case 'B': ans_after.replace(i, 1, "A"); break;
			}

			if (memo.find(ans_after) != memo.end()) {
				cost_after = memo[ans_after];
			}
			else {
				cost_after = validate(ans_after);
				memo.emplace(ans_after, cost_after);
			}

			if (cost_after >= cost_before) {
				if (cost_after == cost_before)
					stop_count++;
				else
					stop_count = 0;
				ans_before = ans_after;
				cost_before = cost_after;
				improved = true;
			}
		}
	}

	this->sol = make_tuple(cost_after, ans_after);
	if (pool.find(cost_after) == pool.end()) { // 추가할 자식의 cost가 pool에 없으면 추가
		pool.emplace(cost_after, vector<vector<string>>(3));
	}
	pool[cost_after][0].push_back(ans_after); // 자식 추가
	pool[cost_after][1].push_back(ans_after); // 자식 추가
	pool[cost_after][2].push_back(ans_after); // 자식 추가
	return;
}

// 대륙별 진화
void GA::evolution(int due, int contin, int k, double cut_rate = 0.3) {
	/*
	* 부모 선택
	* 돌연변이
	* 세대 교체
	* 수렴 후 종료
	*/
	uniform_int_distribution<int> plz_add_me(1, 100); // 대체 대상이 없는 자식이 pool에 추가될 확률 2%
	bool is_child_added = false; // 자식이 pool에 추가되었는지
	int cut_count = 0; // 대체 실패한 자식 수
	int* idx = &idx0; // 세대 수
	tuple<string, int, string, int> parent; // 교배에 사용할 부모 쌍
	pair<int, string> child; // 교배 후 생성된 자식

	switch (contin) {
	case 1: idx = &idx1; break;
	case 2: idx = &idx2; break;
	}

	// 부모 선택, 교배, 세대 교체
	while (true) { // 조건을 만족할 때까지 진화, 제한 시간 임박하면 종료
		// 시간 제한 확인
		if (is_timeout(due)) {
			return;
		}

		// 임시 자식 풀 초기화
		temp_pool.clear();

		// 종료 조건 초기화
		cut_count = 0;

		// 자식 생성
		// cout << "generate children\n";
		for (int i = 0; i < k; i++) {
			// 부모 선택
			parent = selection(contin);
			// 교배 및 유효성 확인
			child.second = crossover(get<0>(parent), get<1>(parent), get<2>(parent), get<3>(parent));
			child.first = validate(child.second);
			if (child.first != INT_MIN) {
				temp_pool.push_back(make_tuple(contin, child.first, child.second));
			}
			// thresh 예외 추가 자식
			if (abs(get<1>(parent) - get<3>(parent)) > thresh) {
				child.second = crossover(get<0>(parent), get<1>(parent), get<2>(parent), get<3>(parent));
				child.first = validate(child.second);
				if (child.first != INT_MIN) {
					temp_pool.push_back(make_tuple(contin, child.first, child.second));
				}
			}
		}
		// 시간 제한 확인
		// cout << "children generation complete\n";
		if (is_timeout(due)) {
			return;
		}

		// 세대 교체
		// cout << "replace\n";
		for (auto& child : temp_pool) {
			is_child_added = replacement(get<2>(child), get<1>(child), get<0>(child));
			if (!is_child_added && plz_add_me(this->gen) <= 2) {
				if (pool.find(get<1>(child)) == pool.end()) {
					pool.emplace(get<1>(child), vector<vector<string>>(3));
				}
				pool[get<1>(child)][get<0>(child)].push_back(get<2>(child));
				is_child_added = true;
			}
			else if (!is_child_added)
				cut_count++;
		}

		//print_pool((*idx)++, contin);

		// 시간 제한 확인
		// cout << "children replace complete\n";
		if (is_timeout(due)) {
			return;
		}

		if (cut_count > int(double(k) * cut_rate)) { // 생성된 자식의 지정 비율 이상이 대체되지 못했다면 진화 수렴 판단
			// cout << "evolution complete\n";
			break;
		}
	}
}

// pool에 존재하는 모든 해의 cost 출력
void GA::print_pool(int idx, int contin) {
	map<int, vector<vector<string>>>::iterator iter; // map iterator: https://dar0m.tistory.com/98
	string index = to_string(idx).append(",");
	string cont_a = index, cont_b = index, cont_total = index;

	cout << idx << "," << contin << ",";

	for (iter = pool.begin(); iter != pool.end(); iter++) {
		// iter,continent,pool
		// ex: 1,3,90*1 91*1 92*2 93*1 95*1
		if (iter->second[contin].size() > 0) {
			cout << iter->first << "*" << iter->second[contin].size() << " ";
		}
	}
	cout << "\n";

	return;
}

// 유전 알고리즘 실행
tuple<int, string> GA::execute(int due) { // due: 프로그램 실행 마감시간
	/*
	* 랜덤 해 생성
	* 대륙 내 교배
		* 부모 선택
		* 돌연변이
		* 세대 교체
	* 1차 수렴
	* 대륙 외 교배
	* 2차 수렴 후 종료
	*/
	int n_pool = min(150, int(37500 / (this->graph.size() - 1))); // 초기 생성 pool 크기
	int k = n_pool * 0.3; // 한 세대 수
	uniform_int_distribution<int> plz_add_me(1, 100); // 대체 대상이 없는 자식이 pool에 추가될 확률 2%
	bool is_child_added = false; // 자식이 pool에 추가되었는지
	int cut_count = 0; // 대체 실패한 자식 수

	// 지역 최적화에 사용할 인덱스 셔플 벡터 초기화
	verts.resize(graph.size() - 1);
	for (int i = 0; i < graph.size() - 1; i++)
		verts[i] = i;

	// 랜덤 해 생성
	// cout << "generate\n";
	/*map<int, vector<vector<string>>> pool; // female[0], male[1]*/
	for (int i = 0; i < 2 * n_pool; i++) { // 두 대륙 포함해 2 * n_pool 만큼 생성, 개체수는 서로 같게 함
		string chromosome = generate();
		int cost = validate(chromosome);
		if (cost != INT_MIN) { // 유효한 해만 pool에 추가
			if (pool.find(cost) == pool.end()) {
				pool.emplace(cost, vector<vector<string>>(3));
			}
			pool[cost][i % 2].push_back(chromosome);
		}
		else
			i--;
		if (is_timeout(due)) {
			return get_current_best();
		}
	}

	/*print_pool(idx0++, 0);
	print_pool(idx1++, 1);*/

	// cout << "generate complete\n";
	if (is_timeout(due)) {
		return get_current_best();
	}

	// 부모 쌍 cost 차이 제한
	set_thresh(max(int(((--pool.end())->first - pool.begin()->first) * 0.1), 5));

	// 1차 진화: continentA
	evolution(due, 0, k);
	local_opt(due * 0.1); // 지역 최적화

	// 시간 제한 확인
	if (is_timeout(due)) {
		return get_current_best();
	}

	// 1차 진화: continentB
	evolution(due, 1, k);
	local_opt(due * 0.2); // 지역 최적화

	// 시간 제한 확인
	if (is_timeout(due)) {
		return get_current_best();
	}

	// 2차 진화: continent total
	flat_pool(); // 대륙 통일
	local_opt(due * 0.3); // 지역 최적화
	evolution(due, 2, k);
	
	// 시간 제한 확인
	// cout << "evolution complete\n";
	if (is_timeout(due)) {
		return get_current_best();
	}

	// 지역 최적화
	local_opt(due);
	//print_pool(idx2++, 2);

	// cout << "return solution\n";
	return get_current_best();
}

tuple<int, string> GA::get_solution() {
	return get_current_best();
};

string GA::to_string_solution() {
	char key = get<1>(sol).at(0);
	string answer = "";

	for (int i = 1; i <= get<1>(sol).length(); i++) {
		if (get<1>(sol).at(i - 1) == key) {
			answer.append(to_string(i));
			answer.push_back(' ');
		}
	}

	return answer;
}