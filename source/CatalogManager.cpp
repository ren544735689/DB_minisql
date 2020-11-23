#include"CatalogManager.h"
void CatalogManager::set_Buffer(BufferManager& temp_buffer) {
	this->m_Buffer_manager = &temp_buffer;
}
void CatalogManager::create_table(MyTable& temp_table) {//table define
	int temp_buffernum;
	string filename = "";
	filename = temp_table.TableName + "_head" + ".table";

	temp_buffernum = m_Buffer_manager->GetEmptyBuffer();
	//allocateblock a space for a new table named temp_table.TableName
	//this is a table define

	m_Buffer_manager->WriteBlock(temp_buffernum);
	m_Buffer_manager->buffer[temp_buffernum].filename = filename;
	m_Buffer_manager->buffer[temp_buffernum].IsValid = 1;
	m_Buffer_manager->buffer[temp_buffernum].BlockOffset = 0;

	string content;
	content = get_table_define(temp_table);
	strcpy(m_Buffer_manager->buffer[temp_buffernum].BlockContent, content.c_str());
	m_Buffer_manager->WriteBlock_to_File(temp_buffernum);

	//data_table;
	filename = temp_table.TableName  + ".table";
	temp_buffernum = m_Buffer_manager->GetEmptyBuffer();
	m_Buffer_manager->WriteBlock(temp_buffernum);
	m_Buffer_manager->buffer[temp_buffernum].filename = filename;
	m_Buffer_manager->buffer[temp_buffernum].IsValid = 1;
	m_Buffer_manager->buffer[temp_buffernum].BlockOffset = 0;
	memset(m_Buffer_manager->buffer[temp_buffernum].BlockContent,0, BLOCKSIZE);
	m_Buffer_manager->WriteBlock_to_File(temp_buffernum);
}
/*
string CatalogManager::get_table_define(int tablename) {

	int i = 0;//add the information of table define
	int j = 0;
	int temp_strlen = 0;
	
	string str_test = "";
	string content = "";
	str_test = to_string(k);
	i += str_test.length();
	string filename = tablename + "_head.table";
	int temp_buffernum = m_Buffer_manager->GetBufferNum(filename, 0);
	//block num
	strcat(m_Buffer_manager->buffer[temp_buffernum].BlockContent, (char *)&str_test);
	m_Buffer_manager->buffer[temp_buffernum].BlockContent[i++] = '\n';

	//AttriNum
	k = temp_table.attributes.AttrNum;
	str_test = "";
	str_test = to_string(k);
	content += str_test + "\n";

	while (j < temp_table.attributes.AttrNum) {
		//AttriName
		str_test = temp_table.attributes.AttributeName[j] + " ";
		content += str_test + " ";
		//AttriType
		k = temp_table.attributes.Flag[j];
		if (k > 0) { //char(k) 
			str_test == " "+to_string(k) + " ";
			content += str_test + " ";
		}
		else if (k == -1) { //" float "
			str_test == " -1 ";
			content += str_test + " ";
		}
		else if (k == -2) {//"   int " 
			str_test == " -2 ";
			content += str_test + " ";
		}
		k = temp_table.attributes.IsUnique[j];
		//Attriisunique
		if (k == 1) {
			str_test = "Is_U";
			content += str_test + " ";
		}
		else {
			str_test = "NotU";
			content += str_test + " ";
		}
		content += "\n";
		j++;
	}
	//content += "PrimarKey: ";
	k = temp_table.PrimaryLocation;//-1 stand there is no primary key
	content += to_string(k) + "\n";

	//index;
	k = temp_table.indices.IndexNum;
	j = 0;
	str_test = to_string(k);
	content += str_test + "\n";
	while (j < temp_table.indices.IndexNum) {
		//indexName
		str_test = temp_table.indices.IndexName[j] + " ";
		content += str_test;
		//index Position
		k = temp_table.indices.Position[j];
		str_test = to_string(k) + "\n";
		content += str_test;
		j++;
	}
	return content;
}
*/
string CatalogManager::get_table_define(MyTable& temp_table) {
	string content="";
	content += to_string(temp_table.BlockNum);
	content += "\n";
	int attrinum = temp_table.attributes.AttrNum;
	content += to_string(attrinum)+"\n";
	int i = 0;
	while (i < attrinum) {
		content += temp_table.attributes.AttributeName[i]+" ";
		content += to_string(temp_table.attributes.Flag[i]) + " ";
		if (temp_table.attributes.IsUnique[i] == true) {
			content += " IS_U ";
		}
		else content += " NotU ";
		content += "\n";
		i++;
	}
	content += to_string(temp_table.PrimaryLocation) + "\n";
	int indexnum = temp_table.indices.IndexNum;
	i = 0;
	content += to_string(temp_table.indices.IndexNum) + "\n";
	while (i < indexnum) {
		content += temp_table.indices.IndexName[i]+" ";
		content += to_string(temp_table.indices.Position[i]) + " ";
		content += "\n";
		i++;
	}
	return content;
}
void CatalogManager::drop_table(string tablename) {
	/*int blocknum = temp_table.BlockNum;
	int j = 0;
	int buffernum = 0;*/
	//tablename_head.table
	string filename = tablename + "_head.table";
	//int buffernum = m_Buffer_manager->GetBufferNum(filename, 0);
	m_Buffer_manager->SetInvalid(filename);
	//m_Buffer_manager->WriteBlock_to_File();
	
	if (remove((filename).c_str()) == 0) {
		cout << "delete the table head file named " +  tablename<< endl;
	}
	else cout << "can not delete  the table head file named " + tablename << endl;

	//tablename.table
	filename = tablename + ".table";
	m_Buffer_manager->SetInvalid(filename);
	if (remove((filename).c_str()) == 0) {
		cout << "delete the table  file named " + tablename << endl;
	}
	else cout << "can not delete  the table head file named " + tablename << endl;

}
void CatalogManager::create_index(MyTable& temp_table, string indexname, int indexAttri) {
	int i = temp_table.indices.IndexNum++;
	temp_table.indices.Position[i] = indexAttri;
	temp_table.indices.IndexName[i] = indexname;
	string content = "";
	content = get_table_define(temp_table);
	string filename = temp_table.GetTableName() + "_head.table";

	int temp_buffernum = m_Buffer_manager->GetBufferNum(filename, 0);
	strcpy(m_Buffer_manager->buffer[temp_buffernum].BlockContent, content.c_str());
	m_Buffer_manager->WriteBlock(temp_buffernum);
}
void CatalogManager::drop_index(MyTable& temp_table) {
	//temp_table.DropIndex(indexname);
	//write back
	string content = "";
	content = get_table_define(temp_table);
	string filename = temp_table.GetTableName() + "_head.table";

	int temp_buffernum = m_Buffer_manager->GetBufferNum(filename, 0);
	strcpy(m_Buffer_manager->buffer[temp_buffernum].BlockContent, content.c_str());
	m_Buffer_manager->WriteBlock(temp_buffernum);
}

