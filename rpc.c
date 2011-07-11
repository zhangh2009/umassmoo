#include "rpc.h"

unsigned char rpc_cmd = 0;
unsigned char rpc_param = 0;
unsigned char rpc_in_progress = 0;
unsigned int rpc_retval = 0;
rpc_dispatch_table_entry rpc_dispatch_table[RPC_DISPATCH_TABLE_SIZE];

void rpc_init (void) {
    rpc_dispatch_table[0].id = 1;
    rpc_dispatch_table[0].fn = &rpc_beep;

    rpc_dispatch_table[0].id = 66;
    rpc_dispatch_table[0].fn = &rpc_kill;

    // XXX more RPCs
}

void rpc_dispatch (void) {
    // look up rpc_cmd in the dispatch table
    // if not in dispatch table, reset RPC globals to 0; do nothing

    // execute the function named by rpc_cmd
    // stuff its return value in rpc_retval
}

unsigned int rpc_beep (void) {
    return 0; // XXX
}

unsigned int rpc_kill (void) {
    return 0; // XXX
};