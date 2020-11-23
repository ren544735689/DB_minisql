#pragma once
#include"API.h"
#include <iostream>
class API;
class Intepreter {
private:
	API my_api;
public:
	Intepreter() {
	}
	~Intepreter() {}
	void set_buffer(BufferManager& test_buffer) {
		my_api.set_buffer(test_buffer);
	}
	void insert_sql(string tablename, MyTuper& temptuple);//insert
	void Run();
	WHERE get_where(string test_str);
	void process_where(string test_str, int t,int type);
	//0 select
	//1 delect
	void create_table_sql(string tablename,Attribute& temp_Attrribute,string Pri_key);//create table
	void create_index_sql(string indexname,string tablename,string attname);//create index
	void test_str_process(string test_str, Attribute& temp_Attribute);
	void drop_table_sql(string tablename);//drop table
	void drop_index_sql(string indexname);	//drop index
	void delete_tuple_sql(string tablename);
	void delete_tuple_sql(string tablename,WHERE tempwhere,string value,string AttriName);
	void select_tuple_sql(string tablename);
	void select_tuple_sql(string tablename, WHERE tempwhere, string value, string AttriName);
	
	//delete from
	//select*;
};