MyTable* CatalogManager::Get_My_Table(string tablename) {//get the table from table_head.txt
	int block_num;
	Attribute temp_attributes;
	Index temp_index;
	int Attrinum;
	int Primarykey_index;
	int Indexnum;
	int buffernum;
	string filename = tablename + "_head.table";
	string the_content="";
	string str_test="";
	//block num
	try {
		buffernum = m_Buffer_manager->GetBufferNum(filename, 0);
		the_content = m_Buffer_manager->buffer[buffernum].BlockContent;
	}
	catch(TableException exc){
		m_Buffer_manager->SetInvalid(filename);
		throw TableException("table not exist");
		return NULL;
	}
	if (m_Buffer_manager->buffer[buffernum].IsValid == false) {
		throw TableException("table not exist");
		return NULL;
	}
	int i, j;
	i = j = 0;
	j = the_content.find_first_of('\n');
	str_test = the_content.substr(i, j - i);
	block_num = stoi(str_test);
	//Attrinum
	the_content[j] = ' ';
	i = j+1;
	j = the_content.find_first_of('\n');
	str_test = the_content.substr(i, j - i);
	Attrinum = stoi(str_test);
	//primary key
	the_content[j] = ' ';
	i = j+1;
	int m = 0;
	string AttriName="";
	string AttriType = "";
	string is_unique = "";
	int n = 0;
	char test_char;
	while (m < Attrinum) {
		AttriName = "";
		AttriType = "";
		is_unique = "";
		j = the_content.find_first_of('\n');
		test_char = the_content.at(i);
		while (test_char != ' ') {//AttriName
			AttriName += test_char;
			i++;
			test_char = the_content.at(i);
		}
		while (test_char == ' ') {
			i++;
			test_char = the_content.at(i);
		}
		while (test_char != ' ') {//AttriType
			AttriType += test_char;
			i++;
			test_char = the_content.at(i);
		}
		while (test_char == ' ') {
			test_char = the_content.at(i);
			i++;
			test_char = the_content.at(i);
		}
		while (test_char != ' '&&i<j) {//isunique
			is_unique += test_char;
			i++;
			test_char = the_content.at(i);
		}
		i = j + 1;
		the_content[j] = ' ';
		//test_char = the_content.at(i);
		//solve the Attriname
		temp_attributes.AttributeName[m] = AttriName;
		temp_attributes.Flag[m] = stoi(AttriType);
		temp_attributes.IsUnique[m] = false;
		if (is_unique.find("IS_U") !=string::npos) {
			temp_attributes.IsUnique[m] = true;
		}
		m++;
	}
	temp_attributes.AttrNum = Attrinum;
	the_content[j] = ' ';
	i = j + 1;
	j = the_content.find('\n');
	string index_pri = "";
	while (i < j) {
		index_pri += the_content.at(i);
		i++;
	}
	the_content[j] = ' ';
	i = j + 1;
	j = the_content.find('\n');

	string index_num = "";
	while (i < j) {
		index_num += the_content.at(i);
		i++;
	}
	Indexnum = stoi(index_num);
	//indexnum
	m = 0;
	i = j + 1;
	the_content[j] = ' ';
	string indexname="";
	string index_index = "";
	while (m < Indexnum) {
		indexname = "";
		index_index = "";
		j = the_content.find('\n');
		test_char = the_content.at(i);
		while (test_char != ' ') {
			indexname += test_char;
			i++;
			test_char=the_content.at(i);
		}
		i++;
		test_char = the_content.at(i);
		while (test_char == ' ') {
			i++;
			test_char = the_content.at(i);
		}
		while (test_char != '\n'&&test_char!=' ') {
			index_index += test_char;
			i++;
			test_char = the_content.at(i);
		}
		i = j + 1;
		the_content[j] = ' ';
		temp_index.IndexName[m] = indexname;
		temp_index.Position[m] =stoi(index_index);
		m++;
	}
	temp_index.IndexNum = Indexnum;

	MyTable* temp_table= new MyTable(tablename, temp_attributes, block_num);
	temp_table->indices = temp_index;
	temp_table->PrimaryLocation = stoi(index_pri);
	//MyTable* temp_test_table = new MyTable(temp_table);
	return temp_table;
}
