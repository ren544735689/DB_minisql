#include"BufferManager.h"
#include<iostream>
#include<fstream>
using namespace std;

BufferManager::BufferManager()
{
    for(int i = 0;i < BLOCKNUM;i++)
    {
        buffer[i].BufferInitialize();
    }
}

BufferManager::~BufferManager()
{
    for(int i = 0;i < BLOCKNUM;i++)
    {
        WriteBlock_to_File(i);
    }
}

void BufferManager::WriteBlock_to_File(int BlockNum)
{
    if(buffer[BlockNum].IsWritten == 0)  return;                      //which means the block has not been written in,then we don't need any operations

    string filename = buffer[BlockNum].filename;
    FILE* fp;
    fp = fopen(filename.c_str(), "w");
    if(fp == NULL)
    {
        cout << "Can't open the file!" << endl;
        return;
    }
    fseek(fp, BLOCKSIZE * buffer[BlockNum].BlockOffset,SEEK_SET);     //Get the position in the file
    fwrite(buffer[BlockNum].BlockContent,BLOCKSIZE,1,fp);             //Write the content of the block to the file
    buffer[BlockNum].BufferInitialize();                              //Once we have written the block back to file, we initialize the block
    fclose(fp);
}

int BufferManager::GetBufferNum(string filename, int BlockOffset)
{
    int BufferNum = Index_in_Buffer(filename, BlockOffset);
	if(BufferNum == -1)
    {
		BufferNum = GetEmptyBufferExcept(filename);
		ReadBlock(filename, BlockOffset, BufferNum);
	}
	return BufferNum;
}

void BufferManager::ReadBlock(string filename,int BlockOffset,int BlockNum)
{
    buffer[BlockNum].IsValid = 1;
    buffer[BlockNum].IsWritten = 0;
    buffer[BlockNum].filename = filename;
    buffer[BlockNum].BlockOffset = BlockOffset;
    buffer[BlockNum].VisitTime = clock();

    FILE *fp;
    fp = fopen(filename.c_str(),"r");
    if(fp == NULL)
    {
		throw TableException("Table may not exist");
        cout << "Can't open the file!" << endl;
        return;
    }
    fseek(fp, BLOCKSIZE *BlockOffset,SEEK_SET);
    fread(buffer[BlockNum].BlockContent,BLOCKSIZE,1,fp);
    fclose(fp);
}

void BufferManager::WriteBlock(int BlockNum)
{
    buffer[BlockNum].IsWritten = 1;
    buffer[BlockNum].VisitTime = clock();
}

int BufferManager::GetEmptyBuffer()
{
    int Least_Recent_Visited = 0;
    for(int i = 0;i < BLOCKNUM;i++)
    {
        if(buffer[i].IsValid == 0)          //If the block is empty, get its index
        {
            buffer[i].BufferInitialize();
            buffer[i].IsValid = 1;
            return i;
        }
        else                                //Else get the index of the least_recent_visited one
        {
            if(buffer[Least_Recent_Visited].VisitTime > buffer[i].VisitTime) Least_Recent_Visited = i;
        }
        
    }
    WriteBlock_to_File(Least_Recent_Visited);
    buffer[Least_Recent_Visited].IsValid = 1;
    return Least_Recent_Visited;
}

int BufferManager::GetEmptyBufferExcept(string filename)
{
    int Least_Recent_Visited = 0;
    for(int i = 0;i < BLOCKNUM;i++)
    {
        if(buffer[i].IsValid == 0)          //If the block is empty, get its index
        {
            buffer[i].BufferInitialize();
            buffer[i].IsValid = 1;
            return i;
        }
        else                                //Else get the index of the least_recent_visited one
        {
            if(buffer[Least_Recent_Visited].VisitTime > buffer[i].VisitTime && buffer[i].filename != filename) Least_Recent_Visited = i;
        }
        
    }
    WriteBlock_to_File(Least_Recent_Visited);
    buffer[Least_Recent_Visited].IsValid = 1;
    return Least_Recent_Visited;
}

int BufferManager::AddNewBlock(MyTable& table)
{
    int BufferNum = GetEmptyBuffer();
	buffer[BufferNum].BufferInitialize();
	buffer[BufferNum].IsValid = 1;
	buffer[BufferNum].IsWritten = 1;
	buffer[BufferNum].filename = table.GetTableName() + ".table";
	buffer[BufferNum].BlockOffset = table.BlockNum++;
	buffer[BufferNum].VisitTime = clock();
	//CataManager ca;
	//ca.changeblock(table.GetTableName(), table.BlockNum);
	return BufferNum;
}

int BufferManager::Index_in_Buffer(string filename,int BlockOffset)
{
    for(int i = 0;i < BLOCKNUM;i++)
    {
        if(buffer[i].filename == filename && buffer[i].BlockOffset == BlockOffset) return i;    
    }
    return -1;
}

void BufferManager::ScanWholeFile(MyTable& table)
{
    string filename = table.GetTableName() + ".table";
	fstream fin(filename.c_str(), ios::in);
	for(int i = 0; i < table.BlockNum; i++)
    {
		if(Index_in_Buffer(filename, i) == -1)
        {
			int index = GetEmptyBufferExcept(filename);
			ReadBlock(filename, i, index);
		}
	}
	fin.close();
}

void BufferManager::SetInvalid(string filename)
{
	for(int i = 0; i < BLOCKNUM;i++)
    {
		if(buffer[i].filename == filename)
        {
			buffer[i].IsValid = 0;
			buffer[i].IsWritten = 0;
		}
	}
}

int BufferManager::AllocateBlock(string filename, int BlockOffset)
{
	int BufferNum = Index_in_Buffer(filename, BlockOffset);
	if(BufferNum == -1)
    {
		BufferNum = GetEmptyBuffer();
		ReadBlock(filename, BlockOffset, BufferNum);
	}
	buffer[BufferNum].VisitTime = clock();
	return BufferNum;
}

InsertPosition BufferManager::GetInsertPosition(MyTable& table){
	InsertPosition MyPosition;
	if (table.BlockNum == 0)         //A new file
    { 
		MyPosition.BufferNum = AddNewBlock(table);
		MyPosition.Position = 0;
		return MyPosition;
	}

	string filename = table.GetTableName() + ".table";
	int length;
	length = table.OneTuperSize() + 1;          //One extra bit is placed at the beginning to indicate whether it is valid
	int BlockOffset = table.BlockNum - 1;         //New element is inserted at the end
	int BufferNum = Index_in_Buffer(filename, BlockOffset);
	
    if(BufferNum == -1)
    {
		BufferNum = GetEmptyBuffer();             //Get an empty block
		ReadBlock(filename, BlockOffset, BufferNum);
	}
	
    int RowNum = BLOCKSIZE / length;
	for(int i = 0; i < RowNum;i++)
    {
		int Position = i * length;
		char IsEmpty = buffer[BufferNum].BlockContent[Position];             //Check whether the first bit is valid to determine whether one row is empty or not
		if(IsEmpty == '\0')
        {                                                                    //find an empty space
			MyPosition.BufferNum = BufferNum;
			MyPosition.Position = Position;
			return MyPosition;
		}
	}
	                                                                        //The block is full, we need to add a new block
	MyPosition.BufferNum = AddNewBlock(table);
	MyPosition.Position = 0;
	return MyPosition;
}
