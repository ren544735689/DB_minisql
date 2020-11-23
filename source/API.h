#pragma once
#include"DataBase.h"
#include"CatalogManager.h"
#include"RecordManager.h"
#include"BufferManager.h"
#include"IndexManager.h"
//#include"Intepreter.h"
class API {
private:
	CatalogManager API_Catalog;
	RecordManager API_Record;
	IndexManager  API_Index;
	vector<MyTable*> MyTableDef;
public:
	API() {};
	~API() {};

	void set_buffer(BufferManager& temp_buffer) {
		API_Catalog.set_Buffer(temp_buffer);
		API_Record.set_Buffer(temp_buffer);
		API_Index.set_Buffer(temp_buffer);
	}
	void api_insert_table(string tablename, MyTuper&tempmytupler);
	int Istable(string tablename);
	//check it exsits the vector ,if not check buffer from Catalog
	void api_create_table(string tablename,Attribute &temp_attribue,string Pri_key);
	void api_create_index(string indexname, string tablename, string attrname);
	void drop_table(string tablename);
	void drop_table_fromDef(string TableName);
	void drop_index(string indexname,string tablename);
	void select_tuple_sql(string tablename);
	void select_tuple_sql(string tablename, WHERE tempwhere, string value, string AttriName);
	void delete_tuple_sql(string tablename);
	void delete_tuple_sql(string tablename, WHERE tempwhere, string value, string AttriName);
	string get_tablename_fromindex(string idnexname);
};
