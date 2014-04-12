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
typedef struct rvm_t
{
int rvmID;
const char* directoryName;
vector<segment*> segmentList;
}rvm_t;
