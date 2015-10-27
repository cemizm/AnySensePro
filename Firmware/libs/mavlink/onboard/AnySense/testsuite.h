/** @file
 *	@brief MAVLink comm protocol testsuite generated from AnySense.xml
 *	@see http://qgroundcontrol.org/mavlink/
 */
#ifndef ANYSENSE_TESTSUITE_H
#define ANYSENSE_TESTSUITE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAVLINK_TEST_ALL
#define MAVLINK_TEST_ALL
static void mavlink_test_common(uint8_t, uint8_t, mavlink_message_t *last_msg);
static void mavlink_test_AnySense(uint8_t, uint8_t, mavlink_message_t *last_msg);

static void mavlink_test_all(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_test_common(system_id, component_id, last_msg);
	mavlink_test_AnySense(system_id, component_id, last_msg);
}
#endif

#include "../common/testsuite.h"


static void mavlink_test_configuration_control(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
	mavlink_configuration_control_t packet_in = {
		17235,17339,17
    };
	mavlink_configuration_control_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        	packet1.param1 = packet_in.param1;
        	packet1.param2 = packet_in.param2;
        	packet1.command = packet_in.command;
        
        

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_control_encode(system_id, component_id, &msg, &packet1);
	mavlink_msg_configuration_control_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_control_pack(system_id, component_id, &msg , packet1.command , packet1.param1 , packet1.param2 );
	mavlink_msg_configuration_control_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_control_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.command , packet1.param1 , packet1.param2 );
	mavlink_msg_configuration_control_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
        	comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
	mavlink_msg_configuration_control_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_control_send(MAVLINK_COMM_1 , packet1.command , packet1.param1 , packet1.param2 );
	mavlink_msg_configuration_control_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}

static void mavlink_test_configuration_version(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
	mavlink_configuration_version_t packet_in = {
		17235,139,206
    };
	mavlink_configuration_version_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        	packet1.fw_version = packet_in.fw_version;
        	packet1.port1 = packet_in.port1;
        	packet1.port2 = packet_in.port2;
        
        

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_version_encode(system_id, component_id, &msg, &packet1);
	mavlink_msg_configuration_version_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_version_pack(system_id, component_id, &msg , packet1.fw_version , packet1.port1 , packet1.port2 );
	mavlink_msg_configuration_version_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_version_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.fw_version , packet1.port1 , packet1.port2 );
	mavlink_msg_configuration_version_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
        	comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
	mavlink_msg_configuration_version_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_version_send(MAVLINK_COMM_1 , packet1.fw_version , packet1.port1 , packet1.port2 );
	mavlink_msg_configuration_version_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}

static void mavlink_test_configuration_naza_heartbeat(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
	mavlink_configuration_naza_heartbeat_t packet_in = {
		17235
    };
	mavlink_configuration_naza_heartbeat_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        	packet1.lastNazaHeartbeat = packet_in.lastNazaHeartbeat;
        
        

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_naza_heartbeat_encode(system_id, component_id, &msg, &packet1);
	mavlink_msg_configuration_naza_heartbeat_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_naza_heartbeat_pack(system_id, component_id, &msg , packet1.lastNazaHeartbeat );
	mavlink_msg_configuration_naza_heartbeat_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_naza_heartbeat_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.lastNazaHeartbeat );
	mavlink_msg_configuration_naza_heartbeat_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
        	comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
	mavlink_msg_configuration_naza_heartbeat_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_naza_heartbeat_send(MAVLINK_COMM_1 , packet1.lastNazaHeartbeat );
	mavlink_msg_configuration_naza_heartbeat_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}

static void mavlink_test_configuration_port(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
	mavlink_configuration_port_t packet_in = {
		5,72,{ 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }
    };
	mavlink_configuration_port_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        	packet1.port = packet_in.port;
        	packet1.protocol = packet_in.protocol;
        
        	mav_array_memcpy(packet1.data, packet_in.data, sizeof(uint8_t)*128);
        

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_port_encode(system_id, component_id, &msg, &packet1);
	mavlink_msg_configuration_port_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_port_pack(system_id, component_id, &msg , packet1.port , packet1.protocol , packet1.data );
	mavlink_msg_configuration_port_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_port_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.port , packet1.protocol , packet1.data );
	mavlink_msg_configuration_port_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
        	comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
	mavlink_msg_configuration_port_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_port_send(MAVLINK_COMM_1 , packet1.port , packet1.protocol , packet1.data );
	mavlink_msg_configuration_port_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}

static void mavlink_test_configuration_version2(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
	mavlink_configuration_version2_t packet_in = {
		963497464,17,84
    };
	mavlink_configuration_version2_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        	packet1.fw_version = packet_in.fw_version;
        	packet1.port1 = packet_in.port1;
        	packet1.port2 = packet_in.port2;
        
        

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_version2_encode(system_id, component_id, &msg, &packet1);
	mavlink_msg_configuration_version2_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_version2_pack(system_id, component_id, &msg , packet1.fw_version , packet1.port1 , packet1.port2 );
	mavlink_msg_configuration_version2_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_version2_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.fw_version , packet1.port1 , packet1.port2 );
	mavlink_msg_configuration_version2_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
        	comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
	mavlink_msg_configuration_version2_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_configuration_version2_send(MAVLINK_COMM_1 , packet1.fw_version , packet1.port1 , packet1.port2 );
	mavlink_msg_configuration_version2_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}

static void mavlink_test_AnySense(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_test_configuration_control(system_id, component_id, last_msg);
	mavlink_test_configuration_version(system_id, component_id, last_msg);
	mavlink_test_configuration_naza_heartbeat(system_id, component_id, last_msg);
	mavlink_test_configuration_port(system_id, component_id, last_msg);
	mavlink_test_configuration_version2(system_id, component_id, last_msg);
}

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // ANYSENSE_TESTSUITE_H
