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
vector<transaction> globalTransactionList;
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
    struct stat st;
    stat(pathToFile.c_str(), &st);

    FILE* fptr = fopen(pathToFile.c_str(), "rw+");
    if(fptr == NULL){
        ofstream ofs(pathToFile.c_str(), std::ios::out);
        ofs.seekp(size_to_create - 1);
        ofs.write("", 1);
        segment newSegment;
        newSegment.segmentName = segname;
        newSegment.segmentData = (void*)malloc(sizeof(size_to_create));
        newSegment.segmentSize = size_to_create;
        newSegment.mapped = true;
        newSegment.beingModified = false;
        rvm.segmentList.push_back(newSegment);
        RVM.segmentList.push_back(newSegment);
        cout << "exiting func: rvm_map\n";
        return newSegment.segmentData;
    }
    else{
        for(vector<segment*>::size_type i = 0; i != rvm.segmentList.size(); i++){
            if(strcmp(segname,rvm.segmentList[i].segmentName) == 0){
                exit(-1);
            }
        }
        int fileSize = st.st_size;
        if(fileSize == size_to_create){
            ifstream in(pathToFile.c_str());
            string temp((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
            contents = temp;
	    in.close();
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
            in.close();
        }        
    }
    segment newSegment;
    newSegment.segmentName = segname;
    newSegment.segmentData = (void*)contents.c_str();
    newSegment.segmentSize = size_to_create;
    newSegment.mapped = true;
    newSegment.beingModified = false;
    rvm.segmentList.push_back(newSegment);
    RVM.segmentList.push_back(newSegment);
    cout << "exiting func: rvm_map\n";
    fclose(fptr);
    return newSegment.segmentData;
}
    
/**
 *@brief unmap a segment from memory.
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */
void rvm_unmap(rvm_t rvm, void *segbase){
    cout << "entered func: rvm_unmap\n";
    for(vector<Segment>::size_type i = 0; i != rvm.segmentList.size(); i++){
        if(segbase == rvm.segmentList[i].segmentData){
            rvm.segmentList.erase(rvm.segmentList.begin()+i);
            RVM.segmentList.erase(rvm.segmentList.begin()+i);
        }
    }
    cout << "segmentList size = " << rvm.segmentList.size() << "\n";
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
        if(strcmp(rvm.segmentList[i].segmentName,segname)==0 && rvm.segmentList[i].mapped == false){
            if(remove(pathToFile.c_str()) != 0 )
                perror( "Error deleting file" );
            else
                puts( "File successfully deleted" );
            return;
        }
        else if(strcmp(rvm.segmentList[i].segmentName,segname)==0 && rvm.segmentList[i].mapped != false){
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
    vector<segment> listOfSegments;    
    for(int j = 0; j < numsegs; j++){
        for(vector<segment>::size_type i = 0; i != rvm.segmentList.size(); i++){
            if(segbases[j] == rvm.segmentList[i].segmentData){
                listOfSegments.push_back(rvm.segmentList[i]);
            }
        }
    }

    for(vector<segment>::size_type k = 0; k != listOfSegments.size(); k++){
        if(listOfSegments[k].beingModified){
            return -1;
        }
    }
    transaction newTransaction;
    newTransaction.transactionID = TID;
    newTransaction.numOfSegs = numsegs;
    newTransaction.undoLogList.clear();
    newTransaction.segbases = segbases;
    globalTransactionList.push_back(newTransaction);
    rvm.transactionList.push_back(newTransaction);
    TID++;
    cout << "exiting func: rvm_bein_trans\n";
    return newTransaction.transactionID;
}

/**
 *@brief eclare that the library is about to modify a specified range of memory in the specified segment. The segment must be one of the segments specified in the call to rvm_begin_trans. Your library needs to ensure that the old memory has been saved, in case an abort is executed. It is legal call rvm_about_to_modify multiple times on the same memory area.
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */
void rvm_about_to_modify(trans_t tid, void *segbase, int offset, int size){
    //with tid get transaction
    cout << "entering func: rvm_about_to_modify\n";
    segment Segment;
    cout << "After mallocing segment\n";
    int currentIndex = 0;
    //transaction *newTransaction = (transaction*)malloc(sizeof(transaction));
    transaction newTransaction;
    for(vector<transaction>::size_type i = 0; i != globalTransactionList.size(); i++){
        if(globalTransactionList[i].transactionID == tid){
            newTransaction = globalTransactionList[i];
            cout << "newTransaction id = " << newTransaction.transactionID << "\n";
            cout << "undoLogList size = " << newTransaction.undoLogList.size() << "\n";
            currentIndex = i;
            break;
        }
    }
    for(int i = 0; i < newTransaction.numOfSegs; i++){
        if(newTransaction.segbases[i] == segbase){
            cout << "enters first if with " << RVM.segmentList.size() << "\n";
            for(vector<segment>::size_type k = 0; k != RVM.segmentList.size(); k++){
                if(segbase == RVM.segmentList[k].segmentData){
                    cout << "enters second if \n";
                    Segment = RVM.segmentList[k];
                    RVM.segmentList[k].beingModified = true;
                    break;
                }
            }
            void *backup = malloc(size +1);
            memcpy(backup, (segbase + offset), size);
            string originalString = string((char*)(segbase));
            log *undoRecord = (log*)malloc(sizeof(log));
            undoRecord->size = size;
            undoRecord->offset = offset;
            undoRecord->data = backup;
            undoRecord->segmentName = Segment.segmentName;
            newTransaction.undoLogList.push_back(undoRecord);    
            cout << "After updating undoLogList size = " << newTransaction.undoLogList.size() << "\n";        
            cout << "Done with segbase" << "\n";
            globalTransactionList.erase(globalTransactionList.begin() + currentIndex);
            globalTransactionList.insert((globalTransactionList.begin() + currentIndex), newTransaction); 
            //free(backup); 
            break;
        }
    }
    cout << "exiting func: rvm_about_to_modify\n";

}
/**
 *@brief  commit all changes that have been made within the specified transaction. When the call returns, then enough information should have been saved to disk so that, even if the program crashes, the changes will be seen by the program when it restarts.
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */

void rvm_commit_trans(trans_t tid){
    int index;
    cout << "entering func: rvm_commit_trans\n";
    transaction Transaction;
    //segment *Segment = (segment*)malloc(sizeof(segment));
    for(vector<transaction>::size_type i = 0; i != globalTransactionList.size(); i++){
        if(globalTransactionList[i].transactionID == tid){
            Transaction = globalTransactionList[i];
            index =(int) i;
            globalTransactionList.erase(globalTransactionList.begin()+index);
	        break;
        }
    }

    //cout<< "sebases size = " << sizeof(Transaction.segbases)<<"\n";
    for(int j = 0;j<Transaction.numOfSegs;j++){
        for(vector<segment>::size_type k = 0; k != RVM.segmentList.size(); k++){
            if(Transaction.segbases[j] == RVM.segmentList[k].segmentData){
                RVM.segmentList[k].beingModified = false;
                //string filename = RVM.segmentList[k].segmentName;
                //cout << filename << "\n";
                //string directoryName = RVM.directoryName;
                //cout << directoryName << "\n";
                //string pathToFile = directoryName + "/" + filename;
		string homeDirectory = "/home/spurthi/spoorthi/CS6210/project4/";
                string pathToFile = homeDirectory + RVM.directoryName + string("/") + string(RVM.segmentList[k].segmentName);
                cout << "pathToFile = " << pathToFile << "\n";
		FILE *fp;
		fp = fopen(pathToFile.c_str(), "wb");
		cout << "file opened successfully\n";
  		fwrite ((char*)Transaction.segbases[j],sizeof(char),RVM.segmentList[k].segmentSize,fp);
		cout << "write completed \n";
  		fclose (fp);
                //ofstream outfile(pathToFile.c_str(),ofstream::binary);
                //outfile.write ((char*)Transaction.segbases[j],RVM.segmentList[k].segmentSize);
                cout << "Finished writing to the file \n";
            }
        }
    }

    //remove all enries from undo log record
    Transaction.undoLogList.clear();
    globalTransactionList.insert((globalTransactionList.begin() + index),Transaction);
    cout << "undoLogList size =" << Transaction.undoLogList.size() << "\n";
    cout << "exiting func: rvm_commit_trans\n";

}
/**
 *@brief undo all changes that have happened within the specified transaction.
 *@param rvm, segmnet name, size of segment to be created
 *@returns void* (data contained in the segment)
 */
void rvm_abort_trans(trans_t tid){
    cout << "In abort \n";
  /*  transaction *Transaction = (transaction*)malloc(sizeof(transaction));
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
*/
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
}*/

/*int main(){
     int pid;

     pid = fork();
     if(pid < 0) {
      perror("fork");
      exit(2);
     }
    printf("in main \n");
     if(pid == 0) {
     rvm_t rvm;
     trans_t trans;
	char* segs[1];

     rvm_init("rvm_segments");
     //rvm_destroy(rvm, "testseg");
     segs[0] = (char *) rvm_map(rvm, "testseg", 10000);

     
     trans = rvm_begin_trans(rvm, 1, (void **) segs);
     
     rvm_about_to_modify(trans, segs[0], 0, 100);
     sprintf(segs[0], "hello world");
     
     rvm_about_to_modify(trans, segs[0], 1000, 100);
     sprintf(segs[0]+1000, "hello world");
     
     rvm_commit_trans(trans);
     abort();
    }

    // waitpid(pid, NULL, 0);

     //proc2();

     return 0;
}*/
