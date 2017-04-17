#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>
#include <random>
#include <stdlib.h>
#include <numeric>
#include <cmath>
#include <time.h>
#include <algorithm>

#define ITER 25000

using namespace std;

// pick a number
uniform_int_distribution<int64_t> rando(1, pow(10, 12));
uniform_int_distribution<int> rand1(0, 99);
uniform_int_distribution<int> rand2(0, 1);
uniform_real_distribution<double> rand3(0, 1);

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

// returns absolute value of sums of all elements in a vector
int64_t vsumup(vector<int64_t> nlist){
	int64_t total = 0;
	for(int i = 0; i < 100; i++){
		total += nlist[i];
	}
	return abs(total);
}

// random number list generator between 1 and 10^12
vector<int64_t> num_list_gen(default_random_engine& gen){
	// put those in a new vector
	vector<int64_t> num_list;
	for(int i = 0; i < 100; i++){
		num_list.push_back(rando(gen));
	}
	return num_list;
}

// standard form algorithm
vector<int64_t> sgen(default_random_engine& gen, vector<int64_t> nlist){
	vector<int> s;
	for(int i = 0; i < 100; i++){
		int io = rand1(gen);
		int x = (io > 0) ? 1 : -1;
		s.push_back(x);
	}
	vector<int64_t> nlist1;
	for(int i = 0; i < 100; i++){
		nlist1.push_back(nlist[i] * s[i]);
	}
	return nlist1;
}


// prepartitioning: done in two steps
// first give partition ordering
vector<int64_t> prepart(default_random_engine& gen){
	vector<int64_t> partition;
	for(int i = 0; i < 100; i++){
		randlst.push_back(rand1(gen));
	}
	return partition;
}

// second produce partitioned vector
vector<int64_t> partition(vector<int64_t> prepart, vector<int64_t> nlist)
	vector<int64_t> nlist1(100, 0);
	for(int j = 0; j < 100; j++){
		nlist1[prepart[j]] += nlist[j];
	}
	return nlist1;
}

// repeated random under standard form
int64_t rr1(default_random_engine& gen, vector<int64_t> nlist){
	vector<int64_t> s0 = sgen(gen, nlist);
	for(int i = 0; i < ITER; i++){
		vector<int64_t> s1 = sgen(gen, nlist);
		s0 = (vsumup(s1) < vsumup(s0)) ? s1 : s0;
	}
	return vsumup(s0);
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
	vector<int64_t> s0 = sgen(gen, nlist);
	for(int i = 0; i < ITER; i++){
		vector<int64_t> s1 = s0;
		int a = rand1(gen);
		int b;
		do{b = rand1(gen);}
		while(a == b);
		s1[a] = -s1[a];
		s1[b] = (rand2(gen) > 0) ? s1[b] : -s1[b];
		s0 = (vsumup(s1) < vsumup(s0)) ? s1 : s0;
	}
	return vsumup(s0);
}

// hill climbing under prepartitioning
int64_t hc2(default_random_engine& gen, vector<int64_t> nlist){
	vector<int64_t> p0 = pgen(gen, nlist);
	for(int i = 0; i < ITER; i++){
		vector<int64_t> p1 = p0;
		int a = rand1(gen);
		int b;
		do{b = rand1(gen);}
		while(a == b);
		p1[a] = p1[b];
		p1[b] = (rand2(gen) > 0) ? p1[b] : p1[a];

		p0 = (karkar(p1) < karkar(p0)) ? p1 : p0;
	}
	return karkar(p0);
}

// simulated annealing under standard form
int64_t sa1(default_random_engine& gen, vector<int64_t> nlist){
	vector<int64_t> s0 = sgen(gen, nlist);
	vector<int64_t> s2 = s0;
	for(int i = 0; i < ITER; i++){
		vector<int64_t> s1 = s0;
		int a = rand1(gen);
		int b = rand1(gen);
		s1[a] = (rand2(gen) > 0) ? s1[a] : -s1[a];
		s1[b] = (rand2(gen) > 0) ? s1[b] : -s1[b];
		
		if(vsumup(s1) < vsumup(s0)){
			s0 = s1;
		}
		else if(rand3(gen) < exp((double)-(vsumup(s1) < vsumup(s0)) / 
			((double)10000000000 * (double)pow((double)0.8, (double)i/(double)300)))){
			s0 = s1;
		}
		s2 = (vsumup(s0) < vsumup(s2)) ? s0 : s2;
	}
	return vsumup(s2);
}

