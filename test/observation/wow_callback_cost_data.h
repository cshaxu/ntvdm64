#ifndef WOW_CALLBACK_COST_DATA_H
#define WOW_CALLBACK_COST_DATA_H
#define CALLBACK_COST_MAGIC 0x447c0b11u
#define CALLBACK_COST_CAPACITY 64u
typedef struct callback_cost_row {
    volatile LONG complete;
    DWORD window,message,thread;
    LONGLONG entered,returned,phase_entered,phase_returned;
    LONG accepted,result;
    DWORD error;
    LONG normal;
    LONG phase_count,reserved;
} callback_cost_row;
typedef struct callback_cost_data {
    DWORD magic,version,capacity;
    volatile LONG reserved;
    LONGLONG frequency;
    callback_cost_row rows[CALLBACK_COST_CAPACITY];
} callback_cost_data;
typedef char callback_cost_row_layout[(sizeof(callback_cost_row)==72)?1:-1];
typedef char callback_cost_data_layout[(sizeof(callback_cost_data)==4632)?1:-1];
#endif
