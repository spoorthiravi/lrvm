#include "rvm.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<iostream>
/**
 *@author Spoorthi Ravi
 */

/**
 *@file rvm.cpp
 */
 
/**
 *@brief Get Web Content from Cache
 *@param URL
 *@returns Web Content
 */
int numberOfDirectories=0;
int numOfSegments = 0;
/**
 *@brief Initialize the library with the specified directory as backing store
 *@param directory name
 *@returns rvm_t object;
 */
rvm_t rvm_init(const char *directory){
	int status;
	rvm_t rvm;
	rvm.ID = 1;
	rvm.directory = directory;
	status = mkdir(directory,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if(status == 0){
	 	numberOfDirectories++;
		return rvm;
	}
	else{
		exit(-1);
	}

}

/**
 *@brief map a segment from disk into memory.If the segment does not already exist, then create it and give it size size_to_create. If the segment exists but is shorter than size_to_create, then extend it until it is long enough. It is an error to try to map the same segment twice.
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */
void *rvm_map(rvm_t rvm, const char *segname, int size_to_create){
	for(vector<Segment>::size_type i = 0; i != rvm.segmentList.size(); i++){
		if(strcmp(segname,rvm.segmentList[i].segmentName) == 0){
			if(size_to_create == rvm.segmentList[i].segmentSize){
				segmentList[i].mapped = true;
			}
			else{
				rvm.segmentList[i].segmentData = realloc(rvm.segmentList[i].segmentData,size_to_create);
				rvm.segmentList[i].segmentSize = size_to_create;
				segmentList[i].mapped = true;
			}
			if(map[segmentList[i].segmentName] != 1){
                        	map[segmentList[i].segmentName] = 1;
                        }
			rvm.segmentList[i].undoLog.logData = (void*)malloc(sizeof(size_to_create));
			return rvm.segmentList[i].segmentData;


		}
	}
	segment newSegment = (segment*)malloc(sizeof(segment));
	newSegment.segmentName = segname;
	newSegment.segmentData = (void*)malloc(sizeof(size_to_create));
	newSegment.segmentSize = size_to_create;
	newSegment.mapped = true;
	map[newSegment.segmentName] = 1;
	rvm.segmentList.push_back(newSegment);	
	return newSegment.segmentData;
}
/**
 *@brief unmap a segment from memory.
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */
void rvm_unmap(rvm_t rvm, void *segbase){
	rvm->map.erase(char(segbase));
}

//destroy a segment completely, erasing its backing store. This function should not be called on a segment that is currently mapped.
/**
 *@brief map a segment from disk into memory
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */
void rvm_destroy(rvm_t rvm, const char *segname){ 
	for(vector<Segment>::size_type i = 0; i != rvm.segmentList.size(); i++){
		if(strcmp(rvm.segmentList[i].segmentName,segname)==0 && rvm.segmentList[i].mapped == false){
			segmentList.erase(list.begin() + i);
			free();
			return;
		}
	}
	cout << "wrong segname or segment is a mapped segment";
	return;
}
/**
 *@brief begin a transaction that will modify the segments listed in segbases. If any of the specified segments is already being modified by a transaction, then the call should    fail and return (trans_t) -1. Note that trant_t needs to be able to be typecasted to an integer type.
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */
trans_t rvm_begin_trans(rvm_t rvm, int numsegs, void **segbases){
	if(**segnames == )	

}

/**
 *@brief eclare that the library is about to modify a specified range of memory in the specified segment. The segment must be one of the segments specified in the call to rvm_begin_trans. Your library needs to ensure that the old memory has been saved, in case an abort is executed. It is legal call rvm_about_to_modify multiple times on the same memory area.
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */
void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size){





}
/**
 *@brief  commit all changes that have been made within the specified transaction. When the call returns, then enough information should have been saved to disk so that, even if the program crashes, the changes will be seen by the program when it restarts.
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */
void rvm_commit_trans(trans_t tid){



}
/**
 *@brief undo all changes that have happened within the specified transaction.
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */
void rvm_abort_trans(trans_t tid){



}
/**
 *@brief play through any committed or aborted items in the log file(s) and shrink the log file(s) as much as possible.
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */
void rvm_truncate_log(rvm_t rvm){





}


/* proc1 writes some data, commits it, then exits */
/*void proc1()
{
     rvm_t rvm;
     trans_t trans;
     char* segs[1];

     rvm = rvm_init("rvm_segments");
     //rvm_destroy(rvm, "testseg");
     /*segs[0] = (char *) rvm_map(rvm, "testseg", 10000);

     
     trans = rvm_begin_trans(rvm, 1, (void **) segs);
     
     rvm_about_to_modify(trans, segs[0], 0, 100);
     sprintf(segs[0], TEST_STRING);
     
     rvm_about_to_modify(trans, segs[0], OFFSET2, 100);
     sprintf(segs[0]+OFFSET2, TEST_STRING);
     
     rvm_commit_trans(trans);
     abort();
}
*/
/*int main(){
     int pid;

     pid = fork();
     if(pid < 0) {
      perror("fork");
      exit(2);
     }
    printf("in main");
     if(pid == 0) {
      proc1();
      exit(0);
     }

    // waitpid(pid, NULL, 0);

     //proc2();

     return 0;
}*/
