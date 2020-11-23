#include<iostream>
#include"Intepreter.h"

using namespace std;
int main() {
	Intepreter the_Intepreter;
	BufferManager the_Buffer_Manager;
	the_Intepreter.set_buffer(the_Buffer_Manager);
	string start = "";
	
	while (1) {
		start = "";
		the_Intepreter.Run();
		cout << endl;
		cout << "input exit(end program) else continue" << endl;
		getline(cin,start);
		if (start == "exit") break;
	}
	/*
	unsigned int  n = 133;
	unsigned char p[4] = {};

	p[0] = (n >> 24) & 0xFF;
	p[1] = (n >> 16) & 0xFF;
	p[2] = (n >> 8) & 0xFF;
	p[3] = n & 0xFF;
	//p[4] = '\0';

	int pp=0;
	pp += p[0];
	pp << 8;
	pp += p[1];
	pp << 8;;
	pp += p[2];
	pp << 8;
	pp += p[3];
	cout << pp  ;
	cin >> pp;*/
}
