#include "readfile.h"
#include <iostream>
using namespace std;

/* Hash function to choose bucket
 * Input: key used to calculate the hash
 * Output: HashValue;
 */
int hashCode(int key){
   return key % MBUCKETS;
}


int insertItem(int fd, DataItem item){
   //TODO: implement this function
	//Definitions
	struct DataItem data;   //a variable to read in it the records from the db
	int hashIndex = hashCode(item.key);  				//calculate the Bucket index
	int startingOffset = hashIndex*sizeof(Bucket);		//calculate the starting address of the bucket
	//No need for that!
	int Offset = startingOffset;						//Offset variable which we will use to iterate on the db
	item.valid = 1;
	int count = -1;										//# Records we span in order to find an empty space
	int overflowFlag = 0;
	
	//Main Loop
	RESEEK:
	count++;
	//read data at offset
	ssize_t read_result = pread(fd,&data,DATASIZE, Offset);
	//Error occured...
    if(read_result <= 0)
		return -1;

	//Deleted or Unused place for data.
    else if (data.valid == 0) 
	{
		//Get the last offset of that bucket starting offset
		int lastOffset = startingOffset + ((RECORDSPERBUCKET-1)*DATASIZE);
		//if offset occurs at OVERFLOW, create a pointer to it
		if (Offset >= OVERFLOWSIZE)
		{
			//check if the data written is at overflow and last offset is assigned to -1
			ssize_t read_result_2 = pread(fd, &data, DATASIZE, lastOffset);
			count++;
			//if that last data points to -1, then assign it
			if (data.pointer_index == -1)
			{
				//make sure next points to null.
				item.pointer_index = -1;
				//Write your item at offset.
    			pwrite(fd, &item, DATASIZE, Offset);
				//last element of the bucket points to chain
				data.pointer_index = Offset;
				pwrite(fd, &data, DATASIZE, lastOffset);
			}
			else
			{
				//chaining logic
				//Search for an Empty place
				//if not, return -1
				struct DataItem chainData;
				ssize_t read_result_offset = pread(fd, &chainData, DATASIZE, Offset);
				count++;
				int checkPoint = Offset;
				while (chainData.pointer_index != 0)
				{
					Offset += DATASIZE;
					if (Offset == checkPoint)
						return -1;
					
					if (Offset >= FILESIZE)
						Offset = OVERFLOWSIZE;
					ssize_t read_result_offset = pread(fd, &chainData, DATASIZE, Offset);
					count++;
				}
				//make sure next points to null.
				item.pointer_index = -1;
				//Write your item at empty offset.
    			pwrite(fd, &item, DATASIZE, Offset);

				//Search for last element of that chain, to point to offset
				int prevToLastOffset = lastOffset;
				lastOffset = data.pointer_index;
				while (lastOffset != -1)
				{
					ssize_t read_result_2 = pread(fd, &data, DATASIZE, lastOffset);
					count++;
					prevToLastOffset = lastOffset;
					lastOffset = data.pointer_index;
				}
				data.pointer_index = Offset;
				pwrite(fd, &data, DATASIZE, prevToLastOffset);
			}
			
		}
		else
		{
			if (Offset != startingOffset)
			{
				int prevOffset = Offset - DATASIZE;
				ssize_t read_result_2 = pread(fd, &data, DATASIZE, prevOffset);
				count++;
				data.pointer_index = Offset;
				pwrite(fd, &data, DATASIZE, prevOffset);
			}
			
			//it is the last element of the bucket, make sure it points to null.
			item.pointer_index = -1;
			//Write your item at offset.
			pwrite(fd, &item, DATASIZE, Offset);
			
		}
		
		return count;
    }	
	else
	{
		//Find first empty place within the bucket
		//	OR chain and add to last
		//	OR Error (Filesize exceeded)
		
		Offset  += DATASIZE;  //move the offset to next record within the bucket

		if(Offset >= (startingOffset + BUCKETSIZE) && overflowFlag == 0)
		{	
			overflowFlag = 1;
			Offset = OVERFLOWSIZE;
			goto RESEEK;
		} 
		else
		{
			if(overflowFlag == 1 && Offset >= FILESIZE)
			{
				printf("File Limit Exceeded, no more Capacity in the Overflow\n");
				return -1;
			}
			
		}
		goto RESEEK;
    }
}


