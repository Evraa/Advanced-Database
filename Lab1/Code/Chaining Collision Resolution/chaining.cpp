#include "readfile.h"


/* Hash function to choose bucket
 * Input: key used to calculate the hash
 * Output: HashValue;
 */
int hashCode(int key){
   return key % MBUCKETS;
}

// int lastOverflowIndex = -1;

/* Functionality insert the data item into the correct position
 *          1. use the hash function to determine which bucket to insert into
 *          2. search for the first empty space within the bucket
 *          	2.1. if it has empty space
 *          	           insert the item
 *          	     else
 *          	          use OpenAddressing to insert the record
 *          3. return the number of records accessed (searched)
 *
 * Input:  fd: filehandler which contains the db
 *         item: the dataitem which should be inserted into the database
 *
 * Output: No. of record searched
 *
 * Hint: You can check the search function to give you some insights
 * Hint2: Don't forget to set the valid bit = 1 in the data item for the rest of functionalities to work
 * Hint3: you will want to check how to use the pwrite function using man pwrite on the terminal
 * 			 ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);
 *
 * 	pwrite() writes up to count bytes from the buffer starting  at  buf  to
       the  file  descriptor  fd  at  offset  offset.
 */
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
	struct DataItem dataIter;
	struct DataItem dataIter_2;
	ssize_t read_result = pread(fd, &dataIter, DATASIZE, Offset);

	while (dataIter.pointer_index != -1)
	{
		//update
		Offset = dataIter.pointer_index;
		ssize_t read_result_2 = pread(fd, &dataIter_2, DATASIZE, dataIter.pointer_index);
		dataIter.data = dataIter_2.data;
		dataIter.pointer_index = dataIter_2.pointer_index;
		ssize_t read_result = pread(fd, &dataIter, DATASIZE, Offset);
		
	}

	struct DataItem dummyItem;
	dummyItem.valid = 0;
	dummyItem.key = -1;
	dummyItem.data = 0;
	int result = pwrite(fd,&dummyItem,sizeof(DataItem), Offset);

	return result;
}
