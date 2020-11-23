#ifndef DATABASE_H
#define DATABASE_H

#include<iostream>
#include<vector>

using namespace std;
#define MAXATTRIBUTE 32
#define MAXINDEX 32

struct My_place {
	int buffer_num;// buffermanager
	int bolck_num;
	int tuple_offset;
};
class MyData{
public:
    int Flag;          //We use -2 to represent int type, -1 to represent float type, 0 ~ 255 to represent char type
	virtual ~MyData() {}
};

class MyInt: public MyData{
public:
    int value;
    MyInt(int x):value(x){
        Flag = -2;
    };
	virtual ~MyInt() {}
};

class MyFloat: public MyData{
public:
    float value;
    MyFloat(float x):value(x){
        Flag = -1;
    };
	virtual ~MyFloat() {}
};

class MyChar: public MyData{
public:
    string value;
    MyChar(string x):value(x){
        Flag = x.size();         //We use different Flag values to represent different length
    };
	virtual ~MyChar() {}
};

struct Attribute{
    int AttrNum;                           //Represent the number of attributes
    short Flag[MAXATTRIBUTE];              //Represent the flag of every attribute
    string AttributeName[MAXATTRIBUTE];    //Represent the name of every attribute
    bool IsUnique[MAXATTRIBUTE];           //Determing whether every attribute is unique
};

struct Index{
    int IndexNum;
    short Position[MAXINDEX];          //Represent the location of the index in the attribute
    string IndexName[MAXINDEX];
};


class MyTuper{
public:
    vector<MyData*> Data;

    MyTuper(){}
     
    MyTuper(const MyTuper& MT);
     
	~MyTuper();

    int TuperLength() const
    {
        return (int)Data.size();
    }                            //Represent the length of the data

    void AddData(MyData* NewData)
    {
        Data.push_back(NewData);
    }                             //Add a new data to the tuper.
    
    MyData* operator[](unsigned short i);
                                  //Return the pointer to the specified data item.    
    void DisplayTuper();
                                  //Display the data in the tuper
};
typedef enum {
	eq,  //equal
	leq, //less than or equal
	l,   //less than
	geq, //greater than or equal
	g,   //greater than
	neq  //not equal
}WHERE;

struct where {
	MyData* data;
	WHERE flag;
};

class MyTable{
    //friend class CataManager;

public:
    int BlockNum;                 //The number of blocks the table occupies
    string TableName;             //The name of the table
    Attribute attributes;         //Attributes in table
    vector<MyTuper*> tupers;      //Tupers in table
    Index indices;                //Indices in table
    int PrimaryLocation;          //The location of primary key, -1 represent that there is no promary key

    int OneTuperSize()
    {
        int size = 0;
        for(int i = 0;i < attributes.AttrNum;i++)
        {
            if(attributes.Flag[i] == -2){
                size += sizeof(int);
            }
            else if(attributes.Flag[i] == -1)
            {
                size += sizeof(float);
            }
            else
            {
                size += attributes.Flag[i] + 1;   //Type of char, we need a additional one bit to store '\0'
            }            
        }
        return size;
    }

    MyTable(string s, Attribute attribute, int blocknum):TableName(s),attributes(attribute),BlockNum(blocknum)
    {
        PrimaryLocation = -1;
        for(int i = attributes.AttrNum;i < MAXATTRIBUTE;i++)
        {
            attribute.IsUnique[i] = false;
        }
        indices.IndexNum = 0;

    }

    MyTable(const MyTable& MT);
    
    ~MyTable();
   
    Attribute GetAttribute()
    {
        return attributes;
    }
    
    void SetIndex(short i, string indexname);
    
    void DropIndex(string indexname);
    
    void CopyIndex(Index index)
    {
        indices = index;
    }
    
    Index GetIndex()
    {
        return indices;
    }
    
    void SetPrimaryKey(int location)
    {
        PrimaryLocation = location;
    }                                      //Set the primary key
    
    void DisplayTable();
    
    string GetTableName()
    {
        return TableName;
    }
    
    int GetAttributeSize() const
    {
        return attributes.AttrNum;
    }
    
    int GetTuperSize() const
    {
        return (int)tupers.size();
    }
    
    void AddTuper(MyTuper* MT);
	void SubTuper(WHERE Tempwhere, int Attriindex, MyInt* TempMyInt, int delete_tuple);
	void SubTuper(WHERE Tempwhere, int Attriindex, MyFloat* TempMyFloat, int delete_tuple);
	void SubTuper(WHERE Tempwhere, int Attriindex, MyChar* TempMyChar, int delete_tuple);
	//�д�����
};




class TableException: public exception{
public:
    TableException(string s):Message(s){}
    
    string DisplayMessage()
    {
        return Message;
    }

private:
    string Message;
};

#endif

