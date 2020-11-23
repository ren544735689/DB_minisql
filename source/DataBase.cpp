#include<iostream>
#include<string>
#include"DataBase.h"


MyTuper::MyTuper(const MyTuper& MT)
{
    MyData* data;
    for(int i = 0;i < MT.TuperLength();i++)
    {
        if(MT.Data[i]->Flag == -2)          //int type
        {
            data = new MyInt(((MyInt*)(MT.Data[i]))->value);
            this->AddData(data);
        }
        else if(MT.Data[i]->Flag == -1)    //float type
        {
            data = new MyFloat(((MyFloat*)(MT.Data[i]))->value);
            this->AddData(data);
        }
        else                               //char type
        {
            data = new MyChar(((MyChar*)(MT.Data[i]))->value);
            this->AddData(data);
        }
    }
}

MyTuper::~MyTuper()
{
    for(int i = 0;i < Data.size();i++)
    {
        if(Data[i] == NULL) continue;
        if(Data[i]->Flag == -2)
        {
            delete (MyInt*)Data[i];
        }
        else if(Data[i]->Flag == -1)
        {
            delete (MyFloat*)Data[i];
        }
        else
        {
            delete (MyChar*)Data[i];
        }
    }
}

void MyTuper::DisplayTuper()
{
    for(int i = 0;i < Data.size();i++)
    {
        if(Data[i] == NULL)
        {
            cout << "NULL" << "\t";
        }
        else if(Data[i]->Flag == -2)
        {
            cout << ((MyInt*)Data[i])->value << "\t";
        }
        else if(Data[i]->Flag == -1)
        {
            cout << ((MyFloat*)Data[i])->value << "\t";
        }
        else
        {
            cout << ((MyChar*)Data[i])->value << "\t";
        }
        cout << endl << endl;
        
    }
}

MyData* MyTuper::operator[](unsigned short i)
{
    if(i >= Data.size()) throw out_of_range("Out of range!");
    return Data[i];
}

MyTable::MyTable(const MyTable& MT)
{
    this->attributes = MT.attributes;
    this->TableName = MT.TableName;
    this->PrimaryLocation = MT.PrimaryLocation;
    this->indices = MT.indices;
    this->BlockNum = MT.BlockNum;
    
    for(int i = 0;i < MT.GetAttributeSize();i++)
    {
        MyTuper NewMT =*( new MyTuper(*(MT.tupers[i])));
        this->AddTuper(&NewMT);
    }
}

MyTable::~MyTable()
{
    for(int i = 0;i < GetTuperSize();i++)
    {
        delete tupers[i];
    }
}

