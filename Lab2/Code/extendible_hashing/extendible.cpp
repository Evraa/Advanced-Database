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


bool allAreTheSame(vector<DataItem> buck_array, int hashy)
{
	for (int i=0; i<buck_array.size(); i++)
	{
		if (hashCode(buck_array[i].key) != hashy)
			return false;
	}
	return true;
}
/**
 * Input: value in range [0:hash_value-1] and Depth
 * Output: returns the down casted value
 * 
 * Ex: value 9 and depth = 2
 * 		binarized 9 is 01001
 * 		casted 9 is 1 .. thus 1 is returned.
*/
int castIt (int value, int depth)
{
	int base = 0;
	int result = 0;
	vector<int> bin_vec = binarize(value);
	for (int i=0; i<depth; i++)
	{
		if (bin_vec[i] == 1)
			result += pow(2,base);
		base += 1;
	}
	return result;
}

/**
 * If file is already exist, we gotta read the directory info from it
 * If it's new/clear file, a directory has to be created with two empty bickets
 * 
 * Input: Directory pointer from main, which is always empty at first...even if the file has old data.
*/
void init(vector<Bucket*> * Directory, bool exist)
{
	if (exist)
	{
		//read it from file
		//note, file may exist and directory is not, wo you gotta handle it somewhere
		return;
	}
	else
	{
		//Fill the directory sent
		Bucket buck_1,buck_2;
		buck_1.local_depth = 1;
		buck_2.local_depth = 1;
		Directory->push_back(&buck_1);
		Directory->push_back(&buck_2);
		return;
	}
}

int insertItem(int fd, DataItem item, vector<Bucket*>* Directory){
	//TODO: implement the insert function.
	if (item.key >= hash_value)
		return 5;
		
	int hashed_key = hashCode(item.key);						//int hashed key
	int GD = (int)log2(Directory->size()); 						//global depth
	Bucket* main_bucket = (*Directory)[castIt(hashed_key,GD)];  //that exact bucket shall hold item.key's value.

	if (main_bucket->data_array.size() == M)
	{
		//OVERFLOW encountered

		//imagine these values was added with M=2, hash_value=32: [0,32,64]
		//where will 64 be stored at?
		//so this is not file size error, nor writing error, this a Special error.	
		if (allAreTheSame(main_bucket->data_array, hashed_key))
			return 4;
		//Unexpected error, yet possible.
		if (main_bucket->local_depth > GD)
			return 5;
		
		bool doubling = false;
		if (main_bucket->local_depth == GD)
		{
			//Double the Directory, adjust your pointers, and then Split&Rehash
			
			//Can't double any more...hash value limit exceeded!
			if (GD == K)
				return 3;
			//loop for the second half, first one is fine.
			int size = Directory->size();
			
			for (int i=0; i<size; i++)
				Directory->push_back((*Directory)[i]);
		
			doubling = true;
		}

		//Task: Just splitting and reshshing

		//create new guy.
		Bucket* secondary_bucket = new Bucket;
		//adjust new local depths.
		main_bucket->local_depth += 1;
		int LD = main_bucket->local_depth;
		secondary_bucket->local_depth = LD;
		//Task: Rehash

		//temp holder
		vector<DataItem> temp_data_holder = main_bucket->data_array;
		//add the new value we wish to add
		temp_data_holder.push_back(item);
		//empty the main bucket data
		main_bucket->data_array.clear();
		//main holder boundry
		int main_bucket_boundry = castIt(hashed_key,LD);
		//Range to exceed
		int middle_range = (int)pow(2,LD-1);

		//Arrange the main bucket boundry....I prefer main bucket to be the smaller in value.
		if (main_bucket_boundry >= middle_range)
			main_bucket_boundry -= middle_range;
		//secondary holder boundry
		int sec_bucket_boundry = main_bucket_boundry + middle_range;
		
		//rehash all values
		for (int i=0; i<temp_data_holder.size(); i++)
		{
			int casted_hashed_key = castIt(hashCode(temp_data_holder[i].key),LD);
			if (casted_hashed_key == main_bucket_boundry)
				main_bucket->data_array.push_back(temp_data_holder[i]);
			else if (casted_hashed_key == sec_bucket_boundry)
				secondary_bucket->data_array.push_back(temp_data_holder[i]);
			else
				printf ("BREAKPOINT FOR EV ... Delete when done\n\n");
		}
		//Task: Create/Update Directory Pointers
		for (int i=0; i<Directory->size(); i++)
		{
			if (castIt(i,LD) == main_bucket_boundry)
				(*Directory)[i] = main_bucket;

			else if (castIt(i,LD) == sec_bucket_boundry)
				(*Directory)[i] = secondary_bucket;

			//else: m4 tb3na..
		}
		if (doubling)
			return 2;
		else
			return 1;			
		
	}
	else
	{
		//we are at el-Saleem
		main_bucket->data_array.push_back(item);
		return 0;
	}
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
