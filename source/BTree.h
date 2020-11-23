#ifndef _BTree
#define _BTree

#include<string>
#include<list>
#include"DataBase.h"
#include<fstream>
using namespace std;

struct TupleData {
	char tuple_data[20];					// store data
	int blocknum;						// the blockoffset of this tuple
	int tuplenum;						// the tuplenum in this block
	int flag;
	bool isdelete = false;
	bool operator < (const TupleData& cmp) {
		if (flag == -2) {
			unsigned int a, b;
			a = (unsigned char)tuple_data[0];			a *= 256;
			a += (unsigned char)tuple_data[1];			a *= 256;
			a += (unsigned char)tuple_data[2];			a *= 256;
			a += (unsigned char)tuple_data[3];
			b = (unsigned char)cmp.tuple_data[0];			b *= 256;
			b += (unsigned char)cmp.tuple_data[1];			b *= 256;
			b += (unsigned char)cmp.tuple_data[2];			b *= 256;
			b += (unsigned char)cmp.tuple_data[3];
			return a < b;
		}
		else if (flag == -1) {
			float *a, *b;
			char data1[4]; data1[0] = tuple_data[3]; data1[1] = tuple_data[2]; data1[2] = tuple_data[1]; data1[3] = tuple_data[0];
			char data2[4]; data2[0] = cmp.tuple_data[3]; data2[1] = cmp.tuple_data[2]; data2[2] = cmp.tuple_data[1]; data2[3] = cmp.tuple_data[0];
			a = reinterpret_cast<float*> (data1);
			b = reinterpret_cast<float*> (data2);
			return *a < *b;
		}
		else {
			string ch1, ch2;
			for (int i = 0; i < flag; i++) {
				ch1.push_back(tuple_data[i]);
				ch2.push_back(cmp.tuple_data[i]);
			}
			return ch1 < ch2;
		}
	}
	bool operator == (const TupleData& cmp) {
		if (flag == -2) {
			unsigned int a, b;
			a = (unsigned char)tuple_data[0];			a *= 256;
			a += (unsigned char)tuple_data[1];			a *= 256;
			a += (unsigned char)tuple_data[2];			a *= 256;
			a += (unsigned char)tuple_data[3];
			b = (unsigned char)cmp.tuple_data[0];			b *= 256;
			b += (unsigned char)cmp.tuple_data[1];			b *= 256;
			b += (unsigned char)cmp.tuple_data[2];			b *= 256;
			b += (unsigned char)cmp.tuple_data[3];
			return a == b;
		}
		else if (flag == -1) {
			float *a, *b;
			char data1[4]; data1[0] = tuple_data[3]; data1[1] = tuple_data[2]; data1[2] = tuple_data[1]; data1[3] = tuple_data[0];
			char data2[4]; data2[0] = cmp.tuple_data[3]; data2[1] = cmp.tuple_data[2]; data2[2] = cmp.tuple_data[1]; data2[3] = cmp.tuple_data[0];
			a = reinterpret_cast<float*> (data1);
			b = reinterpret_cast<float*> (data2);
			return *a == *b;
		}
		else {
			string ch1, ch2;
			for (int i = 0; i < flag; i++) {
				ch1.push_back(tuple_data[i]);
				ch2.push_back(cmp.tuple_data[i]);
			}
			return ch1 == ch2;
		}
	}
	bool operator > (const TupleData& cmp) {
		if (flag == -2) {
			unsigned int a, b;
			a = (unsigned char)tuple_data[0];			a *= 256;
			a += (unsigned char)tuple_data[1];			a *= 256;
			a += (unsigned char)tuple_data[2];			a *= 256;
			a += (unsigned char)tuple_data[3];
			b = (unsigned char)cmp.tuple_data[0];			b *= 256;
			b += (unsigned char)cmp.tuple_data[1];			b *= 256;
			b += (unsigned char)cmp.tuple_data[2];			b *= 256;
			b += (unsigned char)cmp.tuple_data[3];
			return a > b;
		}
		else if (flag == -1) {
			float *a, *b;
			char data1[4]; data1[0] = tuple_data[3]; data1[1] = tuple_data[2]; data1[2] = tuple_data[1]; data1[3] = tuple_data[0];
			char data2[4]; data2[0] = cmp.tuple_data[3]; data2[1] = cmp.tuple_data[2]; data2[2] = cmp.tuple_data[1]; data2[3] = cmp.tuple_data[0];
			a = reinterpret_cast<float*> (data1);
			b = reinterpret_cast<float*> (data2);
			return *a > *b;
		}
		else {
			string ch1, ch2;
			for (int i = 0; i < flag; i++) {
				ch1.push_back(tuple_data[i]);
				ch2.push_back(cmp.tuple_data[i]);
			}
			return ch1 > ch2;
		}
	}
};

typedef struct BTreeNode* PtrtoNode;
struct BTreeNode {
	list<TupleData> data;				// store data
	list<PtrtoNode> ptr;				// store children ptr
	int ptrsum = 0, datasum = 0;		// value sum in each BTreeNode
	PtrtoNode pre, next, parent;		// ptrs
	void OutputBTreeNode(ofstream& outf);
};
          
class BTree {
private:
	int k;								// define max values sum in each node
	int flag;							// tuple data type
	string table;						// on which table
	string attribute;					// on which attribute
	string indexname;
	PtrtoNode Head;						// head node is just a ptr
public:
	int isempty = 1;
	BTree() {}							// default ctor
	BTree(string table, string attribute, string indexname, int flag, int k = 20) :table(table), attribute(attribute)
		, indexname(indexname), k(k), flag(flag) {
		Head = nullptr;
	}
	~BTree() {
		this->OutputtoFile(this->indexname);
	}
	void Insert(TupleData& data);				// B+Tree insert
	void InsertintoTNode(TupleData& data, PtrtoNode tmp);
	// inset into correct location in PtrtoNode tmp
	TupleData Delete(TupleData& data);				// B+Tree delete
	PtrtoNode Find(TupleData& data);				// B+Tree find certain data
	PtrtoNode ParentEmpty(PtrtoNode tmp);		// check whether there is a parent node is not full and return PtrtoNode
	void Split(PtrtoNode tmp);					// split TNode to two TNode;
	void OutputtoFile(string indexname);

	string Gettablename() { return this->table; }
	PtrtoNode GetHead() { return this->Head; }
	void SetHead(PtrtoNode Head) { this->Head = Head; }
	string Getattr() { return attribute; }
	string Getindexname() { return indexname; }
	int Getflag() { return flag; }
	int GetK() { return k; }
	void Settable(string table) { this->table = table; }
	void Setattr(string attr) { this->attribute = attr; }
	void Setk(int k) { this->k = k; }
	void Setflag(int flag) { this->flag = flag; }
};

#endif // !_BTree