void MyTable::AddTuper(MyTuper* MT)
{
    if(MT->TuperLength() != attributes.AttrNum) throw TableException("Illegal Tuper Inserted: unequal column size!");
    //the tuper we want to add to the table has a different number of attributes
	tupers.push_back(MT);
}
void MyTable::SubTuper(WHERE Tempwhere, int Attriindex, MyInt* TempMyInt, int delete_tuple)
{
	int i=0;
	for (auto iter = tupers.begin(); iter != tupers.end();i++) {
		switch (Tempwhere) {
		case 0://eq,  //equal
			if (((MyInt *)(tupers[i]->Data[Attriindex]))->value == TempMyInt->value) {
				iter=tupers.erase(iter);
			}
			else iter++;
			break;
		case 1://leq, //less than or equal
			if (((MyInt *)(tupers[i]->Data[Attriindex]))->value <= TempMyInt->value) {
				iter = tupers.erase(iter);
			}
			else iter++;
			break;
		case 2://l,   //less than
			if (((MyInt *)(tupers[i]->Data[Attriindex]))->value < TempMyInt->value) {
				iter = tupers.erase(iter);
			}
			else iter++;
			break;
		case 3://geq, //greater than or equal
			if (((MyInt *)(tupers[i]->Data[Attriindex]))->value >= TempMyInt->value) {
				iter = tupers.erase(iter);
			}
			else iter++;
			break;
		case 4://g,   //greater than
			if (((MyInt *)(tupers[i]->Data[Attriindex]))->value > TempMyInt->value) {
				iter = tupers.erase(iter);
			}
			else iter++;
			break;
		case 5://neq  //not equal
			if (((MyInt *)(tupers[i]->Data[Attriindex]))->value != TempMyInt->value) {
				iter = tupers.erase(iter);
			}
			else iter++;
			break;
		}
	}
}
void MyTable::SubTuper(WHERE Tempwhere, int Attriindex, MyFloat* TempMyFloat, int delete_tuple) {
	int i=0;
	for (auto iter = tupers.begin(); iter != tupers.end();i++) {
		switch (Tempwhere) {
		case 0://eq,  //equal
			if (((MyFloat *)(tupers[i]->Data[Attriindex]))->value == TempMyFloat->value) {
				iter = tupers.erase(iter);
			}
			else iter++;
			break;
		case 1://leq, //less than or equal
			if (((MyFloat *)(tupers[i]->Data[Attriindex]))->value <= TempMyFloat->value) {
				iter = tupers.erase(iter);
			}
			else iter++;
			break;
		case 2://l,   //less than
			if (((MyFloat *)(tupers[i]->Data[Attriindex]))->value < TempMyFloat->value) {
				iter = tupers.erase(iter);
			}
			else iter++;
			break;
		case 3://geq, //greater than or equal
			if (((MyFloat *)(tupers[i]->Data[Attriindex]))->value >= TempMyFloat->value) {
				iter = tupers.erase(iter);
			}
			else iter++;
			break;
		case 4://g,   //greater than
			if (((MyFloat *)(tupers[i]->Data[Attriindex]))->value > TempMyFloat->value) {
				iter = tupers.erase(iter);
			}
			else iter++;
			break;
		case 5://neq  //not equal
			if (((MyFloat *)(tupers[i]->Data[Attriindex]))->value != TempMyFloat->value) {
				iter = tupers.erase(iter);
			}
			else iter++;
			break;
		}
	}
}
void MyTable::SubTuper(WHERE Tempwhere, int Attriindex, MyChar* TempMyChar, int delete_tuple) {
	int i=0;
	for (auto iter = tupers.begin(); iter != tupers.end();i++) {
		switch (Tempwhere) {
		case 0://eq,  //equal
			if (((MyChar*)(tupers[i]->Data[Attriindex]))->value == TempMyChar->value) {
				iter = tupers.erase(iter);
			}
			else iter++;
			break;
		case 1://leq, //less than or equal
			if (((MyChar *)(tupers[i]->Data[Attriindex]))->value <= TempMyChar->value) {
				iter = tupers.erase(iter);
			}
			else iter++;
			break;
		case 2://l,   //less than
			if (((MyChar *)(tupers[i]->Data[Attriindex]))->value < TempMyChar->value) {
				iter = tupers.erase(iter);
			}
			else iter++;
			break;
		case 3://geq, //greater than or equal
			if (((MyChar *)(tupers[i]->Data[Attriindex]))->value >= TempMyChar->value) {
				iter = tupers.erase(iter);
			}
			else iter++;
			break;
		case 4://g,   //greater than
			if (((MyChar *)(tupers[i]->Data[Attriindex]))->value > TempMyChar->value) {
				iter = tupers.erase(iter);
			}
			else iter++;
			break;
		case 5://neq  //not equal
			if (((MyChar *)(tupers[i]->Data[Attriindex]))->value != TempMyChar->value) {
				iter = tupers.erase(iter);
			}
			else iter++;
			break;
		}
	}
}
void MyTable::DisplayTable()
{
    for(int i = 0;i < attributes.AttrNum;i++)
    {
        cout << attributes.AttributeName[i] << "\t";
    }
    cout << endl << endl;
    for(int i = 0;i < tupers.size();i++)
    {
        tupers[i]->DisplayTuper();
    }
}

void MyTable::SetIndex(short i, string indexname)
{
    short j;
    for(j = 0;j < indices.IndexNum;j++)
    {
        if(i == indices.Position[j]) break;
    }
    if(j < indices.IndexNum) throw TableException("Index position already exists!");

    for(j = 0;j < indices.IndexNum;j++)
    {
        if(indexname == indices.IndexName[j]) break;
    }
    if(j < indices.IndexNum) throw TableException("Index name has been used!");

    indices.IndexName[indices.IndexNum] = indexname;
    indices.Position[indices.IndexNum] = i;
    indices.IndexNum++;
}

void MyTable::DropIndex(string indexname)
{
    short j;
    for(j = 0;j < indices.IndexNum;j++)
    {
        if(indexname == indices.IndexName[j]) break;
    }
    if(j == indices.IndexNum) throw TableException("There is no index named " + indexname + "on " + TableName + "!");

    //Always use the last index to substitute the designated index
    string tempname;
    short tempindex;

    tempname = indices.IndexName[j];
    indices.IndexName[j] = indices.IndexName[indices.IndexNum - 1];
    indices.IndexName[indices.IndexNum - 1] = tempname;

    tempindex = indices.IndexNum;
    indices.Position[j] = indices.Position[indices.IndexNum - 1];
    indices.Position[indices.IndexNum - 1] = tempindex;

    indices.IndexNum--;
}