// simulated annealing under prepartitioning
int64_t sa2(default_random_engine& gen, vector<int64_t> nlist){
	vector<int64_t> p0 = pgen(gen, nlist);
	vector<int64_t> p2 = p0;
	for(int i = 0; i < ITER; i++){
		vector<int64_t> p1 = p0;
		int a = rand1(gen);
		int b = rand1(gen);
		p1[a] = (rand2(gen) > 0) ? p1[a] : p1[b];
		p1[b] = (rand2(gen) > 0) ? p1[b] : p1[a];
		
		if(karkar(p1) < karkar(p0)){
			p0 = p1;
		}
		else if(rand3(gen) < exp((double)-(karkar(p1) - karkar(p0)) / 
			((double)10000000000 * (double)pow((double)0.8, (double)i/(double)300)))){
			p0 = p1;
		}
		p2 = (karkar(p0) < karkar(p2)) ? p0 : p2;
	}
	
	return karkar(p2);
}


int main(){
	long long seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine gen((unsigned)seed);

    // sum of residues for all trials
    int64_t kk_sum = 0;
    int64_t rr1_sum = 0, rr2_sum = 0;
    int64_t hc1_sum = 0, hc2_sum = 0;
    int64_t sa1_sum = 0, sa2_sum = 0;

/*    clock_t tkk_s, tkk_e;
    clock_t trr1_s, trr1_e, trr2_s, trr2_e;
    clock_t thc1_s, thc1_e, thc2_s, thc2_e;
    clock_t tsa1_s, tsa1_e, tsa2_s, tsa2_e;*/

    // clock variables
    clock_t t0, t1;

    // sum of time in seconds for all trials
    double tkk = 0;
    double trr1 = 0, trr2 = 0;
    double thc1 = 0, thc2 = 0;
    double tsa1 = 0, tsa2 = 0;

    // testing
    for(int trial = 0; trial < 2; trial++){
		vector<int64_t> numlist = num_list_gen(gen);
		
		t0 = clock();
		kk_sum += karkar(numlist);
		t1= clock();
		tkk += double(t1 - t0)/CLOCKS_PER_SEC;

		t0 = clock();
		rr1_sum += rr1(gen, numlist);
		t1= clock();
		trr1 += double(t1 - t0)/CLOCKS_PER_SEC;

		t0 = clock();
		rr2_sum += rr2(gen, numlist);
		t1= clock();
		trr2 += double(t1 - t0)/CLOCKS_PER_SEC;

		t0 = clock();
		hc1_sum += hc1(gen, numlist);
		t1= clock();
		thc1 += double(t1 - t0)/CLOCKS_PER_SEC;

		t0 = clock();
		hc2_sum += hc2(gen, numlist);
		t1= clock();
		thc2 += double(t1 - t0)/CLOCKS_PER_SEC;

		t0 = clock();
		sa1_sum += sa1(gen, numlist);
		t1= clock();
		tsa1 += double(t1 - t0)/CLOCKS_PER_SEC;

		t0 = clock();
		sa2_sum += sa2(gen, numlist);
		t1= clock();
		tsa2 += double(t1 - t0)/CLOCKS_PER_SEC;
    }

    cout << "kk average residue: " << kk_sum / (int64_t) 2 << endl;
    cout << "kk average time taken (s) : " << tkk / (double) 2 << endl << endl;

    cout << "rr1 average residue: " << rr1_sum / (int64_t) 2 << endl;
    cout << "rr1 average time taken (s) : " << trr1 / (double) 2 << endl << endl;

    cout << "rr2 average residue: " << rr2_sum / (int64_t) 2 << endl;
    cout << "rr2 average time taken (s) : " << trr2 / (double) 2 << endl << endl;

    cout << "hc1 average residue: " << hc1_sum / (int64_t) 2 << endl;
    cout << "hc1 average time taken (s) : " << thc1 / (double) 2 << endl << endl;

    cout << "hc2 average residue: " << hc2_sum / (int64_t) 2 << endl;
    cout << "hc2 average time taken (s) : " << thc2 / (double) 2 << endl << endl;

    cout << "sa1 average residue: " << sa1_sum / (int64_t) 2 << endl;
    cout << "sa1 average time taken (s) : " << tsa1 / (double) 2 << endl << endl;

    cout << "sa2 average residue: " << sa2_sum / (int64_t) 2 << endl;
    cout << "sa2 average time taken (s) : " << tsa2 / (double) 2 << endl << endl;

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