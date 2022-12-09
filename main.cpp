#include <iostream>
#include "cmath"
#include <vector>
#include <iostream>
#include <fstream>
#include <bitset>

using namespace std;
const static int bitsize = 64;

// Holds all the information about a single page
struct page{
	int valid; // statues of valid bit
	int perm; // status of permission bits
	int frame; //frame number
	string bframe; // frame number in binary
	int LRU; // recently used bit
};

// holds all the information about the page table
struct pageTable{
	int n; // number of bits in virtual address
	int m; // number of bits in physical address
	int size; // Size of page in bytes
	int vec_size; // size of virtual address table
	vector<page> table; //vector holding all the pages
	void init_sizes(){ // init vector table after n, m and size are read in
		vec_size = exp2(n - log2(size));
		table.resize(vec_size);
	}
};


struct clockS{
	page* currentPage;
	int position;
};

/*
FUNCTION Prototype: pageTable create_table(char* filename);
INTENT: Reads in a file and creates a page table out of the file
@param char* filename: Name of the file to turn into pagetable
@return: pageTable struct containing the page table
@post: pageTable is returned to the calling function
@pre: a valid filename needs to be obtained
*/
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

/*
FUNCTION Prototype: void translate(string& tempinput, int& off, int& location, int& output, const pageTable& tablein);
INTENT: Helper function to handles the translation of a virtual address into a physical address
@param const string& tempinput: string containing virtual address
@param const int& off: number of offset bits
@param int& location: location of entry in pagetable
@param int& output: physical address version of virtual address
@param const pageTable& tablein: struct holding the page table
@return:  nothing function is void
@post: int& location will contain the location of the virtual address in the pagetable. int& output will now contain the string representing the translation from virtual to physical address
@pre: tablein needs to contain a valid pagetable
*/
void translate(const string& tempinput, const int& off, int& location, int& output, const pageTable& tablein){
	int temp_int;
	string binary;
	string pnumber;
	string offset;

	temp_int = stoi(tempinput, nullptr, 0);
	binary = bitset<bitsize>(temp_int).to_string();
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
	int off = bitsize - log2(tablein.size);
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

/*
FUNCTION Prototype: void translate_address_A(pageTable tablein);
INTENT: Handles the translation of a virtual address into a physical address
doesn‘t use any page replacement algorithm
@param const pageTable& tablein: struct holding the page table
@return:  nothing function is void
@post: nothing
@pre: tablein needs to contain a valid pagetable
*/
void translate_address_A(pageTable tablein){
	string input;
	int output;
	int location;
	int off = bitsize - log2(tablein.size);
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
