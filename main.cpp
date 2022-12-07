#include <iostream>
#include "cmath"
#include <vector>
#include <iostream>
#include <fstream>
#include <bitset>

using namespace std;

struct page{
	int valid;
	int perm;
	int frame;
	string bframe;
	int LRU;
};

struct pageTable{
	int n;
	int m;
	int size;
	int vec_size;
	int pvec_szie;
	vector<page> table;
	void init_sizes(){
		vec_size = exp2(n - log2(size));
		pvec_szie = exp2(m - log2(size));
		table.resize(vec_size);
	}
};

pageTable create_table(char* filename){
	pageTable vtable;
	ifstream inputFile;
	inputFile.open (filename);
	if (inputFile.fail())
	{
		cout << "File " << filename << " not found\n";
		exit (2);
	}
	inputFile >> vtable.n >> vtable.m >> vtable.size;
	vtable.init_sizes();
	for(int i =0; i < vtable.vec_size; i++){
		inputFile >> vtable.table.at(i).valid >> vtable.table.at(i).perm >>
			vtable.table.at(i).frame >> vtable.table.at(i).LRU;
		vtable.table.at(i).bframe = bitset<16>(vtable.table.at(i).frame).to_string();
	}

	return vtable;
}

void translate_address(pageTable tablein){
	string tempinput;
	int temp_int;
	string binary;
	string offset;
	string pnumber;
	int output;
	int temp1;
	int off = 16 - log2(tablein.size);
	while(getline(cin,tempinput)){
		temp_int = stoi(tempinput, nullptr, 0);
		binary = bitset<16>(temp_int).to_string();
		//cout <<binary <<endl;
		offset = binary.substr(off);
		//cout <<offset <<endl;
		pnumber = binary.substr(0,off);
		//cout <<pnumber <<endl;
		int temp1 = stoi(pnumber, nullptr,2);
		//cout << temp1 << endl;
		if(tablein.table.at(temp1).valid){
			output = stoi(tablein.table.at(temp1).bframe + offset, nullptr,2);
			cout << output <<endl;
		}
		else if(tablein.table.at(temp1).perm){
			cout << "Disk" <<endl;
		}
		else{
			cout << "SEGFAULT" <<endl;
		}
	}
}

int main(int argc, char * argv[]) {
	if (argc < 2)
	{
		printf ("not enough arguments");
		exit (1);
	}
	pageTable vtable = create_table(argv[1]);
	translate_address(vtable);


	return 0;
}
