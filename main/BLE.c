
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "./BLE/ble_init.h"
#include "./BLE/gatt_server_profile.h"

void app_main() {
    ble_init();

    /*
	for (uint8_t i=0; i<5;i++)
		prof_shared_buf[i] = temp_tab[i];
    */

	esp_ble_gatts_register_callback(gatts_event_handler);

	esp_ble_gap_register_callback(gap_event_handler);

	esp_ble_gatts_app_register(PROFILE_APP_ID);
}