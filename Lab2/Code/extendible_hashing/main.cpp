//============================================================================
// Name        : hashskeleton.cpp
// Author      : 
// Version     :
// Copyright   : Code adapted From https://www.tutorialspoint.com/
// Description : Hashing using open addressing
//============================================================================

#include "readfile.h"

int insert(int key,int data);
int deleteItem(int key);
struct DataItem * search(int key);


//Global Variables

vector<Bucket*> Directory;    //Main Directory Vector
int filehandle;               //handler for the database file

/* DBMS (DataBase Management System) needs to store its data in something non-volatile
 * so it stores its data into files (manteqy :)).

 * Some DBMS or even file-systems constraints the size of that file. 

 * for the efficiency of storing and retrieval, DBMS uses hashing 
 * as hashing make it very easy to store and retrieve, it introduces 
 * another type of problem which is handling conflicts when two items
 * have the same hash index

 * In this exercise we will store our database into a file and experience
 * how to retrieve, store, update and delete the data into this file 

 * This file has a constant capacity and uses external-hashing to store records,
 * however, it suffers from hashing conflicts.
 * 
 * You are required to help us resolve the hashing conflicts 

 * For simplification, consider the database has only one table 
 * which has two columns key and data (both are int)

 * Functions in this file are just wrapper functions, the actual functions are in openAddressing.cpp

*/

int main(){

   
   printf("\t\t############ EXTENDIBLE HASHING ##############\n\n");
   
   //1. Create Database file or Open it if it already exists, check readfile.cpp
   filehandle = createFile(FILESIZE,(char *)"extendible");
   Bucket* bucky = new Bucket;
   printf("%p\n", bucky);
   printf("%lu\n", sizeof(bucky));

   // And Finally don't forget to close the file.
   close(filehandle);
   return 0;
}

/* functionality: insert the (key,data) pair into the database table
                  and print the number of comparisons it needed to find
    Input: key, data
    Output: state
         -1 : error encountered
         0  : value added with no splitting nor doubling
         1  : value added with splitting but no doubling
         2  : value added with splitting and doubling 
*/
int insert(int key,int data){
      struct DataItem item ;
      item.data = data;
      item.key = key;
      int state = insertItem(filehandle,item,&Directory);  //TODO: implement this function in openAddressing.cpp
      switch (state)
      {
      case -1:
         printf ("Error 201: An UNKNOWN error occured while adding data: %d with key: %d\n",data,key);
         break;
      case 0:
         printf("Value: %d \twith key: %d\t added with no splitting nor doubling\n",data,key);
         break;
      case 1:
         printf("Value: %d \twith key: %d\t added with Splitting bit no doubling\n",data,key);
         break;
      case 2:
         printf("Value: %d \twith key: %d\t added with Splitting AND Doubling\n",data,key);
         break;
      case 3:
         printf("Error 202: FILE SIZE LIMIT EXCEEDED at data: %d with key: %d\n",data,key);
         break;
      }
      return state;
}
