#include "govec.h"
/*
 * Print reverse byte buffer including specified length
 */

int main(int argc, char *argv[]) {

	struct goLog *gl = initialize("client","mylogfile");
	char hello[5000];
	cw_pack_context * result = prepareSend(gl, "blah", hello);
	tick(&gl->vc, gl->pid);
	struct vectorClock *vc1 = unpackReceive(result);
	cw_pack_context * gesult = prepareSend(gl, "blah", hello);
	struct vectorClock *vc2 = unpackReceive(result);

	printVC(vc1);
	printVC(vc2);
}