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
    unsigned i;

    for (i = 0; i < RPC_DISPATCH_TABLE_SIZE; ++i) {
        if (rpc_dispatch_table[i].id == rpc_cmd) {
            break;
        }
    }
    // if not in dispatch table, reset RPC globals to 0; do nothing
    if (i == RPC_DISPATCH_TABLE_SIZE) {
        rpc_cmd = rpc_param = rpc_in_progress = rpc_retval = 0;
        return;
    }

    // execute the function named by rpc_cmd; stuff its return value into
    // rpc_retval
    rpc_retval = (*(rpc_dispatch_table[i].fn))();
}

unsigned int rpc_beep (void) {
    return 0; // XXX
}

unsigned int rpc_kill (void) {
    return 0; // XXX
};
