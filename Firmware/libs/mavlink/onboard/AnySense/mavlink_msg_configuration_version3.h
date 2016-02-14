// MESSAGE CONFIGURATION_VERSION3 PACKING

#define MAVLINK_MSG_ID_CONFIGURATION_VERSION3 185

typedef struct __mavlink_configuration_version3_t
{
 uint32_t fw_version; /*< Firmware Version*/
 uint8_t hw_version; /*< Hardware Version*/
} mavlink_configuration_version3_t;

#define MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN 5
#define MAVLINK_MSG_ID_185_LEN 5

#define MAVLINK_MSG_ID_CONFIGURATION_VERSION3_CRC 180
#define MAVLINK_MSG_ID_185_CRC 180



#define MAVLINK_MESSAGE_INFO_CONFIGURATION_VERSION3 { \
	"CONFIGURATION_VERSION3", \
	2, \
	{  { "fw_version", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_configuration_version3_t, fw_version) }, \
         { "hw_version", NULL, MAVLINK_TYPE_UINT8_T, 0, 4, offsetof(mavlink_configuration_version3_t, hw_version) }, \
         } \
}


/**
 * @brief Pack a configuration_version3 message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param fw_version Firmware Version
 * @param hw_version Hardware Version
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_configuration_version3_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint32_t fw_version, uint8_t hw_version)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN];
	_mav_put_uint32_t(buf, 0, fw_version);
	_mav_put_uint8_t(buf, 4, hw_version);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN);
#else
	mavlink_configuration_version3_t packet;
	packet.fw_version = fw_version;
	packet.hw_version = hw_version;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_CONFIGURATION_VERSION3;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN);
#endif
}

/**
 * @brief Pack a configuration_version3 message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param fw_version Firmware Version
 * @param hw_version Hardware Version
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_configuration_version3_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint32_t fw_version,uint8_t hw_version)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN];
	_mav_put_uint32_t(buf, 0, fw_version);
	_mav_put_uint8_t(buf, 4, hw_version);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN);
#else
	mavlink_configuration_version3_t packet;
	packet.fw_version = fw_version;
	packet.hw_version = hw_version;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_CONFIGURATION_VERSION3;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN);
#endif
}

/**
 * @brief Encode a configuration_version3 struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param configuration_version3 C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_configuration_version3_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_configuration_version3_t* configuration_version3)
{
	return mavlink_msg_configuration_version3_pack(system_id, component_id, msg, configuration_version3->fw_version, configuration_version3->hw_version);
}

/**
 * @brief Encode a configuration_version3 struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param configuration_version3 C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_configuration_version3_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_configuration_version3_t* configuration_version3)
{
	return mavlink_msg_configuration_version3_pack_chan(system_id, component_id, chan, msg, configuration_version3->fw_version, configuration_version3->hw_version);
}

/**
 * @brief Send a configuration_version3 message
 * @param chan MAVLink channel to send the message
 *
 * @param fw_version Firmware Version
 * @param hw_version Hardware Version
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_configuration_version3_send(mavlink_channel_t chan, uint32_t fw_version, uint8_t hw_version)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN];
	_mav_put_uint32_t(buf, 0, fw_version);
	_mav_put_uint8_t(buf, 4, hw_version);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION3, buf, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION3, buf, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN);
#endif
#else
	mavlink_configuration_version3_t packet;
	packet.fw_version = fw_version;
	packet.hw_version = hw_version;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION3, (const char *)&packet, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION3, (const char *)&packet, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_configuration_version3_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint32_t fw_version, uint8_t hw_version)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint32_t(buf, 0, fw_version);
	_mav_put_uint8_t(buf, 4, hw_version);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION3, buf, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION3, buf, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN);
#endif
#else
	mavlink_configuration_version3_t *packet = (mavlink_configuration_version3_t *)msgbuf;
	packet->fw_version = fw_version;
	packet->hw_version = hw_version;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION3, (const char *)packet, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_VERSION3, (const char *)packet, MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE CONFIGURATION_VERSION3 UNPACKING


/**
 * @brief Get field fw_version from configuration_version3 message
 *
 * @return Firmware Version
 */
static inline uint32_t mavlink_msg_configuration_version3_get_fw_version(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  0);
}

/**
 * @brief Get field hw_version from configuration_version3 message
 *
 * @return Hardware Version
 */
static inline uint8_t mavlink_msg_configuration_version3_get_hw_version(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  4);
}

/**
 * @brief Decode a configuration_version3 message into a struct
 *
 * @param msg The message to decode
 * @param configuration_version3 C-struct to decode the message contents into
 */
static inline void mavlink_msg_configuration_version3_decode(const mavlink_message_t* msg, mavlink_configuration_version3_t* configuration_version3)
{
#if MAVLINK_NEED_BYTE_SWAP
	configuration_version3->fw_version = mavlink_msg_configuration_version3_get_fw_version(msg);
	configuration_version3->hw_version = mavlink_msg_configuration_version3_get_hw_version(msg);
#else
	memcpy(configuration_version3, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_CONFIGURATION_VERSION3_LEN);
#endif
}