/* Functionality: using a key, it searches for the data item
 *          1. use the hash function to determine which bucket to search into
 *          2. search for the element starting from this bucket and till it find it.
 *          3. return the number of records accessed (searched)
 *
 * Input:  fd: filehandler which contains the db
 *         item: the dataitem which contains the key you will search for
 *               the dataitem is modified with the data when found
 *         count: No. of record searched
 *
 * Output: the in the file where we found the item
 */

int searchItem(int fd,struct DataItem* item,int *count)
{

	//Definitions
	struct DataItem data;   //a variable to read in it the records from the db
	*count = 0;				//No of accessed records
	int rewind = 0;			//A flag to start searching from the first bucket
	int hashIndex = hashCode(item->key);  				//calculate the Bucket index
	int startingOffset = hashIndex*sizeof(Bucket);		//calculate the starting address of the bucket
	int Offset = startingOffset;						//Offset variable which we will use to iterate on the db
	int overflow = 0;
	int overflow_startingOffset = -1;

	//Main Loop
	RESEEK:
	//on the linux terminal use man pread to check the function manual
	ssize_t result = pread(fd,&data,sizeof(DataItem), Offset);
	//one record accessed
	(*count)++;
	//check whether it is a valid record or not
    if(result <= 0) //either an error happened in the pread or it hit an unallocated space
	{ 	 // perror("some error occurred in pread");
		  return -1;
    }
    else if (data.valid == 1 && data.key == item->key) {
    	//I found the needed record
    			item->data = data.data ;
    			return Offset;

    } else { //not the record I am looking for
    		Offset +=sizeof(DataItem);  //move the offset to next record
    		if(Offset >= (startingOffset + BUCKETSIZE) && overflow == 0)
    		 { //if reached end of the Bucket start searching in OVERFLOW
    				overflow = 1;
					ssize_t result = pread(fd,&data,sizeof(DataItem), startingOffset);
					if (data.pointer_index == -1){
						return -1;
					}
    				overflow_startingOffset = data.pointer_index;
					Offset = overflow_startingOffset;
    				goto RESEEK;
    	     } else 
			 	if (Offset >= FILESIZE && rewind ==0){
					rewind = 1;
    				Offset = OVERFLOWSIZE;
    				goto RESEEK;
			 }
			 
			 else
    	    	  if(rewind == 1 && Offset >= overflow_startingOffset) {
    				return -1; //no empty spaces
    	     }
    		goto RESEEK;
    }
}


/* Functionality: Display all the file contents
 *
 * Input:  fd: filehandler which contains the db
 *
 * Output: no. of non-empty records
 */
int DisplayFile(int fd){

	struct DataItem data;
	int count = 0;
	int Offset = 0;
	for(Offset =0; Offset< FILESIZE/2;Offset += sizeof(DataItem))
	{
		ssize_t result = pread(fd,&data,sizeof(DataItem), Offset);
		if(result < 0)
		{ 	  perror("some error occurred in pread");
			  return -1;
		} else if (result == 0 || data.valid == 0 ) { //empty space found or end of file
			printf("Bucket: %d, Offset %d:~\n",(int)(Offset/BUCKETSIZE),Offset);
		} else {
			pread(fd,&data,sizeof(DataItem), Offset);
			printf("Bucket: %d, Offset: %d, Data: %d, key: %d, points to: %d\n",(int)(Offset/BUCKETSIZE),Offset,data.data,data.key, data.pointer_index);
					 count++;
		}
	}
	for(Offset = FILESIZE/2; Offset < FILESIZE; Offset += sizeof(DataItem))
	{
		ssize_t result = pread(fd, &data, sizeof(DataItem), Offset);
		if(result < 0)
		{ 	  perror("some error occurred in pread");
			  return -1;
		} else if (result == 0 || data.valid == 0 ) { //empty space found or end of file
			printf("Overflow, Offset %d:~\n", Offset);
		} else {
			pread(fd,&data,sizeof(DataItem), Offset);
			printf("Overflow, Offset: %d, Data: %d, key: %d, points to: %d\n", Offset, data.data, data.key,data.pointer_index);
					 count++;
		}
	}
	return count;
}


/* Functionality: Delete item at certain offset
 *
 * Input:  fd: filehandler which contains the db
 *         Offset: place where it should delete
 *
 * Hint: you could only set the valid key and write just and integer instead of the whole record
 */
int deleteOffset(int fd, int Offset)
{
	cin>>Offset;

	
	//Data iterators, using slow/fast runner approach.
	struct DataItem dataIter;
	struct DataItem dataIter_2;
	ssize_t read_result = pread(fd, &dataIter, DATASIZE, Offset);
	int hashIndex = hashCode(dataIter.key);  				//calculate the Bucket index
	int startingOffset = hashIndex*sizeof(Bucket);		//calculate the starting address of the bucket
	
	//If Offset is the last element, its presuccesor needs to point to -1.
	if (dataIter.pointer_index == -1)
	{
		//Now offset is the last element, delete it and return.
		struct DataItem dummyItem;
		dummyItem.valid = 0;
		dummyItem.key = -1;
		dummyItem.data = 0;
		dummyItem.pointer_index = 0;
		pwrite(fd,&dummyItem,sizeof(DataItem), Offset);

		if (Offset < OVERFLOWSIZE)
		{
			//within the Originals
			//simple minus operation
			if (Offset!=startingOffset)
			{
				ssize_t read_result_2 = pread(fd, &dataIter_2, DATASIZE, Offset-DATASIZE);
				dataIter_2.pointer_index = -1;
				int result = pwrite(fd,&dataIter_2,sizeof(DataItem), Offset-DATASIZE);
				return result;
			}
			//else is handled down below.
		}
		else
		{
			//within overflow
			//Brute force search for presuccessor
			ssize_t read_result_2 = pread(fd, &dataIter_2, DATASIZE, startingOffset);

			while (dataIter_2.pointer_index != Offset)
			{
				startingOffset = dataIter_2.pointer_index;
				ssize_t read_result_2 = pread(fd, &dataIter_2, DATASIZE, startingOffset);
			}
			dataIter_2.pointer_index = -1;
			int result = pwrite(fd,&dataIter_2,sizeof(DataItem), startingOffset);
			return result;
		}
		
	}
	while (dataIter.pointer_index != -1)
	{
		ssize_t read_result_2 = pread(fd, &dataIter_2, DATASIZE, dataIter.pointer_index);
		//Copy data of the fast runner to slow runner
		dataIter.data = dataIter_2.data;	
		//keep second pointer as a temp.
		int temp = dataIter.pointer_index;
		//if fast runner is the end, then slow runner WILL be the end.
		if (dataIter_2.pointer_index == -1)
			dataIter.pointer_index = -1;
		//Write your updates
		pwrite(fd,&dataIter,sizeof(DataItem), Offset);
		//update the offset
		Offset = temp;
		//read and repeat
		ssize_t read_result = pread(fd, &dataIter, DATASIZE, Offset);
	}
	//Now offset is the last element, delete it and return.
	struct DataItem dummyItem;
	dummyItem.valid = 0;
	dummyItem.key = -1;
	dummyItem.data = 0;
	dummyItem.pointer_index = 0;
	int result = pwrite(fd,&dummyItem,sizeof(DataItem), Offset);
	return result;
}



/*
	Test Cases Tried:
		+ Insert then Delete.
		+ Fill Overflow, and then delete all.
		+ Delete element does not exist.
		+ Deleting offset in the middle of the chain (EVERYWHERE)
*/