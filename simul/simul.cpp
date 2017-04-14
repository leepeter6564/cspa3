#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>
#include <random>
#include <stdlib.h>
#include <numeric>
#include <cmath>

#define ITER 25000

using namespace std;


// KK algorithm
int64_t karkar(vector<int64_t> list){
	// while the list has at least 2 non-zero elements
	while(list.size() > 1){
		// pick the two largest elements and "set them to 0"
		int64_t big1 = *max_element(begin(list), end(list));
		list.erase(max_element(begin(list), end(list)));
		int64_t big2 = *max_element(begin(list), end(list));
		list.erase(max_element(begin(list), end(list)));
		// insert in the difference
		int64_t diff = big1-big2;
		list.push_back(diff);
	}
	return list[0];
}

// random number list generator between 1 and 10^12
vector<int64_t> num_list_gen(default_random_engine& gen){
	// pick a number between 1 and 10^12 inclusive
	uniform_int_distribution<int64_t> rand(1, pow(10, 12));
	// put those in a new vector
	vector<int64_t> num_list;
	for(int i = 0; i < 100; i++){
		num_list.push_back(rand(gen));
	}
	return num_list;
}

// prepartitioning algorithm; returns prepartitioned vector; requires KK
vector<int64_t> pgen(default_random_engine& gen, vector<int64_t> nlist){
	uniform_int_distribution<int> rand(0, 99);
	vector<int> p;
	for(int i = 0; i < 100; i++){
		p.push_back(rand(gen));
	}
	vector<int64_t> nlist1(100, 0);
	for(int j = 0; j < 100; j++){
		nlist1[p[j]] += nlist[j];
	}
	return nlist1;
}

// standard form algorithm
vector<int64_t> sgen(default_random_engine& gen, vector<int64_t> nlist){
	uniform_int_distribution<int> rand(0, 1);
	vector<int> s;
	for(int i = 0; i < 100; i++){
		int io = rand(gen);
		int x = (io > 0) ? 1 : -1;
		s.push_back(x);
	}
	vector<int64_t> nlist1;
	for(int i = 0; i < 100; i++){
		nlist1.push_back(nlist[i] * s[i]);
	}
	return nlist1;
}



// TESTING
int64_t sgen1(default_random_engine& gen, vector<int64_t> nlist){
	uniform_int_distribution<int> rand(0, 1);
	int64_t sum = 0;
	for(int i = 0; i < 100; i++){
		int io = rand(gen);
		int x = (io > 0) ? 1 : -1;
		sum += x * nlist[i];
	}
	return abs(sum);
}

int64_t rr1_t(default_random_engine& gen, vector<int64_t> nlist){
	int64_t s0 = sgen1(gen, nlist);
	for(int i = 0; i < ITER; i++){
		int64_t s1 = sgen1(gen, nlist);
		s0 = (s1 < s0) ? s1 : s0;
	}
	return s0;
}

int64_t rr1_t1(default_random_engine& gen, vector<int64_t> nlist){
	vector<int64_t> s0 = sgen(gen, nlist);
	for(int i = 0; i < ITER; i++){
		vector<int64_t> s1 = sgen(gen, nlist);
		int64_t sum0 = 0;
		int64_t sum1 = 0;
		for( int i = 0; i < 100; i++){
			sum0 += s0[i];
			sum1 += s1[i];
		}
		cout << abs(sum0) << endl;
		s0 = (abs(sum1) < abs(sum0)) ? s1 : s0;
	}
	int64_t sum = 0;
	for( int i = 0; i < 100; i++){
		sum += s0[i];
	}
	return abs(sum);
}


// TESTING


// repeated random under standard form
int64_t rr1(default_random_engine& gen, vector<int64_t> nlist){
	vector<int64_t> s0 = sgen(gen, nlist);
	cout << abs(accumulate(s0.begin(), s0.end(), 0)) << endl;
	for(int i = 0; i < ITER; i++){
		vector<int64_t> s1 = sgen(gen, nlist);
		int64_t sum0 = accumulate(s0.begin(), s0.end(), 0);
		int64_t sum1 = accumulate(s1.begin(), s1.end(), 0);
		s0 = (abs(sum1) < abs(sum0)) ? s1 : s0;
		cout << sum0 << endl;
	}
	int64_t sum = accumulate(s0.begin(), s0.end(), 0);
	cout << sum << endl;
	return abs(sum);
}

// repeated random under prepartitioning
int64_t rr2(default_random_engine& gen, vector<int64_t> nlist){
	vector<int64_t> p0 = pgen(gen, nlist);
	for(int i = 0; i < ITER; i++){
		vector<int64_t> p1 = pgen(gen, nlist);
		p0 = (karkar(p1) < karkar(p0)) ? p1 : p0;
	}
	return karkar(p0);
}

// hill climbing under standard form
int64_t hc1(default_random_engine& gen, vector<int64_t> nlist){
	uniform_int_distribution<int> rand1(0, 99);
	uniform_int_distribution<int> rand2(0, 1);
	vector<int64_t> s0 = sgen(gen, nlist);
	for(int i = 0; i < ITER; i++){
		vector<int64_t> s1 = s0;
		int a = rand1(gen);
		int b = rand1(gen);
		s1[a] = (rand2(gen) > 0) ? s1[a] : -s1[a];
		s1[b] = (rand2(gen) > 0) ? s1[b] : -s1[b];

		int64_t sum0 = abs(accumulate(s0.begin(), s0.end(), 0));
		int64_t sum1 = abs(accumulate(s1.begin(), s1.end(), 0));
		s0 = (sum1 < sum0) ? s1 : s0;
	}
	int64_t sum = abs(accumulate(s0.begin(), s0.end(), 0));
	return sum;
}

// hill climbing under prepartitioning
int64_t hc2(default_random_engine& gen, vector<int64_t> nlist){
	uniform_int_distribution<int> rand1(0, 99);
	uniform_int_distribution<int> rand2(0, 1);
	vector<int64_t> p0 = pgen(gen, nlist);
	for(int i = 0; i < ITER; i++){
		vector<int64_t> p1 = p0;
		int a = rand1(gen);
		int b = rand1(gen);
		p1[a] = (rand2(gen) > 0) ? p1[a] : p1[b];
		p1[b] = (rand2(gen) > 0) ? p1[b] : p1[a];

		p0 = (karkar(p1) < karkar(p0)) ? p1 : p0;
	}
	return karkar(p0);
}

// simulated annealing under standard form
int64_t sa1(default_random_engine& gen, vector<int64_t> nlist){
	uniform_int_distribution<int> rand1(0, 99);
	uniform_int_distribution<int> rand2(0, 1);
	uniform_real_distribution<double> rand3(0, 1);
	vector<int64_t> s0 = sgen(gen, nlist);
	vector<int64_t> s2 = s0;
	for(int i = 0; i < ITER; i++){
		vector<int64_t> s1 = s0;
		int a = rand1(gen);
		int b = rand1(gen);
		s1[a] = (rand2(gen) > 0) ? s1[a] : -s1[a];
		s1[b] = (rand2(gen) > 0) ? s1[b] : -s1[b];

		int64_t sum0 = abs(accumulate(s0.begin(), s0.end(), 0));
		int64_t sum1 = abs(accumulate(s1.begin(), s1.end(), 0));
		int64_t sum2 = abs(accumulate(s2.begin(), s2.end(), 0));
		
		if(sum1 < sum0 || rand3(gen) < exp((double)-(sum1 - sum0) / 
			((double)pow(10, 10) * (double)pow((double)0.8, (double)i/(double)300)))){
			s0 = s1;
		}
		s2 = (sum0 < sum2) ? s0 : s2;
	}
	int64_t sum = abs(accumulate(s2.begin(), s2.end(), 0));
	return sum;
}

// simulated annealing under prepartitioning
int64_t sa2(default_random_engine& gen, vector<int64_t> nlist){
	uniform_int_distribution<int> rand1(0, 99);
	uniform_int_distribution<int> rand2(0, 1);
	uniform_real_distribution<double> rand3(0, 1);
	vector<int64_t> p0 = sgen(gen, nlist);
	vector<int64_t> p2 = p0;
	for(int i = 0; i < ITER; i++){
		vector<int64_t> p1 = p0;
		int a = rand1(gen);
		int b = rand1(gen);
		p1[a] = (rand2(gen) > 0) ? p1[a] : p1[b];
		p1[b] = (rand2(gen) > 0) ? p1[b] : p1[a];


		
		if(karkar(p1) < karkar(p0) || rand3(gen) < exp((double)-(karkar(p1) - karkar(p0)) / 
			((double)pow(10, 10) * (double)pow((double)0.8, (double)i/(double)300)))){
			p0 = p1;
		}
		p2 = (karkar(p0) < karkar(p2)) ? p0 : p2;
	}
	
	return karkar(p2);
}


int main(){
	long long seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine gen((unsigned)seed);


	vector<int64_t> numlist = num_list_gen(gen);
	cout << rr1_t1(gen, numlist) << endl;
	
	/*vector<int64_t> test1 = sgen(gen, numlist);
	vector<int64_t> test2 = sgen(gen, numlist);
	vector<int64_t> test3 = sgen(gen, numlist);
	vector<int64_t> test4 = sgen(gen, numlist);
	vector<int64_t> test5 = sgen(gen, numlist);


	for(int i = 0; i < 100; i++){
		cout << test1[i] << " : " << test2[i] << endl;
	}
	cout << "sum1: " << abs(accumulate(test1.begin(), test1.end(), 0)) << endl;
	cout << "sum2: " << abs(accumulate(test2.begin(), test2.end(), 0)) << endl;
	cout << "sum3: " << abs(accumulate(test3.begin(), test3.end(), 0)) << endl;
	cout << "sum4: " << abs(accumulate(test4.begin(), test4.end(), 0)) << endl;
	cout << "sum5: " << abs(accumulate(test5.begin(), test5.end(), 0)) << endl;*/



    return 0;
}