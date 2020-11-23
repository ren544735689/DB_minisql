#include"API.h"
void   API::api_insert_table(string tablename, MyTuper&tempmytupler) {
	int tableindex;
	tableindex = Istable(tablename);
	MyTable* test_Table;
	if (tableindex < 0) {
		try {
			test_Table = API_Catalog.Get_My_Table(tablename);
		}
		catch (TableException exc) {
			throw exc;
			//throw ("table named" + tablename + " not exist");
			return;
		}
		MyTableDef.push_back(test_Table);
		tableindex = MyTableDef.size() - 1;
	}
	//exist this table 
	//now insert
	try {
		MyTableDef[tableindex]->AddTuper(&tempmytupler);
	}
	catch (TableException exc) {
		throw exc;
		//display message 
		//which means it can not be inserted;
		return;
	}
	///check
	int pri = 0;
	for(pri =0;pri< MyTableDef[tableindex]->indices.IndexNum;pri++){
		char *p = new char(30);
		if (MyTableDef[tableindex]->attributes.Flag[pri] > 0) {
			MyChar* mint = (MyChar*)((tempmytupler.Data[pri]));
			strcpy(p, mint->value.c_str());
		}
		else if (MyTableDef[tableindex]->attributes.Flag[pri] == -2) { //int
			MyInt* mint = (MyInt*)((tempmytupler.Data[pri]));
			char pp[5] = {};
			int n = mint->value;
			pp[0] = (n >> 24) & 0xFF;
			pp[1] = (n >> 16) & 0xFF;
			pp[2] = (n >> 8) & 0xFF;
			pp[3] = n & 0xFF;
			pp[4] = '\0';
			memcpy(p, pp, 4);
		}
		else if (MyTableDef[tableindex]->attributes.Flag[pri] == -1) {
			MyFloat* mint = (MyFloat*)((tempmytupler.Data[pri]));
			unsigned char p[5] = {};
			float n = mint->value;
			memcpy(p, &n, 4);
		}
		string attname = MyTableDef[tableindex]->attributes.AttributeName[pri];
		string idname = MyTableDef[tableindex]->indices.IndexName[pri];

		bool check = API_Index.IsRepeat(*MyTableDef[tableindex], attname, p,idname);

		if (check == true) {
			throw TableException("the attribtue at " + to_string(pri) + "named " + attname + "is a primary or unique key");
			return;
		}
	}
	API_Record.insert_tuple_table(*(MyTableDef[tableindex]), tempmytupler);
	
	//index
	int i;
	for (i = 0; i < MyTableDef[tableindex]->indices.IndexNum; i++) {
		if (MyTableDef[tableindex]->indices.Position[i] == MyTableDef[tableindex]->PrimaryLocation) {	
		My_place test_my_place= API_Record.get_insert_place();
		API_Index.InsertBtree(*MyTableDef[tableindex], test_my_place,MyTableDef[tableindex]->indices.IndexName[i]);
		}
	}
}
int    API::Istable(string tablename) {
	int i = 0;
	for (auto iter = MyTableDef.begin(); iter != MyTableDef.end(); iter++) {
		if ((*iter)->TableName == tablename) {
			return i;
		}
		i++;
	}
	//not find
	/*MyTable* temp_table;
	try {
		temp_table = API_Catalog.Get_My_Table(tablename);
	}
	catch (TableException exc) {
		throw TableException("Table not exist");
		return -1;
	}
	MyTableDef.push_back(temp_table);
	return MyTableDef.size() - 1;*/
	return -1;
}

