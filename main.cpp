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

struct clockS{
	page* currentPage;
	int position;
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

void translate(string& tempinput, int& off, int& location, int& output, const pageTable& tablein){
	int temp_int;
	string binary;
	string pnumber;
	string offset;

	temp_int = stoi(tempinput, nullptr, 0);
	binary = bitset<16>(temp_int).to_string();
	offset = binary.substr(off);
	pnumber = binary.substr(0,off);
	location = stoi(pnumber, nullptr,2);
	output = stoi(tablein.table.at(location).bframe + offset, nullptr,2);
}

void translate_address_B(pageTable tablein){
	clockS clock;
	clock.currentPage = &tablein.table[0];
	clock.position = 0;
	string tempinput;
	string offset;
	int output;
	int location;
	int off = 16 - log2(tablein.size);
	while(getline(cin,tempinput)){
		translate(tempinput, off, location, output, tablein);
		if(tablein.table.at(location).valid){
			tablein.table.at(location).LRU = 1;
			cout << output <<endl;
		}
		else if(tablein.table.at(location).perm){
			cout << "Page Fault ";
			while(tablein.table.at(location).valid == 0){
				if(clock.currentPage->valid == 0){
					clock.position++;
					if(clock.position == tablein.table.size())
						clock.position = 0;
					clock.currentPage = &tablein.table[clock.position];
				}
				else if(clock.currentPage->LRU == 1){
					clock.currentPage->LRU = 0;
					clock.position++;
					if(clock.position == tablein.table.size())
						clock.position = 0;
					clock.currentPage = &tablein.table[clock.position];
				}
				else{
					clock.currentPage->valid = 0;
					tablein.table.at(location).valid = 1;
					tablein.table.at(location).frame = clock.currentPage->frame;
					tablein.table.at(location).bframe = clock.currentPage->bframe;
					tablein.table.at(location).LRU = 1;
					clock.position++;
					if(clock.position == tablein.table.size())
						clock.position = 0;
					clock.currentPage = &tablein.table[clock.position];
				}
			}
			translate(tempinput, off, location, output, tablein);
			cout << output <<endl;
		}
		else{
			cout << "SEGFAULT" <<endl;
		}
	}
}

void translate_address_A(pageTable tablein){
	string input;
	int output;
	int location;
	int off = 16 - log2(tablein.size);
	while(getline(cin,input)){
		translate(input, off, location, output, tablein);
		if(tablein.table.at(location).valid){
			cout << output <<endl;
		}
		else if(tablein.table.at(location).perm){
			cout << "Disk" <<endl;
		}
		else{
			cout << "SEGFAULT" <<endl;
		}
	}
}

int main(int argc, char * argv[]) {
	if (argc < 3)
	{
		printf ("not enough arguments");
		exit (1);
	}
	pageTable vtable = create_table(argv[1]);
	if(*argv[2] == 'A'){
		translate_address_A(vtable);
	}
	else if(*argv[2] == 'B'){
		translate_address_B(vtable);
	}
	else{
		cerr << "please enter 'A' or 'B' as second parameter to specify mode."<<endl;
		exit(1);
	}
	return 0;
}
