#include "simplefs-ops.h"
extern struct filehandle_t file_handle_array[MAX_OPEN_FILES]; // Array for storing opened files

int simplefs_create(char *filename){
    /*
	    Create file with name `filename` from disk
		
	*/
	for(int i = 0; i < NUM_INODES; i++){
		struct inode_t inode;
		simplefs_readInode(i, &inode);
		if((inode.status == INODE_IN_USE || inode.status == INODE_SYMLINK) && (strcmp(inode.name, filename) == 0)){
			return -1;
		}
	}
	int ret_inode = simplefs_allocInode();
	if(ret_inode == -1){
		return -1;
	}
	
	struct inode_t inode;
	simplefs_readInode(ret_inode, &inode);
	inode.file_size = 0;
	inode.status = INODE_IN_USE;
	memset(inode.name, 0, MAX_NAME_STRLEN);
	strncpy(inode.name, filename, MAX_NAME_STRLEN - 1);
	simplefs_writeInode(ret_inode, &inode);
    return ret_inode;
}


void simplefs_delete(char *filename){
    /*
	    delete file with name `filename` from disk
	*/
	int found = -1;
	for(int i = 0; i < NUM_INODES; i++){
		struct inode_t inode;
		simplefs_readInode(i, &inode);
		if((inode.status == INODE_IN_USE) && (strcmp(inode.name, filename) == 0)){
			found = i;
			break;
		}
	}
	if(found == -1){
		return;
	}
	struct inode_t inode;
	simplefs_readInode(found, &inode);
	for(int i = 0; i < MAX_FILE_SIZE; i++){
		if(inode.direct_blocks[i] != -1){
			simplefs_freeDataBlock(inode.direct_blocks[i]);
		}
	}
	simplefs_freeInode(found);
}

int simplefs_open(char *filename){
    /*
	    open file with name `filename`
	*/
	int found = -1;
	struct inode_t req_inode; //Find the inode for the filename
	for(int i = 0; i < NUM_INODES; i++){
		struct inode_t inode;
		simplefs_readInode(i, &inode);
		if((inode.status == INODE_IN_USE || inode.status == INODE_SYMLINK) && (strcmp(inode.name, filename) == 0)){
			found = i;
			req_inode = inode;
			break;
		}
	}
	if(found == -1){
		return -1;
	}
	int handle = -1;
	if(req_inode.status == INODE_SYMLINK){
		//If inode is symlink then we need to get the src_file and open it
		int found1 = -1;
		char blk_data[BLOCKSIZE]; //Read the data block of the inode
		simplefs_readDataBlock(req_inode.direct_blocks[0], blk_data);
		int len; //Read the length of the src_filename which is stored as the first 4 bytes of the data block
		memcpy(&len, blk_data, sizeof(int));
		char src_filename[len + 1]; //Read the src_filename which is stored after the length of the src_filename
		memset(src_filename, 0, len + 1);
		memcpy(src_filename, blk_data + sizeof(int), len);
		for(int i = 0; i < NUM_INODES; i++){
			struct inode_t inode;
			simplefs_readInode(i, &inode);
			if((inode.status == INODE_IN_USE) && (strcmp(inode.name, src_filename) == 0)){
				found = i; //Get the inode of the src_filename and set it as the found 
				//inode which would be opened in the file handler
				found1 = i;
				break;
			}
		}
		if(found1 == -1){
			return -1;
		}
	}
	for(int i = 0; i < MAX_OPEN_FILES; i++){
		if(file_handle_array[i].inode_number < 0){
			handle = i;
			file_handle_array[i].inode_number = found;
			file_handle_array[i].offset = 0;
			break;
		}
	}
    return handle;
}

void simplefs_close(int file_handle){
    /*
	    close file pointed by `file_handle`
	*/
	file_handle_array[file_handle].inode_number = -1;
	file_handle_array[file_handle].offset = 0;

}

