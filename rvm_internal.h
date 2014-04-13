#define trans_t long int
#include <string>
#include <vector>

using namespace std;
typedef struct Log{
	int logID;
	int logSize;
	char* logData;

}log;
typedef struct Segment{
	const char* segmentName;
	int segmentSize;
	void* segmentData;
	bool mapped;
	bool beingModified;
	Log undoLog;
}segment;
typedef struct transaction{
	trans_t transactionID;
	void **segbases;	
}transaction;

typedef struct rvm_t
{
int rvmID;
const char* directoryName;
vector<segment*> segmentList;
vector<transaction> transactionList;
}rvm_t;
