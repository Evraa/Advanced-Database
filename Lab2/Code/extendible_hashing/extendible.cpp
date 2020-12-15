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
// int debinarize(vector<int> vec){
// 	if (vec.size() == 0)
// 		{
// 			printf ("Error 101: Empty vector of binaries\n\n");
// 			return -1;
// 		}
// 	if (vec.size() > K)
// 		{
// 			printf ("Error 102: Vector size exceeds the limit, can't process!\n\n");
// 			return -1;
// 		}
// 	int value = 0;
// 	int base = 0;
// 	for (int i=0; i<vec.size(); i++){
// 		if (vec[i] == 1)
// 			value += pow(2,base);
// 		base += 1;
// 	}
// 	return value;
// }


bool allAreTheSame(vector<DataItem*> buck_array, int hashy)
{
	for (int i=0; i<buck_array.size(); i++)
	{
		if (hashCode(buck_array[i]->key) != hashy)
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
void init(int fd,vector<Bucket*> & Directory, bool exist)
{
    //Fill the directory sent
	Bucket* bucky_1 = new Bucket();
	bucky_1->local_depth = 1;
	Directory.push_back(bucky_1);

	Bucket* bucky_2 = new Bucket();
	bucky_2->local_depth = 1;
	Directory.push_back(bucky_2);
	
	return;
}

int findApprDepth(int casted, int hashed_key, int LD)
{
	//size = K
	int count = 0;
	while (castIt(casted, LD) == castIt(hashed_key, LD))
	{
		count += 1;
		LD += 1;
	}
	return count;
}


int insertItem(int fd, DataItem &item, vector<Bucket*>&Directory){
	//TODO: implement the insert function.

	int hashed_key = hashCode(item.key);						//int hashed key	
	int GD = (int)log2(Directory.size()); 						//global depth
	Bucket* main_bucket = Directory[castIt(hashed_key,GD)];  //that exact bucket shall hold item.key's value.
	
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
		
		int appropriate_depth = findApprDepth(castIt(hashed_key,GD), hashed_key, main_bucket->local_depth);

		bool doubling = false;
		int i = 0;
		while (i < appropriate_depth)
		{
			//Double the Directory, adjust your pointers, and then Split&Rehash
			
			//Can't double any more...hash value limit exceeded!
			if (GD == K)
				return 3;

			//loop for the second half, first one is fine.
			int size = Directory.size();
			//it's just copying the pointers
			//assume GD was 2 and it will be 3 .. [00,01,10,11] -> [000 .. 111]
			//so [000,001,010,011] are the same..no change
			//and [100,101,110,111] -> 100 will be the same as 000 ...etc.
			for (int i=0; i<size; i++)
				Directory.push_back(Directory[i]);
		
			doubling = true;
			//Task: Just splitting and reshshing

			//create new guy.
			Bucket* secondary_bucket = new Bucket;
			//adjust new local depths.
			main_bucket->local_depth += 1;
			int LD = main_bucket->local_depth;
			secondary_bucket->local_depth = LD;
			//Task: Rehash

			//temp holder
			vector<DataItem*> temp_data_holder = main_bucket->data_array;
			//add the new value we wish to add
			if (i == appropriate_depth-1)
				temp_data_holder.push_back(&item);
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
				int casted_hashed_key = castIt(hashCode(temp_data_holder[i]->key),LD);
				if (casted_hashed_key == main_bucket_boundry)
					main_bucket->data_array.push_back(temp_data_holder[i]);
				else if (casted_hashed_key == sec_bucket_boundry)
					secondary_bucket->data_array.push_back(temp_data_holder[i]);
				else
					printf ("BREAKPOINT FOR EV ... Delete when done\n\n");
			}
			//Task: Create/Update Directory Pointers
			for (int i=0; i<Directory.size(); i++)
			{
				if (castIt(i,LD) == main_bucket_boundry)
					(Directory)[i] = main_bucket;

				else if (castIt(i,LD) == sec_bucket_boundry)
					(Directory)[i] = secondary_bucket;

				//else: m4 tb3na..
			}
			
			i++;
		}
		

		
		if (doubling)
			return 2;
		else
			return 1;			
		
	}
	else
	{
		//we are at el-Saleem
		main_bucket->data_array.push_back(&item);
		return 0;
	}
}

