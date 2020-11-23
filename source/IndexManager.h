#ifndef _INDEXMANAGER
#define _INDEXMANAGER

#include<string>
#include<fstream>
#include"DataBase.h"
#include"BTree.h"
#include"BufferManager.h"
using namespace std;


class IndexManager {
private:
	BTree mybtree;						// IndexManager contains a btree
	BufferManager* buffer;
public:
	IndexManager(){}
	~IndexManager(){}
	void CreateIndex(MyTable& table, string attr, string indexname);	// indexname not have ".txt", just name
	void Search(char* value, MyTable& table, string& attr, string indexname, WHERE con, bool set);
	void BuildBTreeFromFile(string indexname);							// indexname not have ".txt", just name
	PtrtoNode GetOneTNode(ifstream& inf, int flag);
	void DeleteAll(string indexname);									// delete all data in this index
	void DropIndex(string indexname);									// delete this index
	void set_Buffer(BufferManager& abuffer) { this->buffer = &abuffer; }
	void InsertBtree(MyTable& table, My_place& data, string indexname);
	void print(char* test_char, Attribute& test_Attri);
	void set0(TupleData& data, MyTable& table);
	string gettablename(string indexname) {
		ifstream inf;
		inf.open(indexname + ".txt");
		string name;
		inf >> name;
		return name;
	}
	bool IsRepeat(MyTable& table, string attr, char* value, string indexname);
};

#endif // !_INDEXMANAGER
