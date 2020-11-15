//============================================================================
// Name        : hashskeleton.cpp
// Author      : 
// Version     :
// Copyright   : Code adapted From https://www.tutorialspoint.com/
// Description : Hashing using open addressing
//============================================================================

#include "readfile.h"

void insert(int key,int data);
int deleteItem(int key);
struct DataItem * search(int key);


int filehandle;   //handler for the database file

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

void testCase_1 (){
   printf("Test Case 1:\n\t+ Adding elements at hash = 1, hash = 2 and various positions.\n\t+ Fill the Overflow\n");
   printf("\t+ Delete item at the end of the chain.\n\t+ Delete item at the middle of the chain.\n");
   printf("\t+ Delete item at the start of the chain.\n\t+ Delete item at the end of Original Buckets.\n");
   printf("\t+ Delete item at the start of bucket\n");
   printf("########### End of Test Case 1 ###############\n");
}

void testCase_2 (){
   printf("Test Case 2:\n\t+ Continuing after test case 1\n");
   printf("\t+ Search for element at bottom of chain\n");
   printf("\t+ Search for element at start of chain\n");
   printf("\t+ Search for element at end of bucket\n");
   printf("\t+ Search for element at start of bucket\n");
   printf("########### End of Test Case 2 ###############\n");
}

int main(){

   
   printf("\t\t############ CHAINING COLLISION RES############n\n");
   
   //1. Create Database file or Open it if it already exists, check readfile.cpp
   filehandle = createFile(FILESIZE,(char *)"chaining");

   //2. Display the database file, check openAddressing.cpp
   // DisplayFile(filehandle);


   //3. Add some data in the table
   insert(1, 1);     //1
   insert(2, 2);     //2
   insert(42, 3);    //2
   insert(91, 4);    //1
   insert(12, 5);    //2
   insert(14, 6);    //4
   insert(17, 7);    //7
   insert(13, 8);    //3
   insert(37, 9);    //7
   insert(11, 10);   //1
   insert(22, 11);   //2
   insert(46, 12);   //6
   insert(9, 13);    //9
   insert(21, 14);   //1
   insert(41, 15);   //1
   insert(71, 16);   //1
   insert(31, 17);   //1
   insert(73, 18);   //3
   insert(99, 19);   //9
   insert(37, 20);   //7
   insert(27, 21);   //7
   insert(47, 22);   //7
   insert(41, 23);   //1
   insert(51, 24);   //1
   insert(61, 25);   //1
   insert(81, 26);   //1
   insert(101, 27);  //1
   insert(111, 28);  //1
   insert(121, 29);  //1
   insert(131, 30);  //1
   insert(141, 31);  //1
   insert(151, 32);  //1
   
   //4. Display the database file again
   DisplayFile(filehandle);
   
   //Test case 1
   testCase_1();
   deleteItem(161);
   deleteItem(151);
   deleteItem(41);
   deleteItem(11);
   deleteItem(91);
   deleteItem(1);
   DisplayFile(filehandle);

   //Test case 2
   testCase_2();
   search(47);
   search(27);
   search(17);
   DisplayFile(filehandle);
   
   
   // And Finally don't forget to close the file.
   close(filehandle);
   return 0;
}

/* functionality: insert the (key,data) pair into the database table
                  and print the number of comparisons it needed to find
    Input: key, data
    Output: print statement with the no. of comparisons
*/
void insert(int key,int data){
     struct DataItem item ;
     item.data = data;
     item.key = key;
     item.valid = 1;
     item.pointer_index = -1;
     int result= insertItem(filehandle,item);  //TODO: implement this function in openAddressing.cpp
     printf("Insert: No. of searched records:%d\n",abs(result));
}

/* Functionality: search for a data in the table using the key

   Input:  key
   Output: the return data Item
*/
struct DataItem * search(int key)
{
  struct DataItem* item = (struct DataItem *) malloc(sizeof(struct DataItem));
     item->key = key;
     int diff = 0;
     int Offset= searchItem(filehandle,item,&diff); //this function is implemented for you in openAddressing.cpp
     printf("Search: No of records searched is %d\n",diff);
     if(Offset <0)  //If offset is negative then the key doesn't exists in the table
       printf("Item not found\n");
     else
        printf("Item found at Offset: %d,  Data: %d and key: %d\n",Offset,item->data,item->key);
  return item;
}

/* Functionality: delete a record with a certain key

   Input:  key
   Output: return 1 on success and -1 on failure
*/
int deleteItem(int key){
   struct DataItem* item = (struct DataItem *) malloc(sizeof(struct DataItem));
   item->key = key;
   int diff = 0;
   int Offset= searchItem(filehandle,item,&diff);
   printf("Delete: No of records searched is %d\n",diff);
   if(Offset >=0 )
   {
    return deleteOffset(filehandle,Offset);
   }
   return -1;
}
