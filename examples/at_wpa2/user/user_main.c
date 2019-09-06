/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "osapi.h"
#include "at_custom.h"
#include "user_interface.h"

#include "wpa2_enterprise.h"

#include "wpa2_pki/ca.h"
#include "wpa2_pki/client_crt.h"
#include "wpa2_pki/client_key.h"

#define WPA2_MAX_IDENTITY_SIZE 32
#define WPA2_MAX_USERNAME_SIZE 32
#define WPA2_MAX_PASSWORD_SIZE 32

typedef enum {
    EAP_TLS,
    EAP_PEAP,
    EAP_TTLS,
} eap_method_t;

// test :AT+SETPEAP="identity","username","password"
void ICACHE_FLASH_ATTR
at_setupCmdSetPEAP(uint8_t id, char *pPara)
{
    char identity[WPA2_MAX_IDENTITY_SIZE+1];
    char username[WPA2_MAX_USERNAME_SIZE+1];
    char password[WPA2_MAX_IDENTITY_SIZE+1];

    pPara++; // skip '='

    // get the first parameter
    memset(identity, 0, sizeof(identity));
    at_data_str_copy(identity, &pPara, sizeof(identity)-1);
    if (strlen(identity) == 0 || *pPara++ != ',') {
        at_response_error();
        return;
    }
    // at_port_print("identity:");
    // at_port_print(identity);
    // at_port_print("\r\n");

    // get the second parameter
    memset(username, 0, sizeof(username));
    at_data_str_copy(username, &pPara, sizeof(username)-1);
    if (strlen(username) == 0 || *pPara++ != ',') {
        at_response_error();
        return;
    }
    // at_port_print("username:");
    // at_port_print(username);
    // at_port_print("\r\n");

    // get the third parameter
    memset(password, 0, sizeof(password));
    at_data_str_copy(password, &pPara, sizeof(password)-1);
    if (strlen(password) == 0 || *pPara++ != '\r') {
        at_response_error();
        return;
    }
    // at_port_print("password:");
    // at_port_print(password);
    // at_port_print("\r\n");


    at_response_ok();

    eap_method_t method = EAP_PEAP;

    wifi_station_set_wpa2_enterprise_auth(1);

    wifi_station_set_enterprise_identity(identity, os_strlen(identity));//This is an option. If not call this API, the outer identity will be "anonymous@espressif.com".

    if (method == EAP_TLS) {
        wifi_station_set_enterprise_cert_key(client_cert, os_strlen(client_cert)+1, client_key, os_strlen(client_key)+1, NULL, 1);
        // wifi_station_set_enterprise_username(username, os_strlen(username));//This is an option for EAP_PEAP and EAP_TLS.
    }
    else if (method == EAP_PEAP || method == EAP_TTLS) {
        wifi_station_set_enterprise_username(username, os_strlen(username));
        wifi_station_set_enterprise_password(password, os_strlen(password));
        // wifi_station_set_enterprise_ca_cert(ca, os_strlen(ca)+1);//This is an option for EAP_PEAP and EAP_TTLS.
    }

    // wifi_station_connect();
}

void ICACHE_FLASH_ATTR
at_SetPEAPCmdSetPEAP(uint8_t id)
{
    uint8 buffer[32] = {0};

    os_sprintf(buffer, "%s\r\n", "at_SetPEAPCmdSetPEAP");
    at_port_print(buffer);
    at_response_ok();
}

void ICACHE_FLASH_ATTR
at_queryCmdSetPEAP(uint8_t id)
{
    uint8 buffer[32] = {0};

    os_sprintf(buffer, "%s\r\n", "at_queryCmdSetPEAP");
    at_port_print(buffer);
    at_response_ok();
}

void ICACHE_FLASH_ATTR
at_exeCmdSetPEAP(uint8_t id)
{
    uint8 buffer[32] = {0};

    os_sprintf(buffer, "%s\r\n", "at_exeCmdSetPEAP");
    at_port_print(buffer);
    at_response_ok();
}

extern void at_exeCmdCiupdate(uint8_t id);
at_funcationType at_custom_cmd[] = {
    {"+SETPEAP", 8, at_SetPEAPCmdSetPEAP, at_queryCmdSetPEAP, at_setupCmdSetPEAP, at_exeCmdSetPEAP},
#ifdef AT_UPGRADE_SUPPORT
    {"+CIUPDATE", 9,       NULL,            NULL,            NULL, at_exeCmdCiupdate}
#endif
};

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABBBCDDD
 *                A : rf cal
 *                B : at parameters
 *                C : rf init data
 *                D : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void ICACHE_FLASH_ATTR
user_init(void)
{
    // char buf[128] = {0};
    at_customLinkMax = 5;
    at_init();
// #ifdef ESP_AT_FW_VERSION
//     if ((ESP_AT_FW_VERSION != NULL) && (os_strlen(ESP_AT_FW_VERSION) < 64)) {
//         os_sprintf(buf,"compile time:%s %s\r\n"ESP_AT_FW_VERSION,__DATE__,__TIME__);
//     } else {
//         os_sprintf(buf,"compile time:%s %s",__DATE__,__TIME__);
//     }
// #else
//     os_sprintf(buf,"compile time:%s %s",__DATE__,__TIME__);
// #endif
    // at_set_custom_info(buf);
    at_port_print("\r\nready\r\n");
    at_cmd_array_regist(&at_custom_cmd[0], sizeof(at_custom_cmd)/sizeof(at_custom_cmd[0]));

    wifi_set_opmode(STATION_MODE);
    // wifi_station_connect();
}
