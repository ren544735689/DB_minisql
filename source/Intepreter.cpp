#include"Intepreter.h"
/*
verify:
	drop index indexname;----done
	drop table tablename;----done

create:	
	create index:
	create index indexname on tablename ( second );----done
	--------done
	create table tablename (
		first int unique ,
		second float ,
		thired char(8) ,
		primary key ( first )
	);
	create table DMID (
		first int ,
		second float ,
		thired char(8) ,
		primary key ( first )
	);
	

	create table book (
		bookid int ,
		bookprice float unique ,
		bookname char(8) ,
		primary key ( bookid )
	);

	insert into book values (0,99.9,'888888');
	insert into book values (143,3.14,'888888');
	insert into tablename values (0,99.9,'888888');
	insert into tablename values (143,3.14,'888888');
	insert into tablename values (453,453.14,'aaaaaa');
	insert into DMID values (143,3.14,'888888');

select:----- done
	select * from tablename;
	select * from tablename where sno = '888888' ;

delete:------done
	delete from tablename;
	delete from tablename where sno = '888888';

insert:
	insert into tablename values (143,3.14,'888888');
	select * from tablenae where first = 123 ;
	select * from tablename where first <123 ;
	select * from tablename where first <= 123 ;
	select * from tablename where first >= 123 ;
	select * from tablename where first > 123 ;
	select * from tablename where first != 123 ;

*/
void  Intepreter::Run() {
	string test_str;
	cout << "MinSQL---->" ;
	//cin >> test_str;
	
	getline(cin,test_str);
	int t;
	if (test_str.find("insert") != string::npos) {
		//insert into student values(��12345678��, ��wy��, 22, ��M��);
		int j = test_str.find("insert");
		t = 0;
		char test_char;
		MyTuper temptuple;
		test_char = test_str.at(t);
		if (j != 0) {
			while (test_char==' ') {
				t++;
				test_char = test_str.at(t);
			}
		}
		if (test_char != 'i') {
			cout << "something wrong before insert" << endl;
			return;
		}
		t = j;
		t += 6;
		j = test_str.find("into", t);
		if (j < 0) {
			cout << "not into or ther is no ' 'before 'into'" << endl;
			return;
		}
		test_char = test_str.at(t);
		if (test_char != ' ') {
			cout << "something wrong after insert" << endl;
			return;
		}
		while (test_char == ' ') {
			t++;
			test_char = test_str.at(t);
		}
		if (t != j) {
			cout << "something wrong after insert and befor into " << endl;
			return;
		}
		t += 4;
		test_char = test_str.at(t);
		if (test_char != ' ') {
			cout << "something wrong after into" << endl;
			return;
		}
		string tablename="";
		while (test_char == ' ') {
			t++;
			test_char = test_str.at(t);
		}
		while (test_char != ' ') {
			tablename += test_char;
			t++;
			test_char = test_str.at(t);
		}
		j = test_str.find("values", t);
		while (t<j) {
			if (test_char != ' ') {
				cout << "something wrong between into and table:" + tablename;
				return;
			}
			//tablename += test_char;
			t++;
			test_char = test_str.at(t);
		}
		t = j;
		t += 6;
		test_char = test_str.at(t);
		if (test_char == ' ') {
			while (test_char == ' ') {
				t++;
				test_char = test_str.at(t);
			}
		}
		if (test_char != '(') {
			cout << "no (" ;
			return;
		}
		t++;
		test_char = test_str.at(t);
		string str_travel = "";
		while (test_char != ')'&&test_char != ';') {
			str_travel = "";
			while (test_char == ' ') {
				t++;
				test_char = test_str.at(t);
			}
			if (test_char == '\'') {//
				str_travel += test_char;
				t++;
				test_char = test_str.at(t);
				while (test_char != '\'') {
					str_travel += test_char;
					t++;
					test_char = test_str.at(t);
					if (test_char == ',') {
						cout << "not \'" << endl;
						return;
					}

				}
				str_travel += test_char;
				MyChar *temp_char = new MyChar (str_travel.c_str());
				temptuple.AddData(temp_char);
			}//char(n)
			else {
				str_travel += test_char;
				t++;
				test_char = test_str.at(t);
				while (test_char != ','&&test_char != ' '&&test_char != ')') {
					str_travel += test_char;
					t++;
					test_char = test_str.at(t);
				}
				if (str_travel.find('.') !=string::npos) {
					int m = str_travel.find('.');
					int n = 0;
					while (n < str_travel.length()) {
						if (m == n) {
							n++;
							continue;
						}
						else {
							if (str_travel.at(n) >= '0'&&str_travel.at(n) <= '9');
							else {
								cout << "it is not a correct float number of index of " + to_string(t) << endl;
								return;
							}
						}
						n++;
					}
					//MyInt temp_float(stof(str_travel));
					MyFloat *temp_char = new MyFloat(stof(str_travel));
					temptuple.AddData(temp_char);
				}
				else {
					//MyInt temp_int(stoi(str_travel));
					int m = 0;
					char str_char;
					while (m < str_travel.length()) {
						str_char = str_travel.at(m);
						if (str_char >= '0'&&str_char <= '9');
						else {
							throw TableException("int type data is not correct");
							return;
						}
						m++;
					}
					MyInt *temp_char=new MyInt(stoi(str_travel));
					temptuple.AddData(temp_char);
				}
			}
			
			if (test_char == '\'') {
				t++;
				test_char = test_str.at(t);
			}
			while (test_char == ' ') {
				t++;
				test_char = test_str.at(t);
			}
			if (test_char == ',') {
				t++;
				test_char = test_str.at(t);
				continue;
			}
			test_char = test_str.at(t);
			
			if (test_char == ')') {
				while (t < test_str.length()) {
					t++;
					test_char= test_str.at(t);
					if (test_char == ';') break;
				}
				if (test_char != ';') {
					throw TableException("no ;");
				}
				else break;
			}
			if (test_char != ',') {
				cout << "no , at the postion of"+to_string(t) << endl;
				return;
			}
			t++;
			test_char = test_str.at(t);
		}
		try {
			insert_sql(tablename, temptuple);
		}
		catch(TableException exc){

			cout << exc.DisplayMessage() << endl;
			return;
		}
		cout << "insert success" << endl;

	}
	else if (test_str.find("create")!= string::npos) {
		/*create table tablename(
			collumnname type,
			collumnname type,
			collumnname type,
			primary key(collumnname)*/
		t = test_str.find("create");
		t += 6;
		char test_char2;
		string table_or_index="";
		test_char2 = test_str.at(t);
		while (test_char2==' ') {
			t++;
			test_char2 = test_str.at(t);
		}
		while (test_char2 != ' '&&test_char2 != ';') {
			table_or_index += test_char2;
			t++;
			test_char2 = test_str.at(t);
		}
		if (test_char2 != ' ') {
			cout << "something wrong happned after " + table_or_index;
			return;
		}
		if(table_or_index=="table"){
			//t
			string tablename="";
			char test_char;
			test_char = test_str.at(t);
			Attribute tempattribue;
			tempattribue.AttrNum = 0;
			if (test_char != ' ') {
				cout << "no ' ' after create " << endl;
				return;
			}
			while (test_char == ' ') {
				t++;
				test_char = test_str.at(t);
			}
			while (test_char != ' '&&test_char != '(') {
					tablename += test_char;
					t++;
					test_char = test_str.at(t);
			}
			while (test_char == ' ') {
				t++;
				test_char = test_str.at(t);
			}
				if (test_char != '(') {
					cout << "no ( after tablename" << endl;
					return;
				}
				int p=0;
				string pri_key = "";
				while (1) {
					p++;
					cout << "---->";
					//cin >> test_str;
					getline(cin, test_str);
				
					if (test_str.find(')')!=string::npos) {
						int i = test_str.find(')');
						int ii = test_str.find('(');
						int iii = test_str.find(';');
						if (ii < 0&&iii<0)  {
							cout << "no ';' at row:"+to_string(p) << endl;
							return ;
						}
						if (ii < 0&& iii==i+1) {
							break;
						}
						if (ii > 0 && iii > 0) {
							cout << "wrong near ';' at row:" + to_string(p) << endl;
							return;
						}
			
					}
					else {
						if (test_str.find(';') !=string::npos) {
							cout << "wrong near ';' at row :" + to_string(p);
							return;
						}
					}
				
					if (test_str.find("primary") != string::npos) {
						t = 0;
						test_char = test_str.at(t);
						if (test_char == ' ') {
							while (test_char == ' ') {
								t++;
								test_char = test_str.at(t);
							}
						}
						int j = test_str.find("primary");
						if (t != j) {
							cout << "something  wrong before primary" << endl;
							return;
						}
						t += 7;
						test_char = test_str.at(t);
						if (test_char != ' ') {
							cout << "something  wrong after primary" << endl;
							return;
						}
						while (test_char == ' ') {
							t++;
							test_char = test_str.at(t);
						}
						j = test_str.find("key");
						if (t != j) {
							cout << "something  wrong after 'primary' and before 'key'" << endl;
							return;
						}
						t += 4;

						test_char = test_str.at(t);
						while (test_char == ' ') {
							t++;
							test_char = test_str.at(t);
						}
						if (test_char != '(') {
							cout << "no (" << endl;
							return;
						}
						t++;
						test_char = test_str.at(t);
						while (test_char == ' ') {
							t++;
							test_char = test_str.at(t);
						}
						while (test_char != ' '&&test_char != ')') {
							pri_key += test_char;
							t++;
							test_char = test_str.at(t);
						}
						while (test_char == ' ') {
							t++;
							test_char = test_str.at(t);
						}
						if (test_char != ')') {
							cout << "no )" << endl;

						}
						if (t != test_str.length() - 1) {
							cout << "something wrong after )" << endl;
							return;
						}
					}
					else {
						try {
							test_str_process(test_str, tempattribue);
						}
						catch (TableException exc) {
							cout << exc.DisplayMessage();
							cout << "at row :" + to_string(p);
						}
					}
				}
		
				try {
					create_table_sql(tablename, tempattribue, pri_key);
				}
				catch (TableException exc) {
					cout<<exc.DisplayMessage()<<endl;
					return;
				}
				cout << "creat table success" << endl;
		}
		else if (table_or_index=="index") {
			//create index indexname on table(attribute);
			//t
			string indexname="";
			string tablename = "";
			string attriname = "";
			char test_char;
			test_char = test_str.at(t);
			if (test_char != ' ') {
				throw TableException("no ' ' after index");
				return;
			}
			while (test_char == ' ') {
				t++;
				test_char = test_str.at(t);
			}
			while (test_char != ' '&&test_char != '\0') {
				indexname += test_char;
				t++;
				test_char = test_str.at(t);
			}
			if (test_char == '\0') {
				cout << "something wrong after " + indexname;
				return;
			}
			
			int j = test_str.find("on", t);
			while (test_char == ' ') {
				t++;
				test_char = test_str.at(t);
			}
			if (t != j) {
				cout << "something wrong before on ";
				return;
			}
			t += 2;
			test_char = test_str.at(t);
			while (test_char == ' ') {
				t++;
				test_char = test_str.at(t);
			}
			while (test_char != ' '&&test_char != '(') {
				tablename += test_char;
				t++;
				test_char = test_str.at(t);
			}
			if (tablename == "table") {
				cout << "table is key word" << endl;
				return;
			}
			while (test_char == ' ') {
				t++;
				test_char = test_str.at(t);
			}
			if (test_char != '(') {
				cout << "no (" << endl;
				return;
			}
			t++;
			test_char = test_str.at(t);
			while (test_char == ' ') {
				t++;
				test_char = test_str.at(t);
			}
			while (test_char != ' ') {
				attriname += test_char;
				t++;
				test_char = test_str.at(t);
			}
			while (test_char == ' ') {
				t++;
				test_char = test_str.at(t);
			}
			if (test_char != ')') {
				cout << "no )" << endl;
				return;
			}
			t++;
			test_char = test_str.at(t);
			while (test_char == ' ') {
				t++;
				test_char = test_str.at(t);
			}
			if (test_char != ';') {
				cout << "no ;" << endl;
				return;
			}
			if (t != test_str.length() - 1) {
				cout << "something wrong after ;" << endl;
				return;
			}
		
			try {
				create_index_sql(indexname, tablename, attriname);
			}
			catch (TableException exc) {
				cout<<exc.DisplayMessage()<<endl;
				return;
			}
			cout << "create index success" << endl;
		}
		else {
			cout << "create table or index??";
			return;
		}
	}
	else if (test_str.find("drop")!=string::npos) {
		//drop table
		//drop index
		t = test_str.find("drop");
		char test_char;
		string temp_str = "";
		int j = 0;
		test_char=test_str.at(j);
		while (test_char == ' ') {
			j++;
			test_char = test_str.at(j);
		}
		if (t != j) {
			cout << "something wrong before drop" << endl;
			return;
		}
		t += 4;
		test_char = test_str.at(t);
		while (test_char == ' ') {
			t++;
			test_char = test_str.at(t);
		}
		while (test_char != ' '&&test_char!='\0') {
			temp_str += test_char;
			t++;
			test_char = test_str.at(t);
		}
		if (test_char == '\0') {
			cout << "something wrong after " + temp_str;
			return;
		}
		if (temp_str.find("table") !=string::npos) {
			string tablename="";
			while (test_char == ' ') {
				t++;
				test_char = test_str.at(t);
			}
			while (test_char != ' '&&test_char != '\0'&&test_char != ';') {
				tablename += test_char;
				t++;
				test_char = test_str.at(t);
			}
			while (test_char == ' ') {
				t++;
				test_char = test_str.at(t);
			}
			if (test_char == '\0');
			else if (test_char == ';') {
				if (t == test_str.length() - 1);
				else {
					cout << "something wrong after ;" << endl;
					return;
				}
			}
			try {
				drop_table_sql(tablename);
			}
			catch (TableException exc) {
				cout<<exc.DisplayMessage();
				return;
			}
			cout <<"drop table success"<<endl;
		}
		else if (temp_str.find("index")!=string::npos) {
			string indexname="";
			while (test_char == ' ') {
				t++;
				test_char = test_str.at(t);
			}
			while (test_char != ' '&&test_char != '\0'&&test_char != ';') {
				indexname += test_char;
				t++;
				test_char = test_str.at(t);
			}
			while (test_char == ' ') {
				t++;
				test_char = test_str.at(t);
			}
			if (test_char == '\0');
			else if (test_char == ';') {
				if (t == test_str.length() - 1);
				else{
					cout << "something wrong after ;" << endl;
					return;
				}
			}

			try {
				drop_index_sql(indexname);
			}
			catch (TableException exc) {
				cout<<exc.DisplayMessage();
				return;
			}
			cout << "drop index " + indexname + " success" << endl;
		}
		else {
			cout << "drop table or index" << endl;
			return;
		}
	}
	else if (test_str.find("select")!= string::npos) {
		t = test_str.find("select");
		t += 6;
		char test_char;
		test_char = test_str.at(t);
		string temp_str = "";
		if (test_char != ' ') {
			cout << "something wrong after select" << endl;
			return;
		}
		while (test_char == ' ') {
			t++;
			test_char = test_str.at(t);
		}
		while (test_char != ' ') {
			temp_str += test_char;
			t++;
			test_char = test_str.at(t);
		}
		if (temp_str != "*") {
			cout << "the version does not support anything but '*'" << endl;
			return;
		}
		try {
			process_where(test_str, t, 0);
		}
		catch (TableException exc) {
			cout<<exc.DisplayMessage();
			return ;
		}
	}
	else if (test_str.find("delete")!= string::npos) {
		t = test_str.find("delete");
		t += 6;
		char test_char;
		test_char = test_str.at(t);
		if (test_char != ' ') {
			cout << "something wrong after delete" << endl;
			return;
		}
		try {
			process_where(test_str, t, 1);
		}
		catch (TableException exc) {
			cout<<exc.DisplayMessage();
			return;
		}
		cout << "delete success" << endl;
	}
	else {
		cout << "wrong sql!(delete / selete * / insert /create / drop)"<<endl;
		return; 
	}
}
WHERE Intepreter::get_where(string test_str) {
	if (test_str == "=") return eq;
	if (test_str == "!=") return neq;
	if (test_str == ">=") return geq;
	if (test_str == "<=") return leq;
	if (test_str == ">") return g;
	if (test_str == "<") return l;
}
void Intepreter::process_where(string test_str, int t, int type) {
	//delete from student;
	//delete from student where sno = ��88888888��;
	int j = test_str.find("from");
	if (j < 0) {
		throw TableException("no from");
		return;
	}
	char test_char;
	string tablename = "";
	test_char = test_str.at(t);
	while (test_char == ' ') {
		t++;
		test_char = test_str.at(t);
	}
	if (t != j) {
		throw TableException("something wrong before form ");
		return;
	}
	t += 4;
	test_char = test_str.at(t);
	if (test_char != ' ') {
		throw TableException("something wrong after form ");
		return;
	}
	while (test_char == ' ') {
		t++;
		if (t >= test_str.length()) {
			throw TableException("something wrong at the end");
			return;
		}
		test_char = test_str.at(t);
	}
	while (test_char != ' '&&test_char!=';'&&test_char!='\0') {
		tablename += test_char;
		t++;
		if (t >= test_str.length()) {
			throw TableException("something wrong at the end");
			return;
		}
		test_char = test_str.at(t);
	}
	while (test_char == ' ') {
		t++;
		if (t >= test_str.length()) {
			throw TableException("no ;");
			return;
		}
		test_char = test_str.at(t);
	
	}
	if (test_char == ';') {
		if(t!=test_str.length()-1){
			throw TableException("something wrong after ;");
			return;
		}
		if (type == 0) select_tuple_sql(tablename);
		else {
			delete_tuple_sql(tablename);
			cout << "delete from " + tablename + "success" << endl;
		}

		return;
	}
	else if (test_char == '\0') {
		if (type == 0) select_tuple_sql(tablename);
		else delete_tuple_sql(tablename);
		return;
	}
	j = test_str.find("where");
	if (t != j) {
		throw TableException("something wrong befor where");
		return;
	}
	t += 5;
	string AttriName = "";
	string value = "";
	string operation = "";
	test_char = test_str.at(t);
	while (test_char == ' ') {
		t++;
		test_char = test_str.at(t);
	}
	while (test_char != ' '&&test_char!=';'&&test_char != '<'&&test_char != '!'&&test_char != '='&&test_char != '>'&&test_char != '\0') {
		AttriName += test_char;
		t++;
		test_char = test_str.at(t);
	}
	if (test_char == ';'&&test_char == '\0') {
		throw TableException("something wrong after " + AttriName);
		return;
	}
	while (test_char == ' ') {
		t++;
		test_char = test_str.at(t);
	}
	if (test_char == '=') {
		operation += test_char;
		t++;
		test_char = test_str.at(t);
		if (test_char == ' ');
		else if ((test_char >= 'a'&&test_char <= 'z') || (test_char >= 'A'&&test_char <= 'Z'));
		else {
			throw TableException("something wrong after " + operation);
			return;
		}
	}
	else if (test_char == '!') {
		operation += test_char;
		t++;
		test_char = test_str.at(t);
		if (test_char != '=') {
			throw TableException("something wrong after " + operation);
			return;
		}
		else {
			operation += test_char;
			t++;
			test_char = test_str.at(t);
			if (test_char == ' ');
			else if ((test_char >= 'a'&&test_char <= 'z') || (test_char >= 'A'&&test_char <= 'Z'));
			else {
				throw TableException("something wrong after " + operation);
				return;
			}
		}
	}
	else if (test_char == '>'||test_char=='<') {
		operation += test_char;
		t++;
		test_char = test_str.at(t);
		if (test_char != '=') {
			if (test_char == ' ');
			else if ((test_char >= 'a'&&test_char <= 'z') || (test_char >= 'A'&&test_char <= 'Z'));
			else {
				throw TableException("something wrong after " + operation);
				return;
			}
		}
		else {
			operation += test_char;
			t++;
			test_char = test_str.at(t);
			if (test_char == ' ');
			else if ((test_char >= 'a'&&test_char <= 'z') || (test_char >= 'A'&&test_char <= 'Z'));
			else {
				throw TableException("something wrong after " + operation);
				return;
			}

		}
	}
	else {
		throw TableException("something wrong about comparation operation" + AttriName);
		return;
	}
	while (test_char == ' ') {
		t++;
		test_char = test_str.at(t);
	}
	if (test_char == ';') {
		throw TableException("something wrong after " + operation);
		return;
	}
	while (test_char != ' '&&test_char != ';') {
		value += test_char;
		
		t++;
		if (t == test_str.length()) break;
		test_char = test_str.at(t);
		
	}
	while (test_char == ' ') {
		t++;
		if (t == test_str.length()) break;
		test_char = test_str.at(t);
	}
	if (test_char != ';') {
		throw TableException("something wrong after " + value);
		return;
	}
	if (t != test_str.length() - 1) {
		throw TableException("something wrong after ;");
		return;
	}
	try {
		if (type == 0) select_tuple_sql(tablename, get_where(operation), value, AttriName);
		else {
			delete_tuple_sql(tablename, get_where(operation), value, AttriName);
			cout << "delete from " + tablename + "where ... success" << endl;
		}
	}
	catch (TableException exc) {
		cout << exc.DisplayMessage() << endl;
		return;
	}
}
//0 select
//1 delect

