#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include<iostream>
#include<fstream>
#include<ctime>
#include"DataBase.h"

using namespace std;

#define BLOCKSIZE 4096
#define BLOCKNUM 100

struct InsertPosition{
    int BufferNum;       //The block number in memory
    int Position;        //The position in the block
};

class MyBuffer{
public:
    string filename;    //Corresponding filename
    bool IsWritten;     //Whether the block has been written
    bool IsValid;       //Whether the block is valid
    int BlockOffset;    //The blockoffset in file,determing its position
    int VisitTime;      //Show the recent visit time
    int LRU;            //We use LRU principle to determine which block will be substituted
    char BlockContent[BLOCKSIZE + 1]; //Used to save the content of the block

    MyBuffer()
    {
        BufferInitialize();
    }

    void BufferInitialize()
    {
        IsWritten = 0;
        IsValid = 0;
        filename = "NULL";
        BlockOffset = 0;
        VisitTime = 0;
        LRU = 0;
        memset(BlockContent,0,BLOCKSIZE);
        BlockContent[BLOCKSIZE] = '\0';
    }

    string GetBlockContent(int start_pos,int end_pos)             //Get the content in the block from start_pos to end_pos
    {
        string content = "";
        if(start_pos >= 0 && end_pos <= BLOCKSIZE && start_pos <= end_pos)
        {
            for(int i = start_pos;i < end_pos;i++)
            {
                content += BlockContent[i];
            }
        }
        return content;
    }

    char GetDesignatedBlockContent(int pos)
    {
        if(pos >= 0 && pos <= BLOCKSIZE)
        {
            return BlockContent[pos];
        }
        return '\0';
    }
};

class BufferManager{
public:
    //friend class RecordManager;
	//friend class CataManager;
    
    MyBuffer buffer[BLOCKNUM];

    BufferManager();

	~BufferManager();

	void WriteBlock_to_File(int BufferNum);                 //Write the content of block back to file,and initialize the block
	
    int GetBufferNum(string filename, int BlockOffset);     //Get the index of the designated block in memory
	
    void ReadBlock(string filename, int BlockOffset, int BufferNum);     //Read the file into block
	
    void WriteBlock(int BufferNum);                         //We use it when the block is rewritten, and we symbolize it
	
    int GetEmptyBuffer();                                   //Get the empty buffer block in memory
	
    int GetEmptyBufferExcept(string filename);              //Get the empty buffer block in memory without substitute the designated file
	
    InsertPosition GetInsertPosition(MyTable& table);         //Return the available position of inserting data
	
    int AddNewBlock(MyTable& table);                        //After inserting a new table file,return the updated index of block in memory

	int Index_in_Buffer(string filename, int BlockOffset);  //Get the index of one block in memory, return -1 if not find the block
	
    void ScanWholeFile(MyTable& table);                      //Read the whole table file into memory(if the file is to large, it may cause collapse)
	
    void SetInvalid(string filename);                       //Set the block of the file to invalid, used for deleting table and index
	
    int AllocateBlock(string filename, int BlockOffset);    //External function call interface

};

#endif
