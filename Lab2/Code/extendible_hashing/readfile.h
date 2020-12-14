/*
 * readfile.h
 *
 *  Created on: Sep 20, 2020
 *      Authors: Ali Khalid & Mohamed Ramzy & Evram Yousef
 */

#ifndef READFILE_H_
#define READFILE_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>     
#include <iterator> 
#include <map> 
#include <vector>
#include <iostream>

using namespace std;
//Global Variables:
   //M: Bucket size.
   //Map of Directories <value, pointer> //max size is 2^k, where k is the number of bits of the hash function
      //value: 000,001...etc ARE KEYS IN MAPS MUTABLE
      //pointer: points to certain bucket
   //Hash funtion: K mod 32 .. like sheet, K is the key
   //Global Depth = log_base_2 (Vector_size) ... Should we keep it? I don't think so.
   //Buckets struct {local_depth, array of values: array cuz it's fixed in size M}
   

// Algorithm for insertion:
   //Inserting value with key K
   //get the binary (K mod 32) 
   //check the Global Depth size (GD) and fetch the Least Significant (GD) bits from that hash.
   //navigate to the corresponding bucket.
   //if bucket is overflowed
      //if local depth < global depth -> split bucket AND Rehash
      //if local depth = global depth -> split bucket AND double the directory AND Rehash


#define M 3                                        //size of each bucket, when exceeding it (overflow), directory is doubled.
#define hash_value 32                              //used in hash function, to map the key of the data
#define MAX_DIRECT_SIZE hash_value                 //we can't have more than that; cuz K mod 32 only results in [32] values.
#define K (int)log2(hash_value)                    //maximum number of bits in directory, ie. directory cant exceed 2^32
#define DATASIZE sizeof(DataItem)                  //Size of Data
#define DIRECTORY_SIZE (hash_value* 8)             //Directory size = max of 32 places * [pointer size = 8]
#define BUCKETS_SIZE (hash_value*sizeof(Bucket))   //obv.
#define FILESIZE  BUCKETS_SIZE +  DIRECTORY_SIZE   //File Size


//holds data item with integer data and a key
struct DataItem
{
   /* data */
   int data;
   int key;
};

//Struct for buckets, data is an array of size M
struct Bucket 
{
   vector<DataItem*> data_array;   //Max size must stay at M
   int local_depth;
};


//Check the create File
int createFile(int size, char *, bool* exist);   //from old part

//check the chaining File
int insertItem(int fd, DataItem &item, vector<Bucket*> & Directory);     //tested clean
int deleteItem(int filehandle, int key,vector<Bucket*> & Directory);    //not tested yet
int searchItem(int filehandle, int key,vector<Bucket*> & Directory);    //not tested yet
int DisplayFile(int fd);                                                //not tested yet
void WriteFile(int fd, vector<Bucket*> & Directory);
vector<int> binarize (int value);                                       //tested clean
int debinarize(vector<int> vec);                                        //tested clean
void init(int fd,vector<Bucket*> & Directory, bool exist);                     //not tested yet
void print_directory(vector<Bucket*>& Directory);                       //tested clean
#endif /* READFILE_H_ */
