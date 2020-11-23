#pragma once
#include"DataBase.h"
#include<string>
#include"BufferManager.h"

struct My_Attri {
	int index;
	int flag;
};

class RecordManager {
private:
	BufferManager* m_Buffer_manager;
	My_place the_place;
public:
	My_place get_insert_place(){
		return this->the_place;
	}
	RecordManager() {}
	~RecordManager() {}
	void set_Buffer(BufferManager& temp_buffer) {
		this->m_Buffer_manager = &temp_buffer;
	}
	//My_Attri get_Attriindex(string tableName, string Attriname);

	int compare(WHERE tempwhere, char A[], char B[],int size);
	int get_blocknum(string tablename);
	int get_Attrinum(string tablename);
	void insert_tuple_table(MyTable& temp_table, MyTuper& temp_tuper);
	void create_table(MyTable& temp_table);
	void delete_from_table(string tablename,int blocknum);
	//void print(char temp[], int size);
	void select_or_delete_from_table(bool select,MyTable& test_table, WHERE tempwhere, string value, string Attriname);
	void serach_in_table(string tablename, MyTable& temp_table);
	void print(char test_char[], Attribute& test_Attri);
	int cmp_of_mine(char A[], char B[],int size);
	void check(MyTable& temp_table, MyTuper& temp_tuper);
	string get_table_define(MyTable& temp_table);
};  
template<class T>
int compare_int_float(WHERE tempwhere, T A, T B);

