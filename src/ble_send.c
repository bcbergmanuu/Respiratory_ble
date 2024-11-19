#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>

#include <zephyr/settings/settings.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include <zephyr/logging/log.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "adc_control.h"
#include "respiratory.pb.h"
#include "ble_send.h"



LOG_MODULE_REGISTER(BLE_MODULE, CONFIG_LOG_DEFAULT_LEVEL);

uint8_t notify_ble_resp_on1 =0;

//general service
static struct bt_uuid_128 respiratory_general_prop = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0xa5c47c93, 0x8a02, 0x4ca5, 0x976e, 0xde7b871f11f3));

//notify service 
static struct bt_uuid_128 respiratory_notify_prop = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0xd67b5503, 0xa25c, 0x4395, 0xbd21, 0xf92823d91442));

static uint8_t respiratory_notify_buff[adcResults_size];

//protobuf encode sensor data, vector_n has 10 entries.
static int encode_data(int16_t vector_n[], uint8_t *protobuf_packed, size_t *packed_size) {
	bool status;

	adcResults message = adcResults_init_zero;
	/* Create a stream that will write to our buffer. */
	pb_ostream_t stream = pb_ostream_from_buffer(protobuf_packed, adcResults_size);	
	
	for(int i = 0; i<MESSAGE_NUM; i++) {
		message.resp[i] = vector_n[i];
	}	

	status = pb_encode(&stream, &adcResults_msg, &message);
	*packed_size = stream.bytes_written;

	LOG_INF("Encoded, actual protobuf message size was %d", stream.bytes_written); //PRIu64

	if (!status) {
		LOG_ERR("Encoding failed: %s\n", PB_GET_ERROR(&stream));
	}

	return status;
}



const struct bt_data ad[] = {
		BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
		BT_DATA_BYTES(BT_DATA_UUID16_ALL, ),	};
		//BT_DATA_BYTES(BT_DATA_UUID128_ALL, motiondata_prop),
// 			BT_DATA(BT_DATA_MANUFACTURER_DATA, uniqueId, sizeof(ui))
// 	};
// }

void mtu_updated(struct bt_conn *conn, uint16_t tx, uint16_t rx)
{
	LOG_INF("Updated MTU: TX: %d RX: %d bytes\n", tx, rx);
}

static struct bt_gatt_cb gatt_callbacks = {
	.att_mtu_updated = mtu_updated
};

static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		LOG_INF("Connection failed (err 0x%02x)\n", err);
	} else {
		LOG_INF("Connected\n");
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	LOG_INF("Disconnected (reason 0x%02x)\n", reason);
}

static void le_data_length_updated(struct bt_conn *conn, struct bt_conn_le_data_len_info *info) {
	LOG_INF("LE data len updated: TX (len: %d time: %d)"
	       " RX (len: %d time: %d)\n", info->tx_max_len,
	       info->tx_max_time, info->rx_max_len, info->rx_max_time);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
	.le_data_len_updated = le_data_length_updated,
};

K_WORK_DEFINE(startcollecting, calibrate_and_start);

static void resp_readmotion_data_notify_changed(const struct bt_gatt_attr *attr, uint16_t value) {
	notify_ble_resp_on1 = (value == BT_GATT_CCC_NOTIFY) ? 1 : 0;
	if(notify_ble_resp_on1) {
		k_work_submit(&startcollecting);
	}	
}

BT_GATT_SERVICE_DEFINE(motion_svc,
	BT_GATT_PRIMARY_SERVICE(&respiratory_general_prop),
	
	BT_GATT_CHARACTERISTIC(&respiratory_notify_prop.uuid, 
	 		       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
	 		       BT_GATT_PERM_READ,
	 		       NULL, NULL, respiratory_notify_buff),
	BT_GATT_CCC(resp_readmotion_data_notify_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE)
);



#define BT_CONN_CUSTOM BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONNECTABLE | \
					    BT_LE_ADV_OPT_USE_NAME, \
					    768, 1024, NULL) //*1.25ms


static void bt_ready(void)
{
	int err;

	LOG_INF("Bluetooth initialized\n");	

	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

	err = bt_le_adv_start(BT_CONN_CUSTOM, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		LOG_ERR("Advertising failed to start (err %d)\n", err);
		return;
	}

	LOG_INF("Advertising successfully started\n");
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Pairing cancelled: %s\n", addr);
}

static struct bt_conn_auth_cb auth_cb_display = {
	.passkey_display = NULL,// auth_passkey_display,
	.passkey_entry = NULL,
	.cancel = auth_cancel,
	.passkey_confirm = NULL,	
};

int ble_load()
{		
	int err;

	err = bt_enable(NULL);
	if (err) {
		LOG_INF("Bluetooth init failed (err %d)\n", err);
		return 0;
	}

	bt_ready();

	bt_gatt_cb_register(&gatt_callbacks);
	bt_conn_auth_cb_register(&auth_cb_display);

	//clean up bonds
	bt_unpair(BT_ID_DEFAULT, BT_ADDR_LE_ANY);
	
	return err;
}



void ble_notify_respiratory_proc(int16_t adcitems[]) {		
	struct bt_gatt_attr *notify_attr = bt_gatt_find_by_uuid(motion_svc.attrs, motion_svc.attr_count, &respiratory_notify_prop.uuid);
	    
        if(notify_ble_resp_on1){

			size_t packed_size;        

			encode_data(adcitems, respiratory_notify_buff, &packed_size);
			LOG_INF("notify:");
			bt_gatt_notify(NULL, notify_attr, respiratory_notify_buff, packed_size);			
		}    
}


// K_THREAD_DEFINE(ble_sender, 2048, ble_notify_respiratory_proc,  NULL, NULL, NULL, 7, 0, 0);
SYS_INIT(ble_load, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
