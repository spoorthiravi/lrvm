#include "rvm.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<errno.h>
#include<iostream>
#include<fstream>
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
rvm_t RVM;
vector<transaction*> globalTransactionList;
int TID = 0;
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
    rvm.rvmID = 1;
    rvm.transactionList;
    rvm.segmentList;
    rvm.directoryName = directory;
    status = mkdir(directory,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(status == 0){
        numberOfDirectories++;
        RVM = rvm;
        return rvm;
    } else{
        exit(-1);
    }
}

/**
 *@brief map a segment from disk into memory.If the segment does not already exist, then create it and give it size size_to_create. If the segment exists but is shorter than size_to_create, then extend it until it is long enough. It is an error to try to map the same segment twice.
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */
void *rvm_map(rvm_t rvm, const char *segname, int size_to_create){

    string contents;
    //char* data;    
    cout << "entering func: rvm_map\n";
    //string directoryName = string(rvm.directoryName);
    string homeDirectory = "/home/spurthi/spoorthi/CS6210/project4/";
    string pathToFile = homeDirectory + string(rvm.directoryName) + "/" + string(segname);
    cout << pathToFile << "\n";
    struct stat st;
    stat(pathToFile.c_str(), &st);

    FILE* fptr = fopen(pathToFile.c_str(), "rw+");
    if(fptr == NULL){
        cout<< "creating new file\n";
        ofstream ofs(pathToFile.c_str(), std::ios::binary | std::ios::out);
        ofs.seekp(size_to_create - 1);
        ofs.write("", 1);
        segment *newSegment = (segment*)malloc(sizeof(segment));
        newSegment->segmentName = segname;
        newSegment->segmentData = (void*)malloc(sizeof(size_to_create));
        newSegment->segmentSize = size_to_create;
        newSegment->mapped = true;
        newSegment->beingModified = false;
        rvm.segmentList.push_back(newSegment);
        RVM.segmentList.push_back(newSegment);
        cout << "segmentList size = " << rvm.segmentList.size() << "\n";
        cout << "exiting func: rvm_map\n";
        return newSegment->segmentData;
    }
    else{
        for(vector<segment*>::size_type i = 0; i != rvm.segmentList.size(); i++){
            if(strcmp(segname,rvm.segmentList[i]->segmentName) == 0){
                exit(-1);
            }
        }
        int fileSize = st.st_size;
        if(fileSize == size_to_create){
            ifstream in(pathToFile.c_str());
            string temp((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
            contents = temp;
        } else {
            int fd = fileno(fptr);
            cout << st.st_size;
            if(ftruncate(fd,size_to_create)==0){
                cout<< "success\n";
                cout << st.st_size;
            }
            else {
                 printf ("Error opening file unexist.ent: %s\n",strerror(errno));
            }
            ifstream in(pathToFile.c_str());
            string temp((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
            contents = temp;
        }        
    }
    segment *newSegment = (segment*)malloc(sizeof(segment));
    newSegment->segmentName = segname;
    newSegment->segmentData = (void*)contents.c_str();
    newSegment->segmentSize = size_to_create;
    newSegment->mapped = true;
    newSegment->beingModified = false;
    rvm.segmentList.push_back(newSegment);
    RVM.segmentList.push_back(newSegment);
    cout << "segmentList size = " << rvm.segmentList.size() << "\n";
    cout << "exiting func: rvm_map\n";
    return newSegment->segmentData;
}
    
/**
 *@brief unmap a segment from memory.
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */
void rvm_unmap(rvm_t rvm, void *segbase){
    cout << "entered func: rvm_unmap\n";
    for(vector<Segment>::size_type i = 0; i != rvm.segmentList.size(); i++){
        if(segbase == rvm.segmentList[i]->segmentData){
            rvm.segmentList.erase(rvm.segmentList.begin()+i);
            RVM.segmentList.erase(rvm.segmentList.begin()+i);
        }
    }
    cout << "segmentList size =" << rvm.segmentList.size() << "\n";
    cout << "exiting func: rvm_unmap\n";
}

/**
 *@brief destroy a segment completely, erasing its backing store. This function should not be called on a segment that is currently mapped.
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */
void rvm_destroy(rvm_t rvm, const char *segname){ 
    string directoryName = string(rvm.directoryName);
    string pathToFile = directoryName + "/" + string(segname);
    for(vector<Segment>::size_type i = 0; i != rvm.segmentList.size(); i++){
        if(strcmp(rvm.segmentList[i]->segmentName,segname)==0 && rvm.segmentList[i]->mapped == false){
            if(remove(pathToFile.c_str()) != 0 )
                perror( "Error deleting file" );
            else
                puts( "File successfully deleted" );
            return;
        }
        else if(strcmp(rvm.segmentList[i]->segmentName,segname)==0 && rvm.segmentList[i]->mapped != false){
            cout<< "segment is mapped\n";
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
    cout << "entering func: rvm_begin_trans\n";
    //vector<segment*> listOfSegments = getSegments(rvm,segbases,numsegs);
    vector<segment*> listOfSegments;    
    for(int j = 0;j < numsegs;j++){
        for(vector<segment*>::size_type i = 0; i != rvm.segmentList.size(); i++){
            if(segbases[j] == rvm.segmentList[i]->segmentData){
                listOfSegments.push_back(rvm.segmentList[i]);
            }
        }
    }

    for(vector<segment*>::size_type k = 0; k != listOfSegments.size(); k++){
        if(listOfSegments[k]->beingModified){
            return -1;
        }
    }
    transaction *newTransaction = (transaction*)malloc(sizeof(transaction));
    newTransaction->transactionID = TID;
    newTransaction->numOfSegs = numsegs;
    newTransaction->undoLogList;
    newTransaction->segbases = segbases;
    globalTransactionList.push_back(newTransaction);
    rvm.transactionList.push_back(newTransaction);
    TID++;
    cout << "exiting func: rvm_bein_trans\n";
    return newTransaction->transactionID;
}

/**
 *@brief eclare that the library is about to modify a specified range of memory in the specified segment. The segment must be one of the segments specified in the call to rvm_begin_trans. Your library needs to ensure that the old memory has been saved, in case an abort is executed. It is legal call rvm_about_to_modify multiple times on the same memory area.
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */
void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size){
    //with tid get transaction
    cout << "entering func: rvm_about_to_modify\n";
    segment *Segment = (segment*)malloc(sizeof(segment));
    transaction *newTransaction = (transaction*)malloc(sizeof(transaction));
    for(vector<transaction*>::size_type i = 0; i != globalTransactionList.size(); i++){
        if(globalTransactionList[i]->transactionID == tid){
            newTransaction = globalTransactionList[i];
            cout << "newTransaction id = " << newTransaction->transactionID << "\n";
        }
    }
    for(int i = 0; i < newTransaction->numOfSegs; i++){
        if(newTransaction->segbases[i] == segbase){
            cout << "enters first if with " << RVM.segmentList.size() << "\n";
            for(vector<segment*>::size_type k = 0; k != RVM.segmentList.size(); k++){
                cout << "RVMsegmentList size = " << RVM.segmentList.size() << "\n";
                if(segbase == RVM.segmentList[k]->segmentData){
                    cout << "enters second if\n";
                    Segment = RVM.segmentList[k];
                    RVM.segmentList[k]->beingModified = true;
                }
            }
            void *backup = malloc(size +1);
            memcpy(backup, (segbase + offset), size);
            string originalString = string((char*)(segbase));
            log *undoRecord = (log*) malloc(sizeof(log));
            undoRecord->size = size;
            undoRecord->offset = offset;
            undoRecord->data = backup;
            undoRecord->segmentName = Segment->segmentName;
            cout << "WTF\n";
            newTransaction->undoLogList.push_back(undoRecord);            
            cout << "Done with segbase" << "\n"; 
        }
        cout << "i=" << i<< "\n";
    }
    cout << "undoLogList size = " << newTransaction->undoLogList.size() << "\n";
    cout << "exiting func: rvm_about_to_modify\n";

}
/**
 *@brief  commit all changes that have been made within the specified transaction. When the call returns, then enough information should have been saved to disk so that, even if the program crashes, the changes will be seen by the program when it restarts.
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */
void rvm_commit_trans(trans_t tid){
    cout << "entering func: rvm_commit_trans\n";
    transaction *Transaction = (transaction*)malloc(sizeof(transaction));
    //segment *Segment = (segment*)malloc(sizeof(segment));
    for(vector<transaction*>::size_type i = 0; i != globalTransactionList.size(); i++){
        if(globalTransactionList[i]->transactionID == tid){
            Transaction = globalTransactionList[i];
        }
    }
    for(int j = 0;j<Transaction->numOfSegs;j++){
        for(vector<segment*>::size_type k = 0; k != RVM.segmentList.size(); k++){
            if(Transaction->segbases[j] == RVM.segmentList[k]->segmentData){
                RVM.segmentList[k]->beingModified = false;
                string filename = RVM.segmentList[k]->segmentName;
                string directoryName = RVM.directoryName;
                string pathToFile = directoryName + "/" + filename;
                cout << "pathToFile =" << pathToFile;
                ofstream outfile(pathToFile.c_str(),ofstream::binary);
                outfile.write ((char*)Transaction->segbases[j],sizeof(Transaction->segbases[j]));
            }
        }
    }

    //remove all enries from undo log record
    Transaction->undoLogList.clear();
    cout << "undoLogList size =" << Transaction->undoLogList.size() << "\n";
    cout << "exiting func: rvm_commit_trans\n";

}
/**
 *@brief undo all changes that have happened within the specified transaction.
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */
void rvm_abort_trans(trans_t tid){
    transaction *Transaction = (transaction*)malloc(sizeof(transaction));
    segment *Segment = (segment*)malloc(sizeof(segment));
    for(vector<transaction*>::size_type i = 0; i != globalTransactionList.size(); i++){
        if(globalTransactionList[i]->transactionID == tid){
            Transaction = globalTransactionList[i];
        }
    }
    for(int j = 0;j<Transaction->numOfSegs;j++){
        for(vector<segment*>::size_type k = 0; k != RVM.segmentList.size(); k++){
            if(Transaction->segbases[j] == RVM.segmentList[k]->segmentData){
                Segment = RVM.segmentList[k];
                Segment->beingModified = false;
                for(vector<log*>::size_type l = 0; l !=Transaction->undoLogList.size(); k++){
                    if(Segment->segmentName == Transaction->undoLogList[l]->segmentName){
                        string undoRecordData = string((char*)Transaction->undoLogList[l]->data);
                        undoRecordData.copy((char*)Segment->segmentData,Transaction->undoLogList[l]->size,Transaction->undoLogList[l]->offset);
                    }
                }
            }
        }
    }
    return;

}
/**
 *@brief play through any committed or aborted items in the log file(s) and shrink the log file(s) as much as possible.
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */
void rvm_truncate_log(rvm_t rvm){





}

/*vector<segment*> getSegment(rvm_t rvm,void **segbases,int numsegs){
    vector<segment*> listOfSegments; 
    for(int j = 0;j < numsegs;j++){
         for(vector<segment*>::size_type i = 0; i != rvm.segmentList.size(); i++){
                    if(segbases[j] == rvm.segmentList[i]->segmentData){
                            listOfSegments.push_back(rvm.segmentList[i]);
                        }
                }
    }
return listOfSegments;
}*/

/* proc1 writes some data, commits it, then exits */
/*void proc1()
{
     rvm_t rvm;
     trans_t trans;
     char* segs[1];

     rvm = rvm_init("rvm_segments");
     //rvm_destroy(rvm, "testseg");
     segs[0] = (char *) rvm_map(rvm, "testseg", 10000);

     
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
    //printf("in main \n");
     if(pid == 0) {
     rvm_t rvm;
     trans_t trans;
     char* segs[2];

      rvm = rvm_init("rvm_segments");
     cout<< "just before calling map\n";
     //rvm_destroy(rvm, "testseg");
      segs[0] = (char*) rvm_map(rvm, "test3.txt", 30);
    cout<<"finished map\n";
      cout << segs[0];
      rvm_unmap(rvm,segs[0]);
    segs[1] =  (char*)rvm_map(rvm,"test3.txt",100);
     //segs[0] = (char*)rvm_map(
     // cout << rvm.segmentList << "\n";
      exit(0);
     }

    // waitpid(pid, NULL, 0);

     //proc2();

     return 0;
}*/