void   API::api_create_table(string tablename, Attribute &temp_attribue, string Pri_key) {
	int tableindex;
	tableindex = Istable(tablename);
	if (tableindex >= 0) {
		throw TableException("Table already Exsit");
		return;
	}
	MyTable* temp_table;
	try {
		temp_table= (API_Catalog.Get_My_Table(tablename));
	}
	catch (TableException exc) {//not exist 
		MyTable test_table(tablename, temp_attribue, 1);
		//block num is 0 at the beginning
		if (Pri_key == "")
			test_table.PrimaryLocation = -1;
		else {
			int m;
			for (m = 0; m < temp_attribue.AttrNum; m++) {
				if (temp_attribue.AttributeName[m] == Pri_key) {
					test_table.PrimaryLocation = m;
					test_table.indices.Position[test_table.indices.IndexNum] = m;
					break;
				}
			}
			if (m == temp_attribue.AttrNum) {
				throw TableException("The primary key does not match,please cheack");
				return;
			}
			
		}
		test_table.indices.IndexName[test_table.indices.IndexNum] = tablename + "_" + Pri_key + "_index";
		test_table.indices.IndexNum++;

		MyTableDef.push_back(&test_table);
		API_Catalog.create_table(test_table);
		
		//create index
		string indexname = tablename + "_" + Pri_key + "_index";
		API_Index.CreateIndex(test_table, Pri_key, indexname);
		return;
	}
		throw TableException("Table already Exsit");
		return;
}
void   API::api_create_index(string indexname, string tablename, string attrname) {
	int tableindex;
	tableindex = Istable(tablename);
	MyTable* test_Table;
	if (tableindex < 0) {
		try {
			test_Table=API_Catalog.Get_My_Table(tablename);
		}
		catch(TableException exc){
			//exc.DisplayMessage();
			throw TableException("table not Exist");
			return;
		}
		MyTableDef.push_back(test_Table);
		tableindex = MyTableDef.size() - 1;
	}
	for (int n = 0; n < MyTableDef[tableindex]->indices.IndexNum; n++) {
		if (MyTableDef[tableindex]->indices.IndexName[n] == indexname) {
			throw TableException("the index named " + indexname + " exist ont the table named " + MyTableDef[tableindex]->TableName);
			return;
		}

	}//check the index exist or not;
		int m;
		for (m = 0; m < MyTableDef[tableindex]->attributes.AttrNum; m++) {
			if (attrname == MyTableDef[tableindex]->attributes.AttributeName[m]) {
				MyTableDef[tableindex]->indices.IndexName[MyTableDef[tableindex]->indices.IndexNum] = attrname;
				MyTableDef[tableindex]->indices.Position[MyTableDef[tableindex]->indices.IndexNum] = m;
				break;
			}
		}
		
		if (m == MyTableDef[tableindex]->attributes.AttrNum) {
			throw TableException("there is no Attribute at the table named" + tablename);
			return;
		}
		if (MyTableDef[tableindex]->attributes.IsUnique[m] == false) {
			throw TableException("can not build index on the attribute which is not unique  " );
			return;
		}
		API_Catalog.create_index(*MyTableDef[tableindex], indexname,m );
		
		//index
		API_Index.CreateIndex(*MyTableDef[tableindex], attrname, indexname);
}
void   API::drop_table(string tablename) {
	int tableindex;
	tableindex = Istable(tablename);
	MyTable* test_Table;
	if (tableindex < 0) {
		try {
			test_Table = API_Catalog.Get_My_Table(tablename);
		}
		catch (TableException exc) {
			//exc.DisplayMessage();
			throw exc;
			return ;
		}
		//exsit 
		MyTableDef.push_back(test_Table);
		tableindex = MyTableDef.size() - 1;
		//API_Catalog.drop_table(tablename);
		//return;
		//only exist in the file but not int the my tabledef
	}
	//exist in the mytabledef and file
	API_Catalog.drop_table(tablename);
	//index
	int i = 0;
	for (i = 0; i < MyTableDef[tableindex]->indices.IndexNum; i++) {
		API_Index.DropIndex(MyTableDef[tableindex]->indices.IndexName[i]);
	}
	drop_table_fromDef(tablename);//drop table drom mytabeledef
}

void   API::drop_table_fromDef(string TableName)//drop table
{
	int flag = 1;
	for (auto iter = MyTableDef.begin(); iter != MyTableDef.end(); iter++) {        //delete one element from vector 
		if ((*iter)->TableName == TableName) {
			MyTableDef.erase(iter);
			flag = 0;
			break;
		}
	}
	if (flag) throw TableException("There is no table named " + TableName + "!");
	//EXception
}
void   API::drop_index(string indexname,string tablename) {
	//think about how to achieve this 
	int tableindex;
	tableindex = Istable(tablename);
	MyTable* test_Table;
	if (tableindex < 0) {
		try {
			test_Table = API_Catalog.Get_My_Table(tablename);
		}
		catch (TableException exc) {
			//exc.DisplayMessage();
			throw TableException("table not Exist");
			return;
		}
		MyTableDef.push_back(test_Table);
		tableindex = MyTableDef.size() - 1;
	}
	//table exist 
	//check the idnex exist or not
	int n;
	for( n= 0; n < MyTableDef[tableindex]->indices.IndexNum; n++) {
		if (MyTableDef[tableindex]->indices.IndexName[n] == indexname) {
			break;
		}

	}//check the index exist or not;
	if (n == MyTableDef[tableindex]->indices.IndexNum) {
		throw TableException("there is no index named " + indexname + "on the table named " + tablename);
		return ;
	}
	MyTableDef[tableindex]->DropIndex(indexname);
	API_Catalog.drop_index(*MyTableDef[tableindex]);
	
	//index
	API_Index.DropIndex(indexname);
}

