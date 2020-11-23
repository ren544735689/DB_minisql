#pragma once

#include"DataBase.h"
#include<iostream>
#include"BufferManager.h"
#include<string>
using namespace std;
class CatalogManager {
private:
	BufferManager* m_Buffer_manager;
public:
	CatalogManager() {};
	~CatalogManager(){};
	MyTable* Get_My_Table(string tablename);
	void set_Buffer(BufferManager& temp_buffer);
	void create_table(MyTable& temp_table);
	string get_table_define(MyTable& temp_table);
	//string get_table_define(int tablename);
	void drop_table(string tablename);
	void create_index(MyTable& temp_table, string indexname, int indexAttri);
	void drop_index(MyTable& temp_table);
	//string find_table_name(string indexname);
};
