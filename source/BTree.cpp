#include"BTree.h"
#include"DataBase.h"
#include<fstream>
#include<list>
using namespace std;

void BTree::Insert(TupleData& data) {			// B+Tree insert
	int k = this->k;							// define max value sum
	PtrtoNode TNode;
	if (Head == nullptr) {						// if empty BTree
		Head = new BTreeNode;
		TNode = new BTreeNode;
		Head->ptr.emplace_back(TNode);			// the first data
		Head->ptrsum = 1; Head->datasum = 0;	// set Head
		Head->next = nullptr; Head->pre = nullptr; Head->parent = nullptr;
		TNode->data.emplace_back(data);			// set TNode
		TNode->parent = Head; TNode->next = nullptr; TNode->pre = nullptr;
		TNode->datasum = 1; TNode->ptrsum = 0;
		return;
	}
	TNode = Find(data);							// get corresponding leaf node ptr
	PtrtoNode ParentCheckNode;
	if (TNode->datasum < k) {					// still empty
		InsertintoTNode(data, TNode);			// insert into this TNode directly
	}
	else if((ParentCheckNode = ParentEmpty(TNode)) != nullptr){		// parent node is null
		InsertintoTNode(data, TNode);			// we first insert data into this TNode
		do{										// internal TNode
			Split(TNode);
			TNode = TNode->parent;
		} while (TNode != ParentCheckNode);
	}
	else {										// split root node
		InsertintoTNode(data, TNode);			// we first insert data into this TNode
		do {									// internal TNode split
			Split(TNode);
			TNode = TNode->parent;
		} while (TNode->parent != nullptr);
		
		PtrtoNode newHead=new BTreeNode;		// create newHead to emplace old Head
		newHead->ptr.emplace_back(Head);
		newHead->datasum = 0; newHead->ptrsum = 1;
		newHead->next = nullptr; newHead->pre = nullptr; newHead->parent = nullptr;
		Head->parent = newHead;
		Head = newHead;
	}
}
void BTree::InsertintoTNode(TupleData& data, PtrtoNode tmp)
{
	list<TupleData>::iterator it = tmp->data.begin();
	while (it != tmp->data.end() && *it < data) {
		// find correct location
		it++;
	}
	if (it != tmp -> data.end() && *it == data) {							// there exist this data, which has been deleted
		TupleData& DelData = *it;
		if (DelData.isdelete) {
			DelData.isdelete = false;			// reinsert
		}
		else { cout << "Repeat key value" << endl; }
	}
	else {
		tmp->data.insert(it, data);
		tmp->datasum++;
	}
}
TupleData BTree::Delete(TupleData& data) {					// B+Tree delete
	PtrtoNode TNode = Find(data);
	list<TupleData>::iterator datait;
	for (datait = TNode->data.begin(); datait != TNode->data.end(); datait++) {
		if (*datait == data) {							// find object data
			TupleData& tmp = *datait;
			if (tmp.isdelete == false) {
				tmp.isdelete = true;
				cout << "Find and delete!" << endl;
			}
			else {
				cout << "This Node has been deleted" << endl;
			}
			return tmp;
		}
	}
	cout << "Not Find, delete failed." << endl;
	return *TNode->data.begin();
}
PtrtoNode BTree::Find(TupleData& data) {				// B+Tree find certain data
	if (Head == nullptr) {
		cout << "Empty Head no need BTree::Find" << endl;
		return nullptr;
	}
	list<PtrtoNode>::iterator nodeit = Head->ptr.begin();
	list<TupleData>::iterator datait;
	PtrtoNode TNode = *nodeit;
	int i=0;
	while (TNode->ptr.size() != 0) {					// while TNode is not leaf node
		for (datait = TNode->data.begin(),i=0; i<TNode->datasum; datait++,i++) {		
			if (*datait > data) {	
				break;
			}				// compare data and get corresponding iterator location for next ptr
		}
		nodeit = TNode->ptr.begin();
		while (i > 0) {
			nodeit++;
			i--;
		}
		TNode = *nodeit;
	}
	return TNode;
}
PtrtoNode BTree::ParentEmpty(PtrtoNode tmp)			// check whether there is a parent node is not full
{
	PtrtoNode TNode=tmp->parent;
	while (TNode != this->Head) {
		if (TNode->datasum < k) {						// TNode datasum < k
			return TNode;
		}
		else {
			TNode = TNode->parent;
		}
	}
	return nullptr;
}
void BTree::Split(PtrtoNode tmp)
{
	PtrtoNode ANode, BNode;
	ANode = new BTreeNode; BNode = new BTreeNode;
	ANode->parent = tmp->parent; BNode->parent = tmp->parent;
	ANode->datasum = (tmp->datasum) / 2; BNode->datasum = tmp->datasum - ANode->datasum;
	if (!tmp->ptr.empty()) {
		ANode->ptrsum = ANode->datasum + 1; BNode->ptrsum = tmp->ptrsum - ANode->ptrsum;
	}
	else {
		ANode->ptrsum = 0; BNode->ptrsum = 0;
	}
	ANode->next = BNode; ANode->pre = tmp->pre;
	BNode->next = tmp->next; BNode->pre = ANode;
	if (tmp->pre != nullptr) tmp->pre->next = ANode;
	if (tmp->next != nullptr) tmp->next->pre = BNode;

	// copy data
	list<TupleData>::iterator tmpit;
	tmpit = tmp->data.begin();
	for (int i = 0; i < ANode->datasum; i++) {
		ANode->data.emplace_back(*tmpit);
		tmpit++;
	}
	for (int i = 0; i < BNode->datasum; i++) {
		BNode->data.emplace_back(*tmpit);
		tmpit++;
	}
	// copy ptr
	list<PtrtoNode>::iterator tmpptrit;
	PtrtoNode tmpptr;
	tmpptrit = tmp->ptr.begin();
	for (int i = 0; i < ANode->ptrsum; i++) {
		tmpptr = *tmpptrit;
		ANode->ptr.emplace_back(*tmpptrit);
		tmpptr->parent = ANode;
		tmpptrit++;
	}
	for (int i = 0; i < BNode->ptrsum; i++) {
		tmpptr = *tmpptrit;
		BNode->ptr.emplace_back(*tmpptrit);
		tmpptr->parent = ANode;
		tmpptrit++;
	}

	// change tmp parent ptr to new TNode and create new ptr to BNode
	list<PtrtoNode>::iterator ptrit = tmp->parent->ptr.begin();
	list<TupleData>::iterator datait = tmp->parent->data.begin();
	while (ptrit != tmp->parent->ptr.end()) {
		if (*ptrit == tmp) break;
		ptrit++;
		datait++;
		if (datait==tmp->parent->data.end()) {
			break;
		}
	}
	tmp->parent->data.insert(datait, *(BNode->data.begin()));						// data
	tmp->parent->datasum++;
	tmp->parent->ptr.erase(ptrit++);								// ptr
	tmp->parent->ptr.insert(ptrit, ANode); 
	tmp->parent->ptr.insert(ptrit, BNode);
	tmp->parent->ptrsum++;

	if (!tmp->ptr.empty())											// This is not leaf node, and we pop first data
	{
		BNode->data.pop_front();									// delete first redundant node
		BNode->datasum--;
	}
}
void BTree::OutputtoFile(string indexname)
{
	if (indexname.empty()) {
		return;
	}
	ofstream outf;
	indexname += ".txt";
	outf.open(indexname);
	outf << table << endl;											// first line: string table name
	outf << attribute<<" "<<flag << endl;							// second line: string attribute name & flag
	outf << k << endl;												// third line: k (max value sum in a BTreeNode)
	outf << isempty << endl;
	if (isempty == 1 || Head == nullptr) {
		cout << "This is an empty B+Tree" << endl;
		return;
	}
	PtrtoNode TNode;
	TNode = *(Head->ptr.begin());
	while (1) {
		TNode->OutputBTreeNode(outf);
		if(TNode->next != nullptr) {								// we first output the BTreeNode in the same Level
			TNode = TNode->next;
			continue;
		}
		while (TNode->pre != nullptr) TNode = TNode->pre;			// this level is OK, go back to this level's first BTreeNode
		if(!TNode->ptr.empty()){									// then output the Next Level
			outf << endl;											// new Line means next level
			TNode = *TNode->ptr.begin();
		}
		else {
			break;													// if This Level is LeafNode, we break the loop	
		}
	}
}
void BTreeNode::OutputBTreeNode(ofstream& outf)
{
	int flag = (*data.begin()).flag;								// get attributr flag
	outf << datasum << " " << ptrsum << " ";
	list<TupleData>::iterator it = data.begin();
	TupleData onedata;
	for (int i = 0; i < datasum; i++,it++) {
		onedata = *it;
		if (flag == -2||flag==-1) {											// output int
			for (int i = 0; i < 4; i++) outf << onedata.tuple_data[i];
		}
		else {
			for (int i = 0; i < flag; i++) outf << onedata.tuple_data[i];
		}
		outf << " " << onedata.blocknum;
		outf << " " << onedata.tuplenum;
		if (onedata.isdelete) outf << "*";							// if the data is deleted, we output a * as following
		outf << " ";												// each onedata end, we need a SPACE
	}
}
