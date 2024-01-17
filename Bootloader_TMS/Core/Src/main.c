/*
 * main.c
 *
 *  Created on: Jul 20, 2022
 *      Author: fatih
 */

#include <communication.h>
#include "main.h"
#include "ethernet.h"
#include "mqtt.h"
#include "mqtt_subscribe.h"
#include "mqtt_publisher.h"
#include "firmware_update.h"
#include "results.h"

#include "sys_vim.h"
#include "esm.h"
#include "rti.h"

#include "Types.h"

static results_enum_t comm_send_data(comm_channels_enum_t channel, uint8_t *data, uint16_t data_len, uint32_t timeout, void *param);
static results_enum_t comm_error_callback(comm_channels_enum_t channel, comm_packet_struct_t *packet, comm_packet_direction_enum_t packet_direction, comm_errors_enum_t error_code, void *param);
static results_enum_t comm_rx_callback(comm_channels_enum_t channel, comm_packet_struct_t *transmit_packet, comm_packet_struct_t *receive_packet, void *param);
static results_enum_t mqtt_init(void);
static results_enum_t mqtt_subscribe_firmware_update_it(mqtt_channels_t channel, subscribe_channels_enum_t subscribe_channel, const uint8_t *const topic, const uint16_t topic_len, const uint8_t *const payload, const mqtt_size_t payload_len);
static void interrupts_start(void);
static void interrupts_stop(void);
static results_enum_t flash_progress_callback(uint8_t state);
static results_enum_t bootloader_update_end_callback(void);
static void ethernet_status_changed_callback(ethernet_channels_enum_t channel, ethernet_status_enum_t status);

volatile static const char *const mqtt_bootloader_request_topic = "server/request/22110102000000001/firmware_update";
volatile static const char *const mqtt_bootloader_response_topic = "server/response/22110102000000001/firmware_update";

extern unsigned int fapi_load_start;
extern unsigned int fapi_run_start;
extern unsigned int fapi_load_size;

extern unsigned int fapi_const_load_start;
extern unsigned int fapi_const_run_start;
extern unsigned int fapi_const_load_size;

volatile int lwip_link_status_ = 0;
volatile int lwip_netif_status_ = 0;
volatile uint8_t send = 0;

extern uint8_t emacAddress[6U];

tcp_struct_t tcp;

void user_main(uint8_t value)
{
	void *p1 = memcpy(&fapi_run_start, &fapi_load_start, (uint32_t) &fapi_load_size);
	void *p2 = memcpy(&fapi_const_run_start, &fapi_const_load_start, (uint32_t) &fapi_const_load_size);



	vimInit();
	esmInit();
	rtiInit();

	interrupts_start();

	//comm_init(COMM_CHANNEL_UART, (uint8_t*) COMM_CHANNEL_UART, COMM_PACKET_TYPE_WITH_VERIFICATION, comm_send_data, comm_rx_callback);
	comm_init(COMM_CHANNEL_MQTT, (uint8_t*) COMM_CHANNEL_MQTT, COMM_PACKET_TYPE_WITHOUT_VERIFICATION, comm_send_data, comm_rx_callback);
	comm_set_error_callback_func(comm_error_callback);

	/*ETHERNET INIT BEGIN*/

	ethernet_set_ip_mode(ETHERNET_CHANNEL_1, ETHERNET_IP_MODE_DHCP);
	ethernet_set_mac_address(ETHERNET_CHANNEL_1, &emacAddress[0]);
	ethernet_set_status_changed_fn(ethernet_status_changed_callback);
	ethernet_init(ETHERNET_CHANNEL_1);
	/*ETHERNET INIT END*/

	/*MQTT INIT BEGIN*/
	mqtt_init();
	/*MQTT INIT END*/

	sciInit();

	firmware_update_init(COMM_CHANNEL_MQTT, flash_progress_callback, bootloader_update_end_callback);
	bootloader_start_update();

	while (1)
	{
		mqtt_loop();
		comm_loop();
		ethernet_loop();
		firmware_update_loop();
		lwip_link_status_ = lwip_link_status_get(0);
		lwip_netif_status_ = lwip_netif_status_get(0);

		/*
		 if (1 == send)
		 {
		 mqtt_publisher_publish(MQTT_CHANNEL_1, mqtt_bootloader_request_topic, strlen(mqtt_bootloader_request_topic), "Deneme", 6, false, MQTT_QOS0, 500);

		 send = 0;
		 }
		 */
		/*
		 uint32_t received_packet_size;
		 results_enum_t result = bootloader_packet_request_it(200, 100, &request_packet[0], &received_packet_size);
		 if (RESULT_IN_THE_PROCESS != result)
		 {
		 result_loop();
		 }
		 */
	}
}

void rtiNotification(uint32 notification)
{
	if (rtiNOTIFICATION_COMPARE0 == notification)
	{
		comm_timer_it();
		mqtt_timer_it();
		tcp_timer_it();
		firmware_update_timer_callback();
		ethernet_timer_it();
	}
	else if (rtiNOTIFICATION_COMPARE1 == notification)
	{
		mqtt_keep_alive_it();
		lwip_timer_callback();
	}
}

static results_enum_t comm_send_data(comm_channels_enum_t channel, uint8_t *data, uint16_t data_len, uint32_t timeout, void *param)
{
	results_enum_t return_value = RESULT_SUCCESS;
	if (COMM_CHANNEL_MQTT == channel)
	{
		return_value = mqtt_publisher_publish(MQTT_CHANNEL_1, mqtt_bootloader_request_topic, (uint16_t) strlen((const char*) &mqtt_bootloader_request_topic[0]), (const uint8_t* const ) data, data_len, false, MQTT_QOS1, 1000);
	}

	return return_value;
}

