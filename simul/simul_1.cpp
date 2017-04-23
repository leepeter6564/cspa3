#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>
#include <random>
#include <cstdlib>
#include <numeric>
#include <cmath>
#include <time.h>
#include <algorithm>
#include <fstream>

#define ITER 25000

using namespace std;

// pick a number
uniform_int_distribution<int64_t> rando(1, 1000000000000);
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
int64_t vsumup(vector<int64_t>& nlist){
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
vector<int64_t> sgen(default_random_engine& gen, vector<int64_t>& nlist){
	vector<int> s;
	for(int i = 0; i < 100; i++){
		int io = rand2(gen);
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
	vector<int64_t> partition (100);
	for(int i = 0; i < 100; i++){
		partition[i] = rand1(gen);
	}
	return partition;
}

// second produce partitioned vector
vector<int64_t> partition(vector<int64_t> prepart, vector<int64_t>& nlist){
	vector<int64_t> nlist1(100, 0);
	for(int j = 0; j < 100; j++){
		nlist1[prepart[j]] += nlist[j];
	}
	return nlist1;
}

// repeated random under standard form
int64_t rr1(default_random_engine& gen, vector<int64_t>& nlist){
	vector<int64_t> s0 = sgen(gen, nlist);
	for(int i = 0; i < ITER; i++){
		vector<int64_t> s1 = sgen(gen, nlist);
		s0 = (vsumup(s1) < vsumup(s0)) ? s1 : s0;
	}
	return vsumup(s0);
}

// repeated random under prepartitioning
int64_t rr2(default_random_engine& gen, vector<int64_t>& nlist){
	vector<int64_t> ppart0 = prepart(gen);
	int64_t val0 = karkar(partition(ppart0, nlist));
	for(int i = 0; i < ITER; i++){
		vector<int64_t> ppart1 = prepart(gen);
		int64_t val1 = karkar(partition(ppart1, nlist));
		if(val1 < val0){
			ppart0 = ppart1;
			val0 = val1;
		}
	}
	return val0;
}

// hill climbing under standard form
int64_t hc1(default_random_engine& gen, vector<int64_t>& nlist){
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
int64_t hc2(default_random_engine& gen, vector<int64_t>& nlist){
	vector<int64_t> ppart0 = prepart(gen);
	int64_t val0 = karkar(partition(ppart0, nlist));
	for(int i = 0; i < ITER; i++){
		vector<int64_t> ppart1 = ppart0;
		int a = rand1(gen);
		int b;
		do{b = rand1(gen);}
		while(a == b);

		ppart1[a] = ppart1[b];
		ppart1[b] = (rand2(gen) > 0) ? ppart1[b] : ppart1[a];

		int64_t val1 = karkar(partition(ppart1, nlist));

		if(val1 < val0){
			ppart0 = ppart1;
			val0 = val1;
		}
	}
	return val0;
}

// simulated annealing under standard form
int64_t sa1(default_random_engine& gen, vector<int64_t>& nlist){
	vector<int64_t> s0 = sgen(gen, nlist);
	vector<int64_t> s2 = s0;
	for(int i = 0; i < ITER; i++){
		vector<int64_t> s1 = s0;
		int a = rand1(gen);
		int b;
		do{b = rand1(gen);}
		while(a == b);

		s1[a] = -s1[a];
		s1[b] = (rand2(gen) > 0) ? s1[b] : -s1[b];

		int64_t val1 = vsumup(s1);
		int64_t val0 = vsumup(s0);

		if(val1 < val0){
			s0 = s1;
		}
		else if(rand3(gen) > exp((double)(val1 - val0) / 
			((double)10000000000 * (double)pow((double)0.8, (double)i/(double)300)))){
			s0 = s1;
		}
		s2 = (vsumup(s0) < vsumup(s2)) ? s0 : s2;
	}
	return vsumup(s2);
}

// simulated annealing under prepartitioning
int64_t sa2(default_random_engine& gen, vector<int64_t>& nlist){
	vector<int64_t> ppart0 = prepart(gen);
	vector<int64_t> ppart2 = ppart0;
	int64_t val2 = karkar(partition(ppart2, nlist));
	for(int i = 0; i < ITER; i++){
		vector<int64_t> ppart1 = ppart0;
		int a = rand1(gen);
		int b;
		do{b = rand1(gen);}
		while(ppart1[a] == b);

		ppart1[a] = b;
		int64_t val1 = karkar(partition(ppart1, nlist));
		int64_t val0 = karkar(partition(ppart0, nlist));
		
		if(val1 < val0){
			ppart0 = ppart1;
			val0 = val1;
		}
		else if(rand3(gen) > exp((double)(val1 - val0) / ((double)10000000000 *
			pow((double)0.8, (double)i/(double)300)))){
			ppart0 = ppart1;
			val0 = val1;
		}
		if(val0 < val2){
			ppart2 = ppart0;
			val2 = val0;
		}
	}
	return val2;
}


int main(){
	long long seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine gen((unsigned)seed);


    int64_t akk = 0;
    int64_t arr1 = 0, arr2 = 0;
    int64_t ahc1 = 0, ahc2 = 0;
    int64_t asa1 = 0, asa2 = 0;

    // clock variables
    clock_t t0, t1;

    fstream file;
    file.open("test.txt");
    if(!file.is_open()){
    	cout << "File not open" << endl;
    	return 0;
    }

    file << "kk res, kk time, rr1 res, rr1 time, rr2 res, rr2 time, ";
    file << "hc1 res, hc1 time, hc2 res, hc2 time, sa1 res, sa1 time, sa2 res, sa2 time";
    file << endl; 

    // testing
    for(int trial = 0; trial < 100; trial++){
		vector<int64_t> numlist = num_list_gen(gen);
		
		t0 = clock();
		akk = karkar(numlist);
		t1= clock();
		file << akk << ", ";
		file << double(t1 - t0)/CLOCKS_PER_SEC << ", ";

		t0 = clock();
		arr1 = rr1(gen, numlist);
		t1= clock();
		file << arr1 << ", ";
		file << double(t1 - t0)/CLOCKS_PER_SEC << ", ";

		t0 = clock();
		arr2 = rr2(gen, numlist);
		t1= clock();
		file << arr2 << ", ";
		file << double(t1 - t0)/CLOCKS_PER_SEC << ", ";

		t0 = clock();
		ahc1 = hc1(gen, numlist);
		t1= clock();
		file << ahc1 << ", ";
		file << double(t1 - t0)/CLOCKS_PER_SEC << ", ";

		t0 = clock();
		ahc2 = hc2(gen, numlist);
		t1= clock();
		file << ahc2 << ", ";
		file << double(t1 - t0)/CLOCKS_PER_SEC << ", ";

		t0 = clock();
		asa1 = sa1(gen, numlist);
		t1= clock();
		file << asa1 << ", ";
		file << double(t1 - t0)/CLOCKS_PER_SEC << ", ";

		t0 = clock();
		asa2 = sa2(gen, numlist);
		t1= clock();
		file << asa2 << ", ";
		file << double(t1 - t0)/CLOCKS_PER_SEC << endl;

		cout << "Trial " << trial << " done!" << endl;
    }

    file.close();

    return 0;
}