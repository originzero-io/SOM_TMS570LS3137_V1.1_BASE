/*
 * communication_config.h
 *
 *  Created on: Dec 27, 2022
 *      Author: fatih
 */

#ifndef COMMUNICATION_CONFIG_H_
#define COMMUNICATION_CONFIG_H_



#define COMM_PACKET_MAX_CONTENT_SIZE 1024
#define COMM_TRANSMIT_PACKET_QUIUE_COUNT 2
#define COMM_RECEIVE_PACKET_QUIUE_COUNT 2


#define COMM_CHANNEL_COUNT 1
/**
 * \brief           Enumeration containing the channel names to be communicated.
 */
typedef enum comm_channels_enum
{
	COMM_CHANNEL_MQTT
} comm_channels_enum_t;

#endif /* COMMUNICATION_CONFIG_H_ */



