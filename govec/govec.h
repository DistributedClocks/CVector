#include "./vclock/vclock.h"
#include "./cwpack/src/cwpack.h"
#include "./cwpack/src/cwpack_defines.h"
#include <sys/stat.h>
#include <libgen.h>


#define FILE_MAX 256 
//This is the Global Variable Struct that holds all the info needed to be maintained
struct goLog {

	char pid[30]; 	//Local Process ID
	//Local vector clock in bytes
	struct vectorClock *vc;

	//Flag to Printf the logs made by Local Program
	int printonscreen;
	// This bool checks whether we should send logging data to a vecbroker whenever
	// a message is logged.
	int realtime;
	//activates/deactivates printouts at each preparesend and unpackreceive
	int debugmode;
	int logging;
	//This bools Checks to send the VC Bundled with User Data on the Wire
	// if False, PrepareSend and UnpackReceive will simply forward their input
	//buffer to output and locally log event. If True, VC will be encoded into packet on wire
	int VConWire;
	char logfile[FILE_MAX];	//Logfile name
	// Publisher to enable sending messages to a vecbroker.
/*	publisher *GoPublisher

	logger *log.Logger

	mutex sync.RWMutex*/
};
//This is the data structure that is actually end on the wire
struct clockPayload {
	char * pid;
	struct vectorClock * vc;
	char * payload;
};

struct goLog *initialize(char * pid, char * logName);
cw_pack_context *prepareSend(struct goLog *gl, char * msg, char *buf);
struct vectorClock *unpackReceive(cw_pack_context * pc);
