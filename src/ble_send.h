#ifndef BLE_SEND__
#define BLE_SEND__


#define MESSAGE_NUM 10

extern uint8_t notify_ble_resp_on1;

struct resp_result_s {
    void *fifo_reserved;
    float resultset[MESSAGE_NUM];
};

extern void ble_notify_respiratory_proc(struct resp_result_s * dataitem);

extern struct k_fifo resp_data_queue;




#endif
