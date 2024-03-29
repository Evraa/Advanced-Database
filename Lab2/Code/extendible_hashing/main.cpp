//============================================================================
// Name        : hashskeleton.cpp
// Author      : 
// Version     :
// Copyright   : Code adapted From https://www.tutorialspoint.com/
// Description : Hashing using open addressing
//============================================================================

#include "readfile.h"

//Global Variables
int filehandle;               //handler for the database file
int insert(int key,int data);
void delete_main(int key);
void search(int key);
vector<Bucket*> Directory;    //Main Directory Vector with max size=hash_value

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
   bool exist = false;
   filehandle = createFile(FILESIZE,(char *)"extendible", &exist);
   //2. Init the Directory
   init(filehandle, Directory, exist);

   insert(32,1);
   insert(64,2);
   
   insert(128,3);
   insert(6,4);
   
   // delete_main(6);
   
   

   WriteFile(filehandle, Directory);
   printf("Displaying from the File..\n\n");
   DisplayFile(filehandle);

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
         3  : max size reached
         4  : very special one; if you inserted the same value untill directory can't be split!
         5  : any unexpected error.
*/
int insert(int key,int data){
   
      DataItem* item = new DataItem ;
      item->data = data;
      item->key = key;

      int state = insertItem(filehandle,*item,Directory);  //TODO: implement this function in openAddressing.cpp
      
      switch (state)
      {
      case -1:
         printf ("Error 201: An UNKNOWN error occured while adding data: %d with key: %d\n",data,key);
         break;
      case 0:
         printf("Value: %d \twith key: %d\t added with no splitting nor doubling\n",data,key);
         break;
      case 1:
         printf("Value: %d \twith key: %d\t added with Splitting BUT no doubling\n",data,key);
         break;
      case 2:
         printf("Value: %d \twith key: %d\t added with Splitting AND Doubling\n",data,key);
         break;
      case 3:
         printf("Error 203: Can't Double more! at data: %d with key: %d\n",data,key);
         break;
      case 4:
         printf("Error 204: Special Error; M keys exist, can't add more of data: %d with key: %d\n",data,key);
         break;
      case 5:
         printf("Error 205: Unexpected Error at All; Data: %d with key: %d\n",data,key);
         break;
      }
      print_directory(Directory);
      return state;
}

void search(int key)
{
   int results = searchItem(filehandle, key, Directory);
   if (results == -1)
      printf("Searching for key: %d, not found or an error occured\n", key);
   else
      printf("Searching for key: %d, and data: %d\n",key, results);
}

void delete_main(int key)
{

   int state = deleteItem(filehandle, key,Directory);
   switch (state)
   {
   case -1:
      printf("DELETE -> Key: %d \tDoes Not exist\n",key);
      break;
   
   case 0:
      printf("DELETE -> Key: %d \tErased with no shrink no merge\n",key);
      break;
   case 1:
      printf("DELETE -> Key: %d \tErased with merging ONLY\n",key);
      break;
   case 2:
      printf("DELETE -> Key: %d \tErased with merge and shrink\n",key);
      break;
   }
   print_directory(Directory);
}