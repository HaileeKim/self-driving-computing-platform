#include <iostream>
#include <fstream>

using namespace std;
using std::ofstream;

int main(void){

	ofstream fout;
	
	fout.open("경로ek");

	fout << "내용" << std::endl;

	return 0;
}