int deleteItem(int filehandle, int key,vector<Bucket*> & Directory){
	//TODO: implement the delete function.

	/**
	 * first search for the value, if it doesn't exist, print error 301
	 * if it does exist, apply your algo.
	 * 
	 * -1	no result found
	 * 0	just erased, no shrinking
	 * 1	erased and merged two buckets
	 * 2 	erased, merged two buckets and shrinked the dict.
	*/

	int hashed_key = hashCode(key);
	int GD = (int)log2(Directory.size());
	int casted_key = castIt(hashed_key,GD);
	Bucket* main_bucket = Directory[casted_key];
	int ld = main_bucket->local_depth;

	if (main_bucket->data_array.empty())
		return -1;

	
	else
	{
		vector<DataItem*>::iterator it;
		bool found = false;

		for (it = main_bucket->data_array.begin(); it != main_bucket->data_array.end(); it++)
		{
			DataItem* curr_item = *it;
			if (curr_item->key == key){
				delete curr_item;
				main_bucket->data_array.erase(it);
				found = true;
				break;
			}
		}
		if (!found)
			return -1;

		if ((int)Directory.size()==2)
			return 0;

		if (main_bucket->data_array.empty())
		{
			//merge?
			int middle = (int)Directory.size()/2;
			if (casted_key >= middle)
				{
					Directory[casted_key] = Directory[casted_key-middle];
					if (Directory[casted_key-middle]->local_depth > 1)
						Directory[casted_key-middle]->local_depth -= 1;
				}
			else
				{
					Directory[casted_key] = Directory[casted_key+middle];
					if (Directory[casted_key+middle]->local_depth > 1)
						Directory[casted_key+middle]->local_depth -= 1;
				}

			//shrink?
			int dict_size = (int)Directory.size();
			bool shrink = true;
			for(int i=0;i <dict_size/2; i++)
			{
				if (Directory[i] != Directory[i+dict_size/2])
					{
						shrink = false;
						break;
					}
			}
			if (shrink)
			{
				// vector<Bucket*>::iterator itt;
				// for (itt = Directory.begin()+(dict_size/2); itt != Directory.end(); itt++)
				// 	Directory.erase(itt);
				vector<Bucket*> new_Directory(Directory.begin(), Directory.begin()+(dict_size/2));
				Directory = new_Directory;
				return 2;
			}
			return 1;
		}
		else
			return 0;
		
	}
	return 0;
}
int searchItem(int filehandle, int key,vector<Bucket*> & Directory){
	// implement the search function.

	/**
	 * return -1 when error or value does not exist
	 * return the data value
	*/
	int hashed_key = hashCode(key);
	int GD = (int)log2(Directory.size());
	int casted_key = castIt(hashed_key,GD);
	Bucket* main_bucket = Directory[casted_key];
	int ld = main_bucket->local_depth;
	if (main_bucket->data_array.empty())
		return -1;
	else
	{
		for (int i=0; i<(int)main_bucket->data_array.size(); i++)
		{
			if (main_bucket->data_array[i]->key== key)
				return main_bucket->data_array[i]->data;
		}
	}
	return -1;
}

void print_directory(vector<Bucket*>& Directory)
{
	printf("Dict Global Size: %d\n\n",(int)Directory.size());
		
	for (int i=0; i<(int)Directory.size(); i++)
	{
		Bucket *buck = Directory[i];
		printf("\tBucket: %d\twith size: %d\n",i, (int)buck->data_array.size());
		if (!buck->data_array.empty())
		{
			for (int j=0; j < (int)buck->data_array.size(); j++)
			{
				printf("\t\tKey: %d, \tData: %d\n", 
					buck->data_array[j]->key,buck->data_array[j]->data);
			}
		}
		else
			printf("\t\tEmpty Bucket\n");
	
	}
	printf("\n");
}


//TODO: modify display file to show the directory then the buckets with offsets and items (keys/data)

// /* Functionality: Display all the file contents
//  *
//  * Input:  fd: filehandler which contains the db
//  *
//  * Output: no. of non-empty records
//  */
 int DisplayFile(int fd){
 	printf("\n\n####### Display File #######\n\n");
 	struct DataItem data;
 	int count = 0;
 	int Offset = 0;
	int n, temp;
	//read the dict size
	pread(fd,&n,sizeof(int), Offset);
	printf("Number of Buckets: %d\n", n);
	printf("Global depth: %d\n", (int)log2(n));
	//set offset to the next int.
	Offset += sizeof(int);
	//array of all sizes possible
	int* sizes = new int[hash_value+1];
	int buck;
	for (int i=0; i<n; i++){
		pread(fd, &temp, sizeof(int), Offset);
		Offset += sizeof(int);
		printf("Bucket: %d, local depth: %d\n",(i),temp);
	}
	for (int i=0; i<n; i++){
		pread(fd, &temp, sizeof(int), Offset);
		Offset += sizeof(int);
		printf("Bucket: %d, number of elements: %d\n",(i),temp);
		sizes[i] = temp;
	}
	bool break_cond = false;
 	for(Offset; Offset< FILESIZE; Offset += (sizeof(DataItem) + sizeof(int)))
 	{
		pread(fd,&buck,sizeof(int), Offset);
		if (buck > 0) 	break_cond = true;
		if (break_cond && buck == 0) break;
		
 		ssize_t result = pread(fd,&data,sizeof(DataItem), Offset + sizeof(int));
 		if(result < 0)
 		{ 	  perror("some error occurred in pread");
 			  return -1;
 		} 
 		else {
 			printf("Bucket: %d, key: %d, data: %d\n", buck, data.key, data.data);
 		}
 	}
 	
	printf("\n####### EOF #######\n\n");
 	return 0;
 }
 
 void WriteFile(int fd, vector<Bucket*>& Directory){
	int sz = Directory.size();
	int buck;
	pwrite(fd, &sz, sizeof(int), 0);
	int offset = sizeof(int);
	for (int i=0; i<(int)Directory.size(); i++){
		pwrite(fd, &Directory[i]->local_depth, sizeof(int), offset);
		offset += sizeof(int);
	}
	
	for (int i=0; i<(int)Directory.size(); i++){
		sz = (int)Directory[i]->data_array.size();
		pwrite(fd, &sz, sizeof(int), offset);
		offset += sizeof(int);
	}
	
	for (int i=0; i<(int)Directory.size(); i++){
		Bucket *buck = Directory[i];
		if (!buck->data_array.empty())
		{
			for (int j=0; j < (int)buck->data_array.size(); j++)
			{	
				
				pwrite(fd, &i, sizeof(int), offset);
				offset += sizeof(int);
				pwrite(fd, &(*buck->data_array[j]), sizeof(DataItem), offset);
				offset += sizeof(DataItem);
			}
		}
	}
 }