void Intepreter::test_str_process(string test_str, Attribute& temp_Attrribute) {
	string AttriName = "";
	string Attritype = "";
	string unique = "";
	char test_char;
	int t = 0;
	test_char = test_str.at(t);
	while (test_char == ' ') {

		t++;
		
		test_char = test_str.at(t);
	}
	while (test_char != ' '&&test_char != ',') {
		AttriName += test_char;
		t++;
		test_char = test_str.at(t);
	}
	while (test_char == ' ') {
		t++;
		test_char = test_str.at(t);

	}
	if (test_char == ',') {
		throw TableException("no Attribute Type");
		return;
	}
	while (test_char != ' '&&test_char != '\0') {
		Attritype += test_char;
		t++;
		test_char = test_str.at(t);
	}
	while (test_char == ' ') {
		t++;
		test_char = test_str.at(t);
	}
	if (test_char == ',');
	else {
		while (test_char != ' ') {
			unique += test_char;
			t++;
			test_char = test_str.at(t);
			if (test_char == ',')break;
		}
		if (unique.find("unique") ==string::npos) {
			throw TableException("wrong after data type");
			return;
		}
		if (test_char == ' ') {
			while (test_char == ' ') {
				t++;
				test_char = test_str.at(t);
				
			}
		}
		if (test_char != ',') {
			throw TableException("no ,");
			return;
		}
	
	}
	if (t != test_str.length()-1) {
		throw TableException("someting wrong after ,");
		return;
	}

	temp_Attrribute.AttributeName[temp_Attrribute.AttrNum] = AttriName;
	if (Attritype.find("int")!=string::npos) {
		t = Attritype.find("int");
		t += 3;
		if(Attritype.length()!=3){
			throw TableException("something wrong arround int");
			return;
		}
		temp_Attrribute.Flag[temp_Attrribute.AttrNum] = -2;
	}
	else if (Attritype.find("float")!=string::npos) {
		t = Attritype.find("float");
		t += 5;
		if (Attritype.length() != 5){
			throw TableException("something wrong arround float");
			return;
		}
		temp_Attrribute.Flag[temp_Attrribute.AttrNum] = -1;
	}
	else if (Attritype.find("char")!=string::npos) {
		t = Attritype.find("char");
		t += 4;
		test_char = Attritype.at(t);
		string char_N = "";
		while (test_char == ' ') {
			t++;
			test_char = Attritype.at(t);
		}
		if (test_char != '(') {
			throw TableException("something wrong arround char such as no (");
			return;
		}
		t++;
		test_char = Attritype.at(t);
		while (test_char != ' '&&test_char != ')'&&test_char != ',') {
			char_N += test_char;
			t++;
			test_char = Attritype.at(t);
		}

		while (test_char == ' ') {
			t++;
			test_char = Attritype.at(t);
		}

		if (test_char != ')') {
			throw TableException("something wrong about char: no  ')'");
			return;
		}
		if (t != Attritype.length() - 1) {
			throw TableException("something wrong after ')' ");
			return;
		}
		temp_Attrribute.Flag[temp_Attrribute.AttrNum] = stoi(char_N);
	}
	if (unique.find("unique") !=string::npos) temp_Attrribute.IsUnique[temp_Attrribute.AttrNum] = 1;
	else temp_Attrribute.IsUnique[temp_Attrribute.AttrNum] = 0;
	temp_Attrribute.AttrNum++;
}
void Intepreter::drop_table_sql(string tablename)//drop table
{
	my_api.drop_table(tablename);
	//cout << "drop table  right" << endl;
	//drop table test start
	//drop table test end-----right

}
void Intepreter::drop_index_sql(string indexname)	//drop index
{
	string tablename = "";
	//test my_api.get_tablename_fromindex(indexname) wait to implement;
	tablename = my_api.get_tablename_fromindex(indexname);
	//tablename = "tablename";
	//tablename test
	my_api.drop_index(indexname,tablename);
	//cout << "drop index right" << endl;
	//test over right

}
void Intepreter::delete_tuple_sql(string tablename) {
	my_api.delete_tuple_sql(tablename);
	//cout << "delete_tuple right no where" << endl;
	//test start
	//test end ---right
}
void Intepreter::delete_tuple_sql(string tablename, WHERE tempwhere, string value, string AttriName) {
	my_api.delete_tuple_sql(tablename, tempwhere, value, AttriName);
	//cout << "delete_tuple right with where" << endl;
	//test start 
	//test 

}
void Intepreter::select_tuple_sql(string tablename) {
	my_api.select_tuple_sql(tablename);
	//cout << "select_tuple right no where" << endl;
	//test selet * from  start
	//test selet * from end --right
}

void Intepreter::select_tuple_sql(string tablename, WHERE tempwhere, string value, string AttriName) {
	my_api.select_tuple_sql(tablename, tempwhere, value, AttriName);
	//cout << "slect_tuple right with where" << endl;
	//test 
}
void Intepreter::insert_sql(string tablename, MyTuper& temptuple)//insert
{
	my_api.api_insert_table(tablename, temptuple);
	//cout << "insert right " << endl;
	//test insert start
	//test insert over;-----right
}
void Intepreter::create_table_sql(string tablename, Attribute& temp_Attrribute, string Pri_key)//create table
{
	
	my_api.api_create_table(tablename, temp_Attrribute, Pri_key);
	//cout << "create table right" << endl;
	//test create start 
	//test create end----success
}
void Intepreter::create_index_sql(string indexname, string tablename, string attname)//create index
{
	my_api.api_create_index(indexname, tablename, attname);
	//cout << "create index right" << endl;
	//test create index start
	//test create index end ------success
	//cout << "create index success" << endl;
}
