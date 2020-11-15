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
   printf("Test Case 1:\n\t+ Adding elements at hash = 1, hash = 2 and various positions.\n\t+ Make sure to fill the File\n");
   printf("\t+ Search for keys at their buckets, and outside it: 13 and 73\n");
   printf("########### End of Test Case 1 ###############\n");
}

void testCase_2 (){
   printf("Test Case 2:\n\t+ Continuing after test case 1\n");
   printf("\t+ Search for element at bottom of chain\n");
   printf("\t+ Delete items at their buckets and outside of it: \n");
   printf("########### End of Test Case 2 ###############\n");
}


int main(){

//here we create a sample test to read and write to our database file
   printf("\t\t############ DOUBLE HASH COLLISION RES ############n\n");
   
  //1. Create Database file or Open it if it already exists, check readfile.cpp
   filehandle = createFile(FILESIZE,(char *)"multiple_hashing");
  //2. Display the database file, check openAddressing.cpp
   // DisplayFile(filehandle);

   int total_count = 0;

  //3. Add some data in the table
   total_count+= insert(1, 1);     //1
   total_count+= insert(2, 2);     //2
   total_count+= insert(42, 3);    //2
   total_count+= insert(91, 4);    //1
   total_count+= insert(12, 5);    //2
   total_count+= insert(14, 6);    //4
   total_count+= insert(22, 7);    //2
   total_count+= insert(13, 8);    //3
   total_count+= insert(37, 9);    //7
   total_count+= insert(11, 10);   //1
   total_count+= insert(22, 11);   //2
   total_count+= insert(46, 12);   //6
   total_count+= insert(9, 13);    //9
   total_count+= insert(21, 14);   //1
   total_count+= insert(41, 15);   //1
   total_count+= insert(71, 16);   //1
   total_count+= insert(31, 17);   //1
   total_count+= insert(73, 18);   //3
   total_count+= insert(99, 19);   //9
   total_count+= insert(37, 20);   //7
   //These wont be added
   total_count+= insert(27, 21);   //7
   total_count+= insert(47, 22);   //7
   total_count+= insert(41, 23);   //1
   total_count+= insert(51, 24);   //1
   total_count+= insert(61, 25);   //1

   DisplayFile(filehandle);

   printf("########## For Comparison: #############\n");
   printf("\tTotal count of readings = %d\n",total_count);
   printf("########## End of Comparison #############\n");

   testCase_1();
   //5. Search the database
   search(13);    //at the end of its bucket
   search(73);    //outside its bucket
   search(2214);  //For comparison
   DisplayFile(filehandle);


   testCase_2();
   //6. delete an item from the database
   deleteItem(1);    //start of its bucket
   deleteItem(91);   //end of its bucket
   deleteItem(11);   //outside its bucket
   DisplayFile(filehandle);


   // //7. Display the final data base
   // DisplayFile(filehandle);
   // And Finally don't forget to close the file.
   close(filehandle);
   return 0;



}

/* functionality: insert the (key,data) pair into the database table
                  and print the number of comparisons it needed to find
    Input: key, data
    Output: print statement with the no. of comparisons
*/
int insert(int key,int data){
     struct DataItem item ;
     item.data = data;
     item.key = key;
     item.valid = 1;
     int result= insertItem(filehandle,item);  //TODO: implement this function in openAddressing.cpp
     printf("Insert: No. of searched records:%d\n",abs(result));
     return result;
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
