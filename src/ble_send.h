#ifndef BLE_SEND__
#define BLE_SEND__




extern uint8_t notify_ble_resp_on1;

extern struct k_queue resp_data_queue;

struct resp_result_s {
    void *next;
    float resultset[];
};


#endif
