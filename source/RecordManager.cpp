#include"RecordManager.h"
#include <iomanip>

template<class T>
int compare_int_float(WHERE tempwhere, T A, T B) {
	switch (tempwhere) {
	case 0://eq,  //equal
		if (A==B) {
			return 1;
		}
		break;
	case 1://leq, //less than or equal
		if (A<=B) {
			return 1;
		}
		break;
	case 2://l,   //less than
		if (A<B) {
			return 1;
		}
		break;
	case 3://geq, //greater than or equal
		if (A>=B) {
			return 1;
		}
		break;
	case 4://g,   //greater than
		if (A>B) {
			return 1;
		}
		break;
	case 5://neq  //not equal
		if (A!=B) {
			return 1;
		}
		break;
	}
	return 0;
}
int RecordManager::compare(WHERE tempwhere, char A[], char B[],int size) {
	
	switch (tempwhere) {
	case 0://eq,  //equal
		if (cmp_of_mine(A,B,size)==0) {
			return 1;
		}
		break;
	case 1://leq, //less than or equal
		if (cmp_of_mine(A, B, size )<=0 ) {
			return 1;
		}
		break;
	case 2://l,   //less than
		if (cmp_of_mine(A, B, size) <0) {
			return 1;
		}
		break;
	case 3://geq, //greater than or equal
		if (cmp_of_mine(A, B, size) >=0) {
			return 1;
		}
		break;
	case 4://g,   //greater than
		if (cmp_of_mine(A, B, size) >0) {
			return 1;
		}
		break;
	case 5://neq  //not equal
		if (cmp_of_mine(A, B, size)!=0) {
			return 1;
		}
		break;
	}
	return 0;
}
int RecordManager::get_blocknum(string tablename) {
	string str_test = "";
	int buffernum;
	string filename = tablename + "_head.table";

	buffernum = m_Buffer_manager->GetBufferNum(filename, 0);
	str_test = m_Buffer_manager->buffer[buffernum].BlockContent;
	int i = 0;
	int j = 0;
	j = str_test.find_first_of('\n');
	string test = "";
	while (i < j) {
		if (str_test.at(i) != ' ') test += str_test.at(i);
		i++;
	}
	return stoi(test);
}
int RecordManager::get_Attrinum(string tablename) {
	string str_test = "";
	int buffernum;
	string filename = tablename + "_head.txt";

	buffernum = m_Buffer_manager->GetBufferNum(filename, 0);
	str_test = m_Buffer_manager->buffer[buffernum].BlockContent;
	int i = 0;
	int j = 0;
	j = str_test.find_first_of('\n');
	string test = "";
	j++;
	while (str_test.at(j) != '\n') {
		test += str_test.at(j);
		j++;
	}
	return stoi(test);
}
void RecordManager::insert_tuple_table(MyTable& temp_table, MyTuper& temp_tuper) {
	//temp_table.AddTuper(&temp_tuper);//add in temp_table;
	
	//check primary or unique
	int id;
	for (id = 0; id < temp_table.attributes.AttrNum; id++) {
		if (temp_table.attributes.IsUnique[id] == true) {
			int index_id;
			for (index_id = 0; index_id < temp_table.indices.IndexNum; index_id++) {
				if (temp_table.indices.Position[index_id] == id) {
					break;
				}
			}
			if (index_id == temp_table.indices.IndexNum) { //unique but no index to check ;
				try {
					check(temp_table, temp_tuper);
				}
				catch (TableException exc) {
					throw exc;
					return;
				}
			}
		}
	}
	
	//check end
	int tuple_size = temp_table.OneTuperSize() + 1;
	InsertPosition temp_ins;
	temp_ins = m_Buffer_manager->GetInsertPosition(temp_table);
	int size = temp_table.OneTuperSize() + 1;
	int j = 0;
	string str_test = "";
	char str_my[500] = {};
	int str_len = 0;
	int k = temp_table.attributes.AttrNum;
	/////tuple_to_string
	while (j < k) {
		if (temp_table.attributes.Flag[j] == -2) {//int
			MyInt* mint = (MyInt*)((temp_tuper.Data[j]));
			char p[5] = {};
		    int n = mint->value;
			p[0] = (n >> 24) & 0xFF;
			p[1] = (n >> 16) & 0xFF;
			p[2] = (n >> 8) & 0xFF;
			p[3] = n & 0xFF;
			p[4] = '\0';
			
			int ii = 0;
			while (ii < 4) {
				str_my[str_len++] = p[ii];
				ii++;
			}
			//strcat(str_my, p);
		}
		else if (temp_table.attributes.Flag[j] == -1) {//float
			MyFloat* mint = (MyFloat*)((temp_tuper.Data[j]));
			unsigned char p[5] = {};
			float n = mint->value;
	
			memcpy(p, &n, 4);
			int ii = 0;
			while (ii < 4) {
				str_my[str_len++] = p[ii];
				ii++;
			}
		}
		else if (temp_table.attributes.Flag[j] > 0) {//char()
			MyChar* mint = (MyChar*)((temp_tuper.Data[j]));

			strcat(str_my+str_len, mint->value.c_str());
		}
		//str_my += " ";
		j++;
	}
	//str_my += "\n";
	////tuple_to_string
	//int tuple_size = temp_table.OneTuperSize() + 1;
	int i= temp_ins.Position;
	m_Buffer_manager->buffer[temp_ins.BufferNum].BlockContent[i] = '\1';
	memcpy(m_Buffer_manager->buffer[temp_ins.BufferNum].BlockContent + i+1, str_my, tuple_size-1);
	m_Buffer_manager->buffer[temp_ins.BufferNum].IsWritten = 1;
	
	//give the index module
	the_place.buffer_num = temp_ins.BufferNum;
	the_place.tuple_offset = temp_ins.Position;
	the_place.bolck_num = m_Buffer_manager->buffer[temp_ins.BufferNum].BlockOffset;

	//write back;
	int buffernum = m_Buffer_manager->GetBufferNum(temp_table.TableName+"_head.table",0);
	string thecontent = get_table_define(temp_table);
	strcpy(m_Buffer_manager->buffer[buffernum].BlockContent, thecontent.c_str());
	m_Buffer_manager->buffer[buffernum].IsWritten = true;
}
void RecordManager::create_table(MyTable& temp_table) {//table define
	int temp_buffernum;
	string filename = "";
	filename = temp_table.TableName + ".txt";
	temp_buffernum = m_Buffer_manager->AllocateBlock(filename, 0);
	//allocateblock a space for a new table named temp_table.TableName
	//this is a table define
	m_Buffer_manager->WriteBlock(temp_buffernum);
	//write some table define information
	m_Buffer_manager->buffer[temp_buffernum].filename = filename;
	m_Buffer_manager->buffer[temp_buffernum].IsWritten = 1;
	m_Buffer_manager->buffer[temp_buffernum].IsValid = 1;
	m_Buffer_manager->buffer[temp_buffernum].BlockOffset = 0;

	memset(m_Buffer_manager->buffer[temp_buffernum].BlockContent, 0, BLOCKSIZE);
	m_Buffer_manager->WriteBlock(temp_buffernum);
}
void RecordManager::delete_from_table(string tablename,int blocknum) {
	int i = 0;
	string filename = tablename + ".table";
	int buffernum;
	while (i < blocknum) {
		buffernum = m_Buffer_manager->GetBufferNum(filename, i);
		memset(m_Buffer_manager->buffer[buffernum].BlockContent, 0, BLOCKSIZE);
		m_Buffer_manager->buffer[buffernum].IsWritten = true;

		i++;
	}


}