int simplefs_read(int file_handle, char *buf, int nbytes){
    /*
	    read `nbytes` of data into `buf` from file pointed by `file_handle` starting at current offset
	*/
	struct filehandle_t *fh = &(file_handle_array[file_handle]);
	if(fh->inode_number < 0){
		return -1;
	}
	struct inode_t inode;
	simplefs_readInode(fh->inode_number, &inode);
	if(inode.status == INODE_FREE){
		return -1;
	}
	if(fh->offset + nbytes > inode.file_size){
		return -1;
	}
	if(nbytes <= 0){
		return -1;
	}
	int num_blks = 0;
	for (int i = 0; i < MAX_FILE_SIZE; i++)
	{
		if(inode.direct_blocks[i] != -1){
			num_blks++;
		}
	}

	int blk = fh->offset / BLOCKSIZE;
	int off_in_blk = fh->offset % BLOCKSIZE;
	int bytes_left_to_read = nbytes;

	while((blk < num_blks) && (bytes_left_to_read > 0)){
		char blk_data[BLOCKSIZE];
		simplefs_readDataBlock(inode.direct_blocks[blk], blk_data);
		if(off_in_blk + bytes_left_to_read > BLOCKSIZE){
			memcpy(buf + (nbytes - bytes_left_to_read), blk_data + off_in_blk, BLOCKSIZE - off_in_blk);
			blk++;
			bytes_left_to_read -= (BLOCKSIZE - off_in_blk);
			off_in_blk = 0;
		}
		else{
			memcpy(buf + (nbytes - bytes_left_to_read), blk_data + off_in_blk, bytes_left_to_read);
			bytes_left_to_read = 0;
			break;
		}
	}
    return 0;
}


int simplefs_write(int file_handle, char *buf, int nbytes){
    /*
	    write `nbytes` of data from `buf` to file pointed by `file_handle` starting at current offset
	*/
	struct filehandle_t *fh = &(file_handle_array[file_handle]);
	if(fh->inode_number < 0){
		return -1;
	}
	struct inode_t inode;
	simplefs_readInode(fh->inode_number, &inode);
	if(inode.status == INODE_FREE){
		return -1;
	}
	if(nbytes + fh->offset > MAX_FILE_SIZE * BLOCKSIZE){
		return -1;
	}
	int num_blks = 0;
	for (int i = 0; i < MAX_FILE_SIZE; i++)
	{
		if(inode.direct_blocks[i] != -1){
			num_blks++;
		}
	}
	if((fh->offset % BLOCKSIZE == 0) && (fh->offset / BLOCKSIZE < MAX_FILE_SIZE) && (fh->offset == inode.file_size) && (inode.direct_blocks[fh->offset / BLOCKSIZE] == -1)){
		inode.direct_blocks[num_blks] = simplefs_allocDataBlock();
		if(inode.direct_blocks[num_blks] == -1){
			return -1;
		}
		char nulldata[BLOCKSIZE];
		memset(nulldata, 0, BLOCKSIZE);
		simplefs_writeDataBlock(inode.direct_blocks[num_blks], nulldata);
		num_blks++;
	}

	int blk = fh->offset / BLOCKSIZE;
	int off_in_blk = fh->offset % BLOCKSIZE;
	int nbytes_avail = (num_blks - (blk + 1)) * BLOCKSIZE + (BLOCKSIZE - off_in_blk);
	int nbytes_req = (nbytes - nbytes_avail > 0) ? nbytes - nbytes_avail : 0;
	int nblocks_req = nbytes_req/BLOCKSIZE + (nbytes_req % BLOCKSIZE ? 1 : 0);
	int new_blocks[nblocks_req];
	for(int i = 0; i < nblocks_req; i++){
		new_blocks[i] = -1;
	}
	for(int i = 0; i < nblocks_req; i++){
		new_blocks[i] = simplefs_allocDataBlock();
		if(new_blocks[i] < 0){
			goto bad;
		}
	}
	goto good;
bad:
	for(int i = 0; i < nblocks_req; i++){
		if(new_blocks[i] >= 0){
			simplefs_freeDataBlock(new_blocks[i]);
		}
	}
	return -1;
good:
	for(int i = 0; i < nblocks_req; i++){
		char nulldata[BLOCKSIZE];
		memset(nulldata, 0, BLOCKSIZE);
		simplefs_writeDataBlock(new_blocks[i], nulldata);
	}

	assert(nblocks_req + num_blks <= MAX_FILE_SIZE);
	for(int i = num_blks; i < nblocks_req + num_blks; i++){
		inode.direct_blocks[i] = new_blocks[i - num_blks];
	}
	int bytes_left_to_write = nbytes;
	while((blk < MAX_FILE_SIZE) && (bytes_left_to_write > 0)){
		char blk_data[BLOCKSIZE];
		simplefs_readDataBlock(inode.direct_blocks[blk], blk_data);
		if(off_in_blk + bytes_left_to_write > BLOCKSIZE){
			memcpy(blk_data + off_in_blk, buf + (nbytes - bytes_left_to_write) , BLOCKSIZE - off_in_blk);
			simplefs_writeDataBlock(inode.direct_blocks[blk], blk_data);
			blk++;
			bytes_left_to_write -= (BLOCKSIZE - off_in_blk);
			off_in_blk = 0;
		}
		else{
			memcpy(blk_data + off_in_blk, buf + (nbytes - bytes_left_to_write), bytes_left_to_write);
			simplefs_writeDataBlock(inode.direct_blocks[blk], blk_data);
			bytes_left_to_write = 0;
			break;
		}
	}
	inode.file_size = (fh->offset + nbytes > inode.file_size) ? (fh->offset + nbytes) : inode.file_size;
	simplefs_writeInode(fh->inode_number, &inode);
    return 0;
}