static results_enum_t comm_error_callback(comm_channels_enum_t channel, comm_packet_struct_t *packet, comm_packet_direction_enum_t packet_direction, comm_errors_enum_t error_code, void *param)
{
	return RESULT_SUCCESS;
}

static results_enum_t comm_rx_callback(comm_channels_enum_t channel, comm_packet_struct_t *transmit_packet, comm_packet_struct_t *receive_packet, void *param)
{
	results_enum_t return_value = RESULT_SUCCESS;

	return return_value;
}

static results_enum_t mqtt_init(void)
{
	mqtt_channels_t mqtt_channel = MQTT_CHANNEL_1;

	const char *const mqtt_client_id = "22110102000000001";
	const char *const mqtt_protocol_name = "MQTT";
	const char *const mqtt_username = /*"Fatih";//*/"test_user";
	const char *const mqtt_password = /*"YAZMAN";//*/"1234";

	//const uint8_t mqtt_ip_address_array[4] = { 138, 68, 104, 15 };
	const uint8_t mqtt_ip_address_array[4] = { 192, 168, 31, 140 };
//const uint8_t mqtt_ip_address_array[4] = { 37,148,211,103 };

	mqtt_set_username(mqtt_channel, mqtt_username, strlen((const char*) &mqtt_username[0]));
	mqtt_set_password(mqtt_channel, mqtt_password, strlen((const char*) &mqtt_password[0]));
	mqtt_set_client_id(mqtt_channel, mqtt_client_id, strlen((const char*) &mqtt_client_id[0]));
	mqtt_set_protocol_name(mqtt_channel, mqtt_protocol_name, strlen((const char*) &mqtt_protocol_name[0]));
	mqtt_set_broker_ip(mqtt_channel, &mqtt_ip_address_array[0], strlen((const char*) &mqtt_protocol_name[0]));
	mqtt_set_port(mqtt_channel, 5200);
	mqtt_set_connection_timeout(mqtt_channel, 5000);
	mqtt_set_is_auto_reconnect(mqtt_channel, true);
	mqtt_set_keep_alive(mqtt_channel, 10);
	mqtt_set_clean_session(mqtt_channel, true);

	/*
	 mqtt_set_will_flag(mqtt_channel, false);
	 mqtt_set_will_qos(mqtt_channel, MQTT_QOS2);
	 mqtt_set_will_retian(mqtt_channel, false);
	 mqtt_set_will_topic(mqtt_channel, mqtt_bootloader_request_topic, sizeof(mqtt_bootloader_request_topic));
	 mqtt_set_will_payload(mqtt_channel, mqtt_will_payload, sizeof(mqtt_will_payload));
	 */
	results_enum_t result;

	do
	{
		result = mqtt_connect(mqtt_channel, true);
	} while (RESULT_IS_ERROR(result));

	if (RESULT_IS_SUCCESS(result))
	{
		mqtt_subscribe_set_mqtt_subscribe_message_it(MQTT_SUBSCRIBE_CHANNEL1, mqtt_subscribe_firmware_update_it);
		do
		{
			result = mqtt_subscribe(mqtt_channel, MQTT_SUBSCRIBE_CHANNEL1, mqtt_bootloader_response_topic, (uint16_t) strlen((const char*) mqtt_bootloader_response_topic), MQTT_QOS1);
		} while (RESULT_IS_ERROR(result));
	}

	return result;
}

static results_enum_t mqtt_subscribe_firmware_update_it(mqtt_channels_t channel, subscribe_channels_enum_t subscribe_channel, const uint8_t *const topic, const uint16_t topic_len, const uint8_t *const payload, const mqtt_size_t payload_len)
{
	comm_raw_data_receive_it(COMM_CHANNEL_MQTT, (uint8_t*) &payload[0], (uint16_t) payload_len);

	return RESULT_SUCCESS;
}

static void interrupts_start(void)
{
	_enable_IRQ();
	_enable_interrupt_();
	esmEnableInterrupt(0xffffffff);

	rtiStartCounter(rtiCOUNTER_BLOCK0);
	rtiEnableNotification(rtiNOTIFICATION_COMPARE0);
	rtiEnableNotification(rtiNOTIFICATION_COMPARE1);
}

static void interrupts_stop(void)
{
	rtiDisableNotification(rtiNOTIFICATION_COMPARE1);
	rtiDisableNotification(rtiNOTIFICATION_COMPARE0);
	rtiStopCounter(rtiCOUNTER_BLOCK0);

	esmDisableInterrupt(0xffffffff);
	_disable_interrupt_();
	_disable_IRQ();
}

/**
 * \brief function pointer from bootloader.h
 */
static results_enum_t flash_progress_callback(uint8_t state)
{
	results_enum_t return_value = RESULT_SUCCESS;

	if (0 == state)
	{
		interrupts_start();
	}
	else
	{
		interrupts_stop();
	}

	return return_value;
}

static results_enum_t bootloader_update_end_callback(void)
{
	mqtt_disconnect(MQTT_CHANNEL_1);

	return RESULT_SUCCESS;
}

static void ethernet_status_changed_callback(ethernet_channels_enum_t channel, ethernet_status_enum_t status)
{
	for (uint8_t i = 0; i < MQTT_CHANNEL_COUNT; i++)
	{
		mqtt_disconnected((mqtt_channels_t) channel);
	}
}
