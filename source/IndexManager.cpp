#include"IndexManager.h"
#include"BTree.h"
#include"DataBase.h"
#include"BufferManager.h"
#include<string>
#include<fstream>
#include <iomanip>
using namespace std;

void IndexManager::CreateIndex(MyTable& table, string attr, string indexname)
{
	int flag;												// tuple attribute data type
	int tuplesize = table.OneTuperSize()+1;
	int offset=1;											// offset before attribute
	for (int i = 0; i < table.attributes.AttrNum; i++) {	// find correct attribute
		if (table.attributes.AttributeName[i] == attr) {
			flag = table.attributes.Flag[i];
			break;
		}
		else {												// get correct attribute offset
			if (table.attributes.Flag[i] == -2|| table.attributes.Flag[i] == -1) offset += 4;
			else offset += table.attributes.Flag[i];
		}
	}
	mybtree.~BTree();
	mybtree = BTree(table.GetTableName(), attr, indexname, flag);

	TupleData onedata;
	char checkbit;
	//int tupleID = 0;										// define tuple ID
	int blockoffset = 0;									// blockoffset in each block
	int datasize;											// object attribute data size
		if (flag == -2 || flag == -1) datasize = 4;
		else datasize = flag;								// not +1 because we don't save '\0'
	for (int i = 0; i < table.BlockNum; i++) {
		int buffernum = buffer->GetBufferNum(table.GetTableName()+".table", i);	/* ??? */
		blockoffset = 0;												/* ??? */
		
		while (blockoffset + tuplesize < 4096) {			// tranverse this block
			// get one tuple data
			onedata.blocknum = buffernum;
			//onedata.tuplenum = tupleID;
			onedata.tuplenum = blockoffset;
			checkbit = buffer->buffer[buffernum].GetDesignatedBlockContent(blockoffset);
			if (checkbit == '\0') {
				blockoffset += tuplesize;
				continue;
			}
			for (int i = 0; i < datasize; i++) {
				onedata.tuple_data[i] = buffer->buffer[buffernum].BlockContent[blockoffset + offset + i];
			}													// we just get the attribute data we want
			onedata.flag = flag;
			
			// one tuple data processing
			mybtree.Insert(onedata);
			//tupleID++;
			blockoffset += tuplesize;
		}
	}

	mybtree.OutputtoFile(indexname);							// write B+index to file
}
void IndexManager::BuildBTreeFromFile(string indexname)
{
	ifstream inf;
	inf.open(indexname+".txt");
	int flag, k, i;
	string table, attribute;
	inf >> table >> attribute >> flag >> k;
	mybtree.~BTree();
	mybtree.Settable(table);
	mybtree.Setattr(attribute);
	mybtree.Setflag(flag);
	mybtree.Setk(k);										// create new B+Tree
	int isempty;
	inf >> isempty;
	if (isempty == 1) {
		mybtree.isempty = 1;
		return;
	}
	else {
		mybtree.isempty = 0;
	}
	PtrtoNode TNode;											// processing Head Node and set
	TNode = new BTreeNode;
	TNode->ptrsum = 1;
	TNode->next = nullptr; TNode->parent = nullptr; TNode->pre = nullptr;
	mybtree.SetHead(TNode);										// we set TNode to Head
	
	PtrtoNode oneNode;
	PtrtoNode preNode = nullptr;
	list<PtrtoNode>::iterator ptrit;
	while (TNode->ptrsum!=0){									// if this level is not leafnode 
		for (i = 0; i < TNode->ptrsum; i++){		// in this BTreeNode
			oneNode = GetOneTNode(inf, flag);					// first we get a new node
			oneNode->parent = TNode;								// parent

			if (i == 0) {
				oneNode->pre = nullptr;							// pre and next
				oneNode->next = nullptr;
				if (preNode != nullptr) preNode->next = oneNode;	// same level not same parent
				TNode->ptr.emplace_back(oneNode);
				ptrit = TNode->ptr.begin();
				preNode = *ptrit;								// now we update preNode
				ptrit++;
			}
			else {
				oneNode->pre = preNode;
				oneNode->next = nullptr;
				TNode->ptr.emplace_back(oneNode);
				preNode->next = oneNode;
				preNode = *ptrit;
				ptrit++;
			}
		}
		if (TNode->next != nullptr) {							// this level is not done
			TNode = TNode->next;								// we first do this level
		}
		else{
			while (TNode->pre != nullptr) TNode = TNode->pre;	// else mean this level is done and have next level
			TNode = *(TNode->ptr.begin());
			preNode = nullptr;
		}
	}
}
PtrtoNode IndexManager::GetOneTNode(ifstream& inf, int flag)
{
	PtrtoNode TNode=new BTreeNode;
	TupleData onedata;
	inf >> TNode->datasum >> TNode->ptrsum;
	/*char ch;
	inf >> ch;
	cout << "ch:" << ch << endl;*/
	
	for (int i = 0; i < TNode->datasum; i++) {
		if (flag == -2) {
			inf >> onedata.tuple_data[0] >> onedata.tuple_data[1] >> onedata.tuple_data[2] >> onedata.tuple_data[3];
		}
		else if (flag == -1) {
			inf >> onedata.tuple_data[0] >> onedata.tuple_data[1] >> onedata.tuple_data[2] >> onedata.tuple_data[3];
		}
		else {
			for (int i = 0; i < flag; i++) {
				inf >> onedata.tuple_data[i];
			}
		}
		inf >> onedata.blocknum >> onedata.tuplenum;
		onedata.flag = flag; onedata.isdelete = false;
		char ch;
		ch = inf.get();										// check delete
		if (ch == '*') {
			onedata.isdelete = true;
		}

		// end of processing one data
		TNode->data.emplace_back(onedata);
	}
	return TNode;
}
void IndexManager::Search(char* value, MyTable& table, string& attr, string indexname, WHERE con, bool set) {
	TupleData searchdata, nowdata;
	int flag;
	for (int i = 0; i < table.attributes.AttrNum; i++) {	// find correct attribute
		if (table.attributes.AttributeName[i] == attr) {
			flag = table.attributes.Flag[i];
			break;
		}
	}
	int datasize;
		if (flag == -2 || flag == -1) datasize = 4;
		else datasize = flag;

	for (int i = 0; i < datasize; i++) {
		searchdata.tuple_data[i] = value[i];
	}

	if (mybtree.Gettablename() != table.GetTableName() || mybtree.Getattr() != attr) {
		BuildBTreeFromFile(indexname);
	}

	PtrtoNode TNode = mybtree.Find(searchdata);
	PtrtoNode conNode = TNode;
	PtrtoNode firstNode;
	Attribute attribute = table.GetAttribute();
	char content[256];
	list<TupleData>::iterator it = conNode->data.begin();
	switch (con) {
	case 0: {					// == 
		while (it != conNode->data.end()) {
			if (*it == searchdata) {
				nowdata = *it;
				if (nowdata.isdelete == false) {
					if (set) set0(nowdata, table);
					else {
						int buffernum = buffer->GetBufferNum(table.TableName+".table",nowdata.blocknum);
						print(buffer->buffer[buffernum].BlockContent + nowdata.tuplenum+1 , attribute);
					}
					break;
				}
			}
			it++;
				
		}
		break;
	}
	case 1: {					// <=
		while (TNode->pre != nullptr) TNode = TNode->pre; firstNode = TNode;
		while (TNode != conNode) {
			it = TNode->data.begin();
			while (it != TNode->data.end()) {
				nowdata = *it;
				if (nowdata.isdelete == false) {
					if (set) set0(nowdata, table);
					else {
						int buffernum = buffer->GetBufferNum(table.TableName + ".table", nowdata.blocknum);
						print(buffer->buffer[buffernum].BlockContent + nowdata.tuplenum +1 , attribute);
					}
				}
				it++;
			}
			TNode = TNode->next;
		}
		it = conNode->data.begin();
		while (it != conNode->data.end()) {
			if (*it > searchdata) {
				break;
			}
			nowdata = *it;
			if (nowdata.isdelete == false) {
				if (set) set0(nowdata, table);
				else {
					int buffernum = buffer->GetBufferNum(table.TableName + ".table", nowdata.blocknum);
					print(buffer->buffer[buffernum].BlockContent + nowdata.tuplenum +1 , attribute);
				}
			}
			it++;
		}
		break;
	}
	case 2: {					// <
		while (TNode->pre != nullptr) TNode = TNode->pre; firstNode = TNode;
		while (TNode != conNode) {
			it = TNode->data.begin();
			while (it != TNode->data.end()) {
				nowdata = *it;
				if (nowdata.isdelete == false) {
					if (set) set0(nowdata, table);
					else {
						int buffernum = buffer->GetBufferNum(table.TableName + ".table", nowdata.blocknum);
						print(buffer->buffer[buffernum].BlockContent + nowdata.tuplenum +1 , attribute);
					}
				}
				it++;
			}
			TNode = TNode->next;
		}
		it = conNode->data.begin();
		while (it != conNode->data.end()) {
			if (*it > searchdata || *it == searchdata) {
				break;
			}
			nowdata = *it;
			if (nowdata.isdelete == false) {
				if (set) set0(nowdata, table);
				else {
					int buffernum = buffer->GetBufferNum(table.TableName + ".table", nowdata.blocknum);
					print(buffer->buffer[buffernum].BlockContent + nowdata.tuplenum +1, attribute);
				}
			}
			it++;
		}
		break;
	}
	case 3: {					// >=
		it = conNode->data.begin();
		while (it != conNode->data.end()) {
			if (*it > searchdata || *it == searchdata) {
				nowdata = *it;
				if (nowdata.isdelete == false) {
					if (set) set0(nowdata, table);
					else {
						int buffernum = buffer->GetBufferNum(table.TableName + ".table", nowdata.blocknum);
						print(buffer->buffer[buffernum].BlockContent + nowdata.tuplenum+ 1, attribute);
					}
				}
			}
			it++;
		}
		TNode = conNode->next;
		while (TNode != nullptr) {
			it = TNode->data.begin();
			while (it != TNode->data.end()) {
				nowdata = *it;
				if (nowdata.isdelete == false) {
					if (set) set0(nowdata, table);
					else {
						int buffernum = buffer->GetBufferNum(table.TableName + ".table", nowdata.blocknum);
						print(buffer->buffer[buffernum].BlockContent + nowdata.tuplenum+1, attribute);
					}
				}
				it++;
			}
			TNode = TNode->next;
		}
		break;
	}
	case 4: {					// >
		it = conNode->data.begin();
		while (it != conNode->data.end()) {
			if (*it > searchdata) {
				nowdata = *it;
				if (nowdata.isdelete == false) {
					if (set) set0(nowdata, table);
					else {
						int buffernum = buffer->GetBufferNum(table.TableName + ".table", nowdata.blocknum);
						print(buffer->buffer[buffernum].BlockContent + nowdata.tuplenum+1, attribute);
					}
				}
			}
			it++;
		}
		TNode = conNode->next;
		while (TNode != nullptr) {
			it = TNode->data.begin();
			while (it != TNode->data.end()) {
				nowdata = *it;
				if (nowdata.isdelete == false) {
					if (set) set0(nowdata, table);
					else {
						int buffernum = buffer->GetBufferNum(table.TableName + ".table", nowdata.blocknum);
						print(buffer->buffer[buffernum].BlockContent + nowdata.tuplenum+1, attribute);
					}
				}
				it++;
			}
			TNode = TNode->next;
		}
		break;
	}
	case 5: {					// !=
		while (TNode->pre != nullptr) TNode = TNode->pre; firstNode = TNode;
		TNode = firstNode;
		while (TNode != conNode) {
			it = TNode->data.begin();
			while (it != TNode->data.end()) {
				nowdata = *it;
				if (nowdata.isdelete == false) {
					if (set) set0(nowdata, table);
					else {
						int buffernum = buffer->GetBufferNum(table.TableName + ".table", nowdata.blocknum);
						print(buffer->buffer[buffernum].BlockContent + nowdata.tuplenum+1, attribute);
					}
				}
				it++;
			}
			TNode = TNode->next;
		}
		it = conNode->data.begin();
		while (it != conNode->data.end()) {
			if (!(*it == searchdata)) {
				nowdata = *it;
				if (nowdata.isdelete == false) {
					if (set) set0(nowdata, table);
					else {
						int buffernum = buffer->GetBufferNum(table.TableName + ".table", nowdata.blocknum);
						print(buffer->buffer[buffernum].BlockContent + nowdata.tuplenum+1, attribute);
					}
				}
			}
			it++;
		}
		TNode = conNode->next;
		while (TNode != nullptr) {
			it = TNode->data.begin();
			while (it != TNode->data.end()) {
				nowdata = *it;
				if (nowdata.isdelete == false) {
					if (set) set0(nowdata, table);
					else {
						int buffernum = buffer->GetBufferNum(table.TableName + ".table", nowdata.blocknum);
						print(buffer->buffer[buffernum].BlockContent + nowdata.tuplenum +1, attribute);
					}
				}
				it++;
			}
			TNode = TNode->next;
		}
		break;
	}
	default: {break; }
	}

}
void IndexManager::DeleteAll(string indexname)
{
	BuildBTreeFromFile(indexname);
	BTree empty(mybtree.Gettablename(),mybtree.Getattr(),mybtree.Getindexname(),mybtree.Getflag(),mybtree.GetK());
	empty.OutputtoFile(indexname + ".txt");
	mybtree = empty;
}
void IndexManager::DropIndex(string indexname)
{
	if (remove((indexname+".txt").c_str()) == 0) {
		cout << "delete the index named " +indexname << endl;
	}
	else cout << "can not delete  the index file named " + indexname << endl;
}
void IndexManager::InsertBtree(MyTable& table, My_place& data, string indexname)
{
	ifstream inf;
	inf.open(indexname + ".txt");
	string tablename, attr;
	int flag, k;
	int isempty;
	inf >> tablename >> attr >> flag >> k >> isempty;
	if (tablename!=mybtree.Gettablename()||attr!=mybtree.Getattr()) {	// if not corrtct reload B+Tree
		BuildBTreeFromFile(indexname);
	}
	mybtree.isempty = 0;
	
	int offset = 1;
	for (int i = 0; i < table.attributes.AttrNum; i++) {	// find correct attribute
		if (table.attributes.AttributeName[i] == attr) {
			flag = table.attributes.Flag[i];
			break;
		}
		else {												// get correct attribute offset
			if (table.attributes.Flag[i] == -2 || table.attributes.Flag[i] == -1) offset += 4;
			else offset += table.attributes.Flag[i];
		}
	}
	int datasize;											// object attribute data size
	if (flag == -2 || flag == -1) datasize = 4;
	else datasize = flag;								// not +1 because we don't save '\0'

	// get content
	TupleData indata;
	for (int i = 0; i < datasize; i++) {
		indata.tuple_data[i] = buffer->buffer[data.buffer_num].BlockContent[data.tuple_offset + offset + i];
	}
	
	indata.blocknum = data.bolck_num; indata.flag = flag; indata.isdelete = false; indata.tuplenum = data.tuple_offset;
	mybtree.Insert(indata);
}
void IndexManager::print(char* test_char, Attribute& test_Attri) {
	int i = 0;
	int t = 0;

	while (i < test_Attri.AttrNum) {
		cout << "|";
		if (test_Attri.Flag[i] == -2) {
			int p;
			p = test_char[t] & 0xFF;
			p = p * 256;
			p += test_char[t + 1] & 0xFF;
			p = p * 256;
			p += test_char[t + 2] & 0xFF;
			p = p * 256;
			p += test_char[t + 3] & 0xFF;

			int pp = p;
			//memcpy(&p, test_char, 4);
			/*char pp[4] = {};
			memcpy(pp, test_char + t, 4);
			p = atoi(pp);*/
			t += 4;
			cout << left << setw(9) << pp;
		}
		else if (test_Attri.Flag[i] == -1) {
			float p;
			char pp[4];
			memcpy(pp, test_char + t, 4);
			p = *(float*)pp;

			t += 4;
			cout << setprecision(6) << left << setw(9) << p;
		}
		else if (test_Attri.Flag[i] > 0) {
			char p[20] = {};
			memcpy(p, test_char + t, test_Attri.Flag[i]);
			cout << p << " ";
			t += test_Attri.Flag[i];

		}
		i++;
	}
	cout << endl;
}
void IndexManager::set0(TupleData& data, MyTable& table)
{
	int buffernum = buffer->GetBufferNum(table.TableName + ".table", data.blocknum);
	buffer->buffer[buffernum].BlockContent[data.tuplenum] = '\0';
	mybtree.Delete(data);
}
bool IndexManager::IsRepeat(MyTable& table, string attr, char* value, string indexname)
{
	int flag;
	if (mybtree.Gettablename() != table.GetTableName() || mybtree.Getattr() != attr) {
		mybtree.~BTree();
		BuildBTreeFromFile(indexname);
	}
	if (mybtree.isempty == 1) return false;
	TupleData searchdata;
	for (int i = 0; i < table.attributes.AttrNum; i++) {	// find correct attribute
		if (table.attributes.AttributeName[i] == attr) {
			flag = table.attributes.Flag[i];
			break;
		}
	}
	if (flag == -2 || flag == -1) {							// copy data
		for (int i = 0; i < 4; i++) {
			searchdata.tuple_data[i] = value[i];
		}
	}
	else {
		for (int i = 0; i < flag; i++) {
			searchdata.tuple_data[i] = value[i];
		}
	}
	PtrtoNode TNode = mybtree.Find(searchdata);
	TupleData thisdata;
	list<TupleData>::iterator it = TNode->data.begin();
	while (it != TNode->data.end()) {
		if (*it > searchdata) {
			break;
		}
		else if (*it == searchdata) {
			thisdata = *it;
			if (thisdata.isdelete == false) {
				return true;
			}
		}
		it++;
	}
	return false;
}