void   API::select_tuple_sql(string tablename) {
	int tableindex;
	tableindex = Istable(tablename);
	MyTable* test_Table;
	if (tableindex < 0) {
		try {
			test_Table = API_Catalog.Get_My_Table(tablename);
		}
		catch (TableException exc) {
			//exc.DisplayMessage();
			throw exc;
			return;
		}
		//Exit
		MyTableDef.push_back(test_Table);
		tableindex = MyTableDef.size() - 1;
	}

	API_Record.serach_in_table(tablename, *MyTableDef[tableindex]);
}
void   API::select_tuple_sql(string tablename, WHERE tempwhere, string value, string AttriName) {
	int tableindex;
	tableindex = Istable(tablename);
	MyTable* test_Table;
	if (tableindex < 0) {
		try {
			test_Table = API_Catalog.Get_My_Table(tablename);
		}
		catch (TableException exc) {
			string str_op=exc.DisplayMessage();
			throw TableException(str_op);
			return;
		}
		//Exist
		MyTableDef.push_back(test_Table);
		tableindex = MyTableDef.size() - 1;
	}
	int i;
	for (i = 0; i < MyTableDef[tableindex]->attributes.AttrNum; i++) {
		if (MyTableDef[tableindex]->attributes.AttributeName[i] == AttriName) {
			break;
		}
	}
	if (i == MyTableDef[tableindex]->attributes.AttrNum) {
		cout << "no attribute like this" << endl;
	}
	int j;
	for ( j = 0; j < MyTableDef[tableindex]->indices.IndexNum; j++) {
		if (MyTableDef[tableindex]->indices.Position[j] == i) {
			break;
		}
	}
	//if (j == MyTableDef[tableindex]->indices.IndexNum)//no index
	if (i != MyTableDef[tableindex]->PrimaryLocation)//no index on primary 
		API_Record.select_or_delete_from_table(true,*MyTableDef[tableindex], tempwhere, value, AttriName);
	else {
		char *p = new char(30);
		if (MyTableDef[tableindex]->attributes.Flag[i] > 0) {
			strcpy(p, value.c_str());
		}
		else if(MyTableDef[tableindex]->attributes.Flag[i] ==-2){ //int
			int myint = stoi(value);
			*p = (myint >> 24) & 0xFF;
			*(p+1) = (myint >> 16) & 0xFF;
			*(p+2) = (myint >> 8) & 0xFF;
			*(p+3) = myint & 0xFF;
		}
		else if (MyTableDef[tableindex]->attributes.Flag[i]== -1) {
			float myfloat = stof(value);
			memcpy(p, &myfloat, 4);
		}
		
		API_Index.Search(p, *MyTableDef[tableindex],AttriName, MyTableDef[tableindex]->indices.IndexName[j],tempwhere,false);
	}
}
void   API::delete_tuple_sql(string tablename) {
	int tableindex;
	tableindex = Istable(tablename);
	MyTable* test_Table;
	if (tableindex < 0) {
		try {
			test_Table = API_Catalog.Get_My_Table(tablename);
		}
		catch (TableException exc) {
			//exc.DisplayMessage();
			throw exc;
			return;
		}
		//Exit
		MyTableDef.push_back(test_Table);
		tableindex = MyTableDef.size() - 1;
	}
	API_Record.delete_from_table(tablename, MyTableDef[tableindex]->BlockNum);
	
	//index
	int i = 0;
	for (i = 0; i < MyTableDef[tableindex]->indices.IndexNum; i++) {
		API_Index.DeleteAll(MyTableDef[tableindex]->indices.IndexName[i]);
	}
	
}
void   API::delete_tuple_sql(string tablename, WHERE tempwhere, string value, string AttriName) {
	int tableindex;
	tableindex = Istable(tablename);
	MyTable* test_Table;
	if (tableindex < 0) {
		try {
			test_Table = API_Catalog.Get_My_Table(tablename);
		}
		catch (TableException exc) {
			//exc.DisplayMessage();
			throw exc;
			return;
		}
		//Exit
		MyTableDef.push_back(test_Table);
		tableindex = MyTableDef.size() - 1;
	}
	//API_Record.select_or_delete_from_table(false, *MyTableDef[tableindex], tempwhere, value, AttriName);
	int i;
	for (i = 0; i < MyTableDef[tableindex]->attributes.AttrNum; i++) {
		if (MyTableDef[tableindex]->attributes.AttributeName[i] == AttriName) {
			break;
		}
	}
	if (i == MyTableDef[tableindex]->attributes.AttrNum) {
		cout << "no attribute like this" << endl;
	}
	int j;
	for (j = 0; j < MyTableDef[tableindex]->indices.IndexNum; j++) {
		if (MyTableDef[tableindex]->indices.Position[j] == i) {
			break;
		}
	}
	//if (j == MyTableDef[tableindex]->indices.IndexNum)
	if (i != MyTableDef[tableindex]->PrimaryLocation)//no index on primary 
		API_Record.select_or_delete_from_table(false, *MyTableDef[tableindex], tempwhere, value, AttriName);
	else {
		//index
		char *p = new char(30);
		if (MyTableDef[tableindex]->attributes.Flag[i] > 0) {
			strcpy(p, value.c_str());
		}
		else if (MyTableDef[tableindex]->attributes.Flag[i] == -2) { //int
			int myint = stoi(value);
			*p = (myint >> 24) & 0xFF;
			*(p + 1) = (myint >> 16) & 0xFF;
			*(p + 2) = (myint >> 8) & 0xFF;
			*(p + 3) = myint & 0xFF;
		}
		else if (MyTableDef[tableindex]->attributes.Flag[i] == -1) {
			float myfloat = stof(value);
			memcpy(p, &myfloat, 4);
		}
		API_Index.Search(p, *MyTableDef[tableindex], AttriName, MyTableDef[tableindex]->indices.IndexName[j], tempwhere, true);
	}
}
string API::get_tablename_fromindex(string idnexname) {
	//wait to implement;
	//return "tablename";
	return API_Index.gettablename(idnexname);
	//wait to achieve this
}