void RecordManager::select_or_delete_from_table(bool select,MyTable& test_table, WHERE tempwhere, string value, string Attriname) {
	My_Attri temp_Attri;
	int i;
	int index_attr = 0;
	int tuple_size = test_table.OneTuperSize() + 1;

	for (i = 0; i < test_table.attributes.AttrNum; i++) {
		if (Attriname == test_table.attributes.AttributeName[i]) {
			temp_Attri.flag = test_table.attributes.Flag[i];
			temp_Attri.index = i;
			break;
		}
		if (test_table.attributes.Flag[i] >0) {
			index_attr += test_table.attributes.Flag[i];
		}
		else {
			index_attr += 4;
		}
	}
	if (i == test_table.attributes.AttrNum) {
		throw TableException("there is no attribute named " + Attriname + " ont the table named " + test_table.TableName);
		return;
	}
	if (select == 1) {
		i = 0;
		while (i < test_table.attributes.AttrNum) {
			if (test_table.attributes.Flag[i] < 0) cout << left << setw(10);
			else cout << left << setw(test_table.attributes.Flag[i] + 2);
			cout << test_table.attributes.AttributeName[i];
			i++;
		}
		cout << endl;
	}
	int myint;
	float myfloat;
	char int_float[35] = {};
	char* cmp_char;
	int sign = 0;
	if (temp_Attri.flag == -2) {
		myint = stoi(value);
		int_float[0] = (myint >> 24) & 0xFF;
		int_float[1] = (myint >> 16) & 0xFF;
		int_float[2] = (myint >> 8) & 0xFF;
		int_float[3] = myint & 0xFF;
		cmp_char = int_float;
		sign = 4;
	}
	else if (temp_Attri.flag == -1) {
		myfloat= stof(value);
		memcpy(int_float, &myfloat, 4);
		cmp_char = int_float;
		sign = 4;

	}
	else {
		//cmp_char = value.c_str();
	
		strcpy(int_float, value.c_str());
		cmp_char = int_float;
		sign = temp_Attri.flag;
	}
	int block_num = test_table.BlockNum;//get the number of blocks
	int buffernum;
	i = 0;
	int j = 0;
	
	int k = 0;
	string filename = test_table.TableName + ".table";
	while (j < block_num) {
		buffernum = m_Buffer_manager->GetBufferNum(filename, j);
		j++;
		int RowNum = BLOCKSIZE / tuple_size;
		char my_char[35] = {};
		for (i = 0; i < RowNum; i++)
		{
			int Position = i * tuple_size;
			char IsEmpty = m_Buffer_manager->buffer[buffernum].BlockContent[Position];             //Check whether the first bit is valid to determine whether one row is empty or not
			if (IsEmpty == '\0')                                                                    //find an empty space
				continue;
			else {
				memcpy(my_char, m_Buffer_manager->buffer[buffernum].BlockContent + Position + index_attr + 1, sign);			
				int condition;
				if (temp_Attri.flag == -2) {
					int p;
					p = (unsigned char)my_char[0];
					p << 8;
					p += (unsigned char)my_char[1];
					p << 8;
					p += (unsigned char)my_char[2];
					p << 8;
					p += (unsigned char)my_char[3];
					condition = compare_int_float(tempwhere, p, myint);
				}
				else if (temp_Attri.flag == -1) {
					float p;
					char pp[4];
					memcpy(pp, my_char, 4);
					p = *(float*)pp;
					condition = compare_int_float(tempwhere, p, myfloat);
				}
				else if (temp_Attri.flag > 0) condition = compare(tempwhere, my_char, cmp_char, sign);
				if (condition) {
					if (select == 1) {
						print(m_Buffer_manager->buffer[buffernum].BlockContent + Position + 1, test_table.attributes);
					}
					else{
						
						memset(m_Buffer_manager->buffer[buffernum].BlockContent + Position,0, tuple_size);
						
					}
				}
				else continue;
				
			}
		}
	}
}
void RecordManager::serach_in_table(string tablename,MyTable& temp_table) {
	
	//all output
	int tuple_size = temp_table.OneTuperSize() + 1;
	int block_num = get_blocknum(tablename);//get the number of blocks
	int buffernum;

	int j = 0;
	
	int k = 0;
	string filename = tablename + ".table";
	char my_char[500] = {};
	int i = 0;
	int row = 0;
	row = BLOCKSIZE / tuple_size;
	cout << " ";
	while (i < temp_table.attributes.AttrNum) {
		if (temp_table.attributes.Flag[i] < 0) cout << left << setw(10);
		else cout << left << setw(temp_table.attributes.Flag[i] + 2);
		cout << temp_table.attributes.AttributeName[i];
		i++;
	}
	cout << endl;
	i = 0;
	while (j < block_num) {
		buffernum = m_Buffer_manager->GetBufferNum(filename, j);
		j++;
		//cout << m_Buffer_manager->buffer[buffernum].BlockContent;
		
		for (i = 0; i < row; i++) {
			memcpy(my_char, m_Buffer_manager->buffer[buffernum].BlockContent + i*tuple_size, tuple_size);
			if (my_char[0]!='\0') print(my_char+1, temp_table.attributes);
		}
		
	}
}
int RecordManager::cmp_of_mine(char A[], char B[],int size) {
	int i=0;
	int len = size;
	for (i = 0; i < len; i++) {
		
		if (A[i] > B[i]) return 1;
		else if (A[i] < B[i]) return -1;
	}
	return 0;
}
void RecordManager::print(char test_char[], Attribute& test_Attri) {
	int i = 0;
	int t= 0;
	
	while (i < test_Attri.AttrNum) {
		cout << "|";
		if (test_Attri.Flag[i] == -2) {
			int p;
			p = test_char[t]&0xFF;
			p = p*256;
			p += test_char[t+1]&0xFF;
			p =p * 256;
			p+= test_char[t + 2] & 0xFF;
			p =p * 256;
			p +=test_char[t + 3] & 0xFF;
			
			int pp = p;
			//memcpy(&p, test_char, 4);
			/*char pp[4] = {};
			memcpy(pp, test_char + t, 4);
			p = atoi(pp);*/
			t += 4;
			cout << left << setw(9)<<pp;
		}
		else if(test_Attri.Flag[i] == -1) {
			float p;
			char pp[4];
			memcpy(pp, test_char + t, 4);
			p = *(float*)pp;

			t += 4;
			cout <<setprecision(6)<< left << setw(9) << p ;
		}
		else if (test_Attri.Flag[i] > 0){
			char p[20] = {};
			memcpy(p, test_char + t, test_Attri.Flag[i]);
			cout << p <<" ";
			t += test_Attri.Flag[i];

		}
		i++;
	}
	cout << endl;
}
void RecordManager::check(MyTable& temp_table, MyTuper& temp_tuper) {
	//check_uniqe_pri
	int i;
	int attri_index = 0;
	int buffernum;
	int block_num = temp_table.BlockNum;
	int tuple_size = temp_table.OneTuperSize() + 1;
	int sign = 0;
	int m = 0;
	for (m = 0; m < temp_table.attributes.AttrNum; m++) {
		if (temp_table.attributes.Flag[m] > 0) {
			sign = temp_table.attributes.Flag[m];
		}
		else {
			sign = 4;
		}
		//can not be the same
		if (m == temp_table.PrimaryLocation || temp_table.attributes.IsUnique[m] == true) {
			string filename = temp_table.TableName + ".table";
			int j = 0;
			while (j < block_num) {
				buffernum = m_Buffer_manager->GetBufferNum(filename, j);
				j++;
				int RowNum = BLOCKSIZE / tuple_size;
				char my_char[35] = {};
				for (i = 0; i < RowNum; i++)
				{
					int Position = i * tuple_size;
					char IsEmpty = m_Buffer_manager->buffer[buffernum].BlockContent[Position];             //Check whether the first bit is valid to determine whether one row is empty or not
					if (IsEmpty == '\0')                                                                    //find an empty space
						continue;
					else {
						memcpy(my_char, m_Buffer_manager->buffer[buffernum].BlockContent + Position + attri_index + 1, sign);
						int condition;
						if (temp_table.attributes.Flag[m] == -2) {
							int p;
							p = (unsigned char)my_char[0];
							p << 8;
							p += (unsigned char)my_char[1];
							p << 8;
							p += (unsigned char)my_char[2];
							p << 8;
							p += (unsigned char)my_char[3];
							condition = compare_int_float(eq, p, ((MyInt*)temp_tuper.Data[m])->value);
						}
						else if (temp_table.attributes.Flag[m] == -1) {
							float p;
							char pp[4];
							memcpy(pp, my_char, 4);
							p = *(float*)pp;
							condition = compare_int_float(eq, p, ((MyFloat*)temp_tuper.Data[m])->value);
						}
						else {
							char test_char[35] = {};
							strcpy(test_char, ((MyChar*)temp_tuper.Data[m])->value.c_str());
							condition = compare(eq, my_char, test_char, sign);
						}
						if (condition) {

							throw TableException("the attribtue at "+to_string(m) + "named " + temp_table.attributes.AttributeName[m] + "is a primary or unique key");
							return;
						}
						else continue;

					}
				}
			}
		}
		if (temp_table.attributes.Flag[m] > 0) 	attri_index += temp_table.attributes.Flag[i];
		else 	attri_index += 4;

	}
}
string RecordManager::get_table_define(MyTable& temp_table) {
	string content = "";
	content += to_string(temp_table.BlockNum);
	content += "\n";
	int attrinum = temp_table.attributes.AttrNum;
	content += to_string(attrinum) + "\n";
	int i = 0;
	while (i < attrinum) {
		content += temp_table.attributes.AttributeName[i] + " ";
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
		content += temp_table.indices.IndexName[i] + " ";
		content += to_string(temp_table.indices.Position[i]) + " ";
		content += "\n";
		i++;
	}
	return content;
}
