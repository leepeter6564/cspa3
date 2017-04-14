#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

int64_t kar_kar(vector<int64_t> list){
	while(list.size() > 1){
		int64_t big1 = *max_element(begin(list), end(list));
		list.erase(max_element(begin(list), end(list)));
		int64_t big2 = *max_element(begin(list), end(list));
		list.erase(max_element(begin(list), end(list)));
		int64_t diff = abs(big1-big2);
		list.push_back(diff);
	}
	return list[0];
}

int main(int argc, char* argv[]){

	string fname = argv[1];
	vector<int64_t> num_list;

	string line;
    ifstream file(fname);
    int64_t cur_num;
    int num_count = 0;

    while (file >> cur_num && num_count<100) {
        num_list.push_back(cur_num);
        num_count++;
    }
    file.close();

	int64_t ans = kar_kar(num_list);
	cout << ans << endl;

	return 0;
}