#ifndef RPC_H
#define RPC_H

#define RPC_DISPATCH_TABLE_SIZE 4

extern unsigned char rpc_cmd;
extern unsigned char rpc_param;
extern unsigned char rpc_in_progress;
extern unsigned int rpc_retval;

typedef struct {
    unsigned char id;
    unsigned int (*fn)(void);
} rpc_dispatch_table_entry;

extern rpc_dispatch_table_entry rpc_dispatch_table[RPC_DISPATCH_TABLE_SIZE];

void rpc_init (void);
void rpc_dispatch (void);

unsigned int rpc_beep (void);
unsigned int rpc_kill (void);

#endif // RPC_H