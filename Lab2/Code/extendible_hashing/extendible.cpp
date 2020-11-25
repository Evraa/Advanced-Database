#include "readfile.h"




/* Hash function to choose bucket
 * Input: key used to calculate the hash
 * Output: HashValue;
 */
int hashCode(int key){
   return key % hash_value;
}

/**
 * This function takes as an input an integer value
 * and returns a vector of its binary value up to K values
*/
vector<int> binarize(int value){
	vector<int> bin;
	
	while (value >= 2 ){
		bin.push_back(value%2);
		value = floor(value/2);
	}
	bin.push_back(value);
	while (bin.size() < K)
		bin.push_back(0);
	
	return bin;
}

/**
 * it's obvious.
*/
int debinarize(vector<int> vec){
	if (vec.size() == 0)
		{
			printf ("Error 101: Empty vector of binaries\n\n");
			return -1;
		}
	if (vec.size() > K)
		{
			printf ("Error 102: Vector size exceeds the limit, can't process!\n\n");
			return -1;
		}
	int value = 0;
	int base = 0;
	for (int i=0; i<vec.size(); i++){
		if (vec[i] == 1)
			value += pow(2,base);
		base += 1;
	}
	return value;
}


int insertItem(int fd, DataItem item, map<vector<int>,int>* Direct){
	//TODO: implement the insert function.

	


	return 0;
}

int deleteItem(int filehandle, int key,map<vector<int>,int>* Direct){
	//TODO: implement the delete function.

	/**
	 * first search for the value, if it doesn't exist, print error 301
	 * if it does exist, apply your algo.
	*/
	return 0;
}
int searchItem(int filehandle, int key,map<vector<int>,int>* Direct){
	// implement the search function.

	/**
	 * return -1 when error
	 * return the data value
	*/
	return 0;
}



//TODO: modify display file to show the directory then the buckets with offsets and items (keys/data)

// /* Functionality: Display all the file contents
//  *
//  * Input:  fd: filehandler which contains the db
//  *
//  * Output: no. of non-empty records
//  */
// int DisplayFile(int fd){
// 	printf("\n\n####### Display File #######\n\n");
// 	struct DataItem data;
// 	int count = 0;
// 	int Offset = 0;
// 	for(Offset =0; Offset< FILESIZE/2;Offset += sizeof(DataItem))
// 	{
// 		ssize_t result = pread(fd,&data,sizeof(DataItem), Offset);
// 		if(result < 0)
// 		{ 	  perror("some error occurred in pread");
// 			  return -1;
// 		} else if (result == 0 || data.valid == 0 ) { //empty space found or end of file
// 			printf("Bucket: %d, Offset %d:~\n",(int)(Offset/BUCKETSIZE),Offset);
// 		} else {
// 			pread(fd,&data,sizeof(DataItem), Offset);
// 			printf("Bucket: %d, Offset: %d, Data: %d, key: %d, points to: %d\n",(int)(Offset/BUCKETSIZE),Offset,data.data,data.key, data.pointer_index);
// 					 count++;
// 		}
// 	}
// 	for(Offset = FILESIZE/2; Offset < FILESIZE; Offset += sizeof(DataItem))
// 	{
// 		ssize_t result = pread(fd, &data, sizeof(DataItem), Offset);
// 		if(result < 0)
// 		{ 	  perror("some error occurred in pread");
// 			  return -1;
// 		} else if (result == 0 || data.valid == 0 ) { //empty space found or end of file
// 			printf("Overflow, Offset %d:~\n", Offset);
// 		} else {
// 			pread(fd,&data,sizeof(DataItem), Offset);
// 			printf("Overflow, Offset: %d, Data: %d, key: %d, points to: %d\n", Offset, data.data, data.key,data.pointer_index);
// 					 count++;
// 		}
// 	}
// 	printf("\n####### EOF #######\n\n");
// 	return count;
// }
