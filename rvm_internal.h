#define trans_t int
#include <string>
#include <vector>
//typedef int trans_t;
using namespace std;
typedef struct Log{
	int size;
	void* data;
	struct segment;
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
	int numOfSegs;
	vector<log*> undoLogList;	
}transaction;

typedef struct rvm_t
{
int rvmID;
const char* directoryName;
vector<segment*> segmentList;
vector<transaction*> transactionList;
}rvm_t;

//vector<segment*> getSegments(rvm_t rvm,void **segbases,int numsegs);
