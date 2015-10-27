// MESSAGE CONFIGURATION_PORT PACKING

#define MAVLINK_MSG_ID_CONFIGURATION_PORT 183

typedef struct __mavlink_configuration_port_t
{
 uint8_t port; /*< Port number*/
 uint8_t protocol; /*< Protocol used on this Port*/
 uint8_t data[128]; /*< Serialized Protocol configuration*/
} mavlink_configuration_port_t;

#define MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN 130
#define MAVLINK_MSG_ID_183_LEN 130

#define MAVLINK_MSG_ID_CONFIGURATION_PORT_CRC 186
#define MAVLINK_MSG_ID_183_CRC 186

#define MAVLINK_MSG_CONFIGURATION_PORT_FIELD_DATA_LEN 128

#define MAVLINK_MESSAGE_INFO_CONFIGURATION_PORT { \
	"CONFIGURATION_PORT", \
	3, \
	{  { "port", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_configuration_port_t, port) }, \
         { "protocol", NULL, MAVLINK_TYPE_UINT8_T, 0, 1, offsetof(mavlink_configuration_port_t, protocol) }, \
         { "data", NULL, MAVLINK_TYPE_UINT8_T, 128, 2, offsetof(mavlink_configuration_port_t, data) }, \
         } \
}


/**
 * @brief Pack a configuration_port message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param port Port number
 * @param protocol Protocol used on this Port
 * @param data Serialized Protocol configuration
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_configuration_port_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint8_t port, uint8_t protocol, const uint8_t *data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN];
	_mav_put_uint8_t(buf, 0, port);
	_mav_put_uint8_t(buf, 1, protocol);
	_mav_put_uint8_t_array(buf, 2, data, 128);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN);
#else
	mavlink_configuration_port_t packet;
	packet.port = port;
	packet.protocol = protocol;
	mav_array_memcpy(packet.data, data, sizeof(uint8_t)*128);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_CONFIGURATION_PORT;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN, MAVLINK_MSG_ID_CONFIGURATION_PORT_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN);
#endif
}

/**
 * @brief Pack a configuration_port message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param port Port number
 * @param protocol Protocol used on this Port
 * @param data Serialized Protocol configuration
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_configuration_port_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint8_t port,uint8_t protocol,const uint8_t *data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN];
	_mav_put_uint8_t(buf, 0, port);
	_mav_put_uint8_t(buf, 1, protocol);
	_mav_put_uint8_t_array(buf, 2, data, 128);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN);
#else
	mavlink_configuration_port_t packet;
	packet.port = port;
	packet.protocol = protocol;
	mav_array_memcpy(packet.data, data, sizeof(uint8_t)*128);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_CONFIGURATION_PORT;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN, MAVLINK_MSG_ID_CONFIGURATION_PORT_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN);
#endif
}

/**
 * @brief Encode a configuration_port struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param configuration_port C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_configuration_port_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_configuration_port_t* configuration_port)
{
	return mavlink_msg_configuration_port_pack(system_id, component_id, msg, configuration_port->port, configuration_port->protocol, configuration_port->data);
}

/**
 * @brief Encode a configuration_port struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param configuration_port C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_configuration_port_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_configuration_port_t* configuration_port)
{
	return mavlink_msg_configuration_port_pack_chan(system_id, component_id, chan, msg, configuration_port->port, configuration_port->protocol, configuration_port->data);
}

/**
 * @brief Send a configuration_port message
 * @param chan MAVLink channel to send the message
 *
 * @param port Port number
 * @param protocol Protocol used on this Port
 * @param data Serialized Protocol configuration
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_configuration_port_send(mavlink_channel_t chan, uint8_t port, uint8_t protocol, const uint8_t *data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN];
	_mav_put_uint8_t(buf, 0, port);
	_mav_put_uint8_t(buf, 1, protocol);
	_mav_put_uint8_t_array(buf, 2, data, 128);
#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_PORT, buf, MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN, MAVLINK_MSG_ID_CONFIGURATION_PORT_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_PORT, buf, MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN);
#endif
#else
	mavlink_configuration_port_t packet;
	packet.port = port;
	packet.protocol = protocol;
	mav_array_memcpy(packet.data, data, sizeof(uint8_t)*128);
#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_PORT, (const char *)&packet, MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN, MAVLINK_MSG_ID_CONFIGURATION_PORT_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_PORT, (const char *)&packet, MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_configuration_port_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t port, uint8_t protocol, const uint8_t *data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint8_t(buf, 0, port);
	_mav_put_uint8_t(buf, 1, protocol);
	_mav_put_uint8_t_array(buf, 2, data, 128);
#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_PORT, buf, MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN, MAVLINK_MSG_ID_CONFIGURATION_PORT_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_PORT, buf, MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN);
#endif
#else
	mavlink_configuration_port_t *packet = (mavlink_configuration_port_t *)msgbuf;
	packet->port = port;
	packet->protocol = protocol;
	mav_array_memcpy(packet->data, data, sizeof(uint8_t)*128);
#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_PORT, (const char *)packet, MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN, MAVLINK_MSG_ID_CONFIGURATION_PORT_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_PORT, (const char *)packet, MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE CONFIGURATION_PORT UNPACKING


/**
 * @brief Get field port from configuration_port message
 *
 * @return Port number
 */
static inline uint8_t mavlink_msg_configuration_port_get_port(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  0);
}

/**
 * @brief Get field protocol from configuration_port message
 *
 * @return Protocol used on this Port
 */
static inline uint8_t mavlink_msg_configuration_port_get_protocol(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  1);
}

/**
 * @brief Get field data from configuration_port message
 *
 * @return Serialized Protocol configuration
 */
static inline uint16_t mavlink_msg_configuration_port_get_data(const mavlink_message_t* msg, uint8_t *data)
{
	return _MAV_RETURN_uint8_t_array(msg, data, 128,  2);
}

/**
 * @brief Decode a configuration_port message into a struct
 *
 * @param msg The message to decode
 * @param configuration_port C-struct to decode the message contents into
 */
static inline void mavlink_msg_configuration_port_decode(const mavlink_message_t* msg, mavlink_configuration_port_t* configuration_port)
{
#if MAVLINK_NEED_BYTE_SWAP
	configuration_port->port = mavlink_msg_configuration_port_get_port(msg);
	configuration_port->protocol = mavlink_msg_configuration_port_get_protocol(msg);
	mavlink_msg_configuration_port_get_data(msg, configuration_port->data);
#else
	memcpy(configuration_port, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_CONFIGURATION_PORT_LEN);
#endif
}
