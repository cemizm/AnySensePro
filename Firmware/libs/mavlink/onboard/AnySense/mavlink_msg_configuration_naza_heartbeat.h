// MESSAGE CONFIGURATION_NAZA_HEARTBEAT PACKING

#define MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT 182

typedef struct __mavlink_configuration_naza_heartbeat_t
{
 uint16_t lastNazaHeartbeat; /*< Last Naza Heartbeat in ms. If no Heartbeat UINT16_MAX*/
} mavlink_configuration_naza_heartbeat_t;

#define MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN 2
#define MAVLINK_MSG_ID_182_LEN 2

#define MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_CRC 21
#define MAVLINK_MSG_ID_182_CRC 21



#define MAVLINK_MESSAGE_INFO_CONFIGURATION_NAZA_HEARTBEAT { \
	"CONFIGURATION_NAZA_HEARTBEAT", \
	1, \
	{  { "lastNazaHeartbeat", NULL, MAVLINK_TYPE_UINT16_T, 0, 0, offsetof(mavlink_configuration_naza_heartbeat_t, lastNazaHeartbeat) }, \
         } \
}


/**
 * @brief Pack a configuration_naza_heartbeat message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param lastNazaHeartbeat Last Naza Heartbeat in ms. If no Heartbeat UINT16_MAX
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_configuration_naza_heartbeat_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint16_t lastNazaHeartbeat)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN];
	_mav_put_uint16_t(buf, 0, lastNazaHeartbeat);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN);
#else
	mavlink_configuration_naza_heartbeat_t packet;
	packet.lastNazaHeartbeat = lastNazaHeartbeat;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN);
#endif
}

/**
 * @brief Pack a configuration_naza_heartbeat message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param lastNazaHeartbeat Last Naza Heartbeat in ms. If no Heartbeat UINT16_MAX
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_configuration_naza_heartbeat_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint16_t lastNazaHeartbeat)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN];
	_mav_put_uint16_t(buf, 0, lastNazaHeartbeat);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN);
#else
	mavlink_configuration_naza_heartbeat_t packet;
	packet.lastNazaHeartbeat = lastNazaHeartbeat;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN);
#endif
}

/**
 * @brief Encode a configuration_naza_heartbeat struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param configuration_naza_heartbeat C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_configuration_naza_heartbeat_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_configuration_naza_heartbeat_t* configuration_naza_heartbeat)
{
	return mavlink_msg_configuration_naza_heartbeat_pack(system_id, component_id, msg, configuration_naza_heartbeat->lastNazaHeartbeat);
}

/**
 * @brief Encode a configuration_naza_heartbeat struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param configuration_naza_heartbeat C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_configuration_naza_heartbeat_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_configuration_naza_heartbeat_t* configuration_naza_heartbeat)
{
	return mavlink_msg_configuration_naza_heartbeat_pack_chan(system_id, component_id, chan, msg, configuration_naza_heartbeat->lastNazaHeartbeat);
}

/**
 * @brief Send a configuration_naza_heartbeat message
 * @param chan MAVLink channel to send the message
 *
 * @param lastNazaHeartbeat Last Naza Heartbeat in ms. If no Heartbeat UINT16_MAX
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_configuration_naza_heartbeat_send(mavlink_channel_t chan, uint16_t lastNazaHeartbeat)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN];
	_mav_put_uint16_t(buf, 0, lastNazaHeartbeat);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT, buf, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT, buf, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN);
#endif
#else
	mavlink_configuration_naza_heartbeat_t packet;
	packet.lastNazaHeartbeat = lastNazaHeartbeat;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT, (const char *)&packet, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT, (const char *)&packet, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_configuration_naza_heartbeat_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint16_t lastNazaHeartbeat)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint16_t(buf, 0, lastNazaHeartbeat);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT, buf, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT, buf, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN);
#endif
#else
	mavlink_configuration_naza_heartbeat_t *packet = (mavlink_configuration_naza_heartbeat_t *)msgbuf;
	packet->lastNazaHeartbeat = lastNazaHeartbeat;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT, (const char *)packet, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT, (const char *)packet, MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE CONFIGURATION_NAZA_HEARTBEAT UNPACKING


/**
 * @brief Get field lastNazaHeartbeat from configuration_naza_heartbeat message
 *
 * @return Last Naza Heartbeat in ms. If no Heartbeat UINT16_MAX
 */
static inline uint16_t mavlink_msg_configuration_naza_heartbeat_get_lastNazaHeartbeat(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  0);
}

/**
 * @brief Decode a configuration_naza_heartbeat message into a struct
 *
 * @param msg The message to decode
 * @param configuration_naza_heartbeat C-struct to decode the message contents into
 */
static inline void mavlink_msg_configuration_naza_heartbeat_decode(const mavlink_message_t* msg, mavlink_configuration_naza_heartbeat_t* configuration_naza_heartbeat)
{
#if MAVLINK_NEED_BYTE_SWAP
	configuration_naza_heartbeat->lastNazaHeartbeat = mavlink_msg_configuration_naza_heartbeat_get_lastNazaHeartbeat(msg);
#else
	memcpy(configuration_naza_heartbeat, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_CONFIGURATION_NAZA_HEARTBEAT_LEN);
#endif
}