int simplefs_seek(int file_handle, int nseek){
    /*
	   increase `file_handle` offset by `nseek`
	*/
	struct filehandle_t* fh = &(file_handle_array[file_handle]);
	if(fh->inode_number < 0){
		return -1;
	}
	struct inode_t inode;
	simplefs_readInode(fh->inode_number, &inode);
	if(inode.status == INODE_FREE){
		return -1;
	}

	if(nseek < 0){
		fh->offset = (fh->offset + nseek > 0) ? fh->offset + nseek : 0;
	}
	else{
		fh->offset = (fh->offset + nseek < inode.file_size) ? (fh->offset + nseek) : (inode.file_size);
	}
	// printf("File Handle %d Offset: %d\n", file_handle, fh->offset);
    return 0;
}

int simplefs_symlink(char * src_filename, char * dest_filename){
	/*
	    create symlink `dest_filename` to `src_filename`
	*/
	int src_found = 0;
	for(int i = 0; i < NUM_INODES; i++){
		struct inode_t inode;
		simplefs_readInode(i, &inode);
		if(strcmp(inode.name, dest_filename) == 0){
			return -1;
		}
		if(strcmp(inode.name, src_filename) == 0)
		{
			src_found = 1;
		}
		if(strcmp(inode.name, src_filename) == 0 && inode.status == INODE_SYMLINK)return -1;
	}
	if(src_found == 0)return -1;
	
	int ret_inode = simplefs_allocInode();
	if(ret_inode == -1){
		return -1;
	}
	
	struct inode_t inode;
	simplefs_readInode(ret_inode, &inode);
	int count = 0;
	while(src_filename[count] != '\0')
	{
		count++;
	}

	inode.file_size = sizeof(int)+count+1;
	inode.status = INODE_SYMLINK;
	memset(inode.name, 0, MAX_NAME_STRLEN);
	strncpy(inode.name, dest_filename, MAX_NAME_STRLEN - 1);
	simplefs_writeInode(ret_inode, &inode);

	inode.direct_blocks[0] = simplefs_allocDataBlock();

	char blk_data[BLOCKSIZE];
	memcpy(blk_data, &count , sizeof(int));
	memcpy(blk_data + sizeof(int), src_filename, count+1);
	simplefs_writeDataBlock(inode.direct_blocks[0], blk_data);	
	simplefs_writeInode(ret_inode, &inode);

  return ret_inode;
}

int simplefs_unlink(char * filename){
	/*
	    delete symlink or file with name `filename` from disk
	*/
	
	int found = 0;
	int store_int;
	struct inode_t store_inode;
	for(int i = 0; i < NUM_INODES; i++){
		struct inode_t inode;
		simplefs_readInode(i, &inode);	
		if(strcmp(inode.name, filename) == 0)
		{
			found = 1;
			store_inode = inode;
			store_int = i;
		}
		if(strcmp(inode.name, filename) == 0 && inode.status != INODE_SYMLINK)
		{
			return -1;
		}
	}
	if(found == 0)return -1;

	simplefs_freeDataBlock(store_inode.direct_blocks[0]);
	simplefs_freeInode(store_int);


  return 0;
}
