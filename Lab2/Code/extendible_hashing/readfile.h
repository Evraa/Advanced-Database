/*
 * readfile.h
 *
 *  Created on: Sep 20, 2016
 *      Author: dina
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


#define M 3                         //size of each bucket, when exceeding it (overflow), directory is doubled.
#define hash_value 32               //used in hash function, to map the key of the data
#define K (int)log2(hash_value)     //maximum number of bits in directory, ie. directory cant exceed 2^32
#define FILESIZE (hash_value*(1 + M*(8))) + (hash_value* ((K*4) + 4)) //File Size


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
   // DataItem* data = DataItem int[M]; //use this if the second one errored.
   DataItem data_array [M];     
   int local_depth;
};


//Check the create File
int createFile(int size, char *);

//check the chaining File
// int deleteItem(int data);
int insertItem(int fd, DataItem item, map<vector<int>,int>* Direct);
int deleteItem(int filehandle, int key,map<vector<int>,int>* Direct);
int searchItem(int filehandle, int key,map<vector<int>,int>* Direct);

int DisplayFile(int fd);

vector<int> binarize (int value);         //tested clean
int debinarize(vector<int> vec);          //tested clean


#endif /* READFILE_H_ */
