// MESSAGE CONFIGURATION_DATA PACKING

#define MAVLINK_MSG_ID_CONFIGURATION_DATA 186

typedef struct __mavlink_configuration_data_t
{
 uint8_t data[240]; /*< Serialized Protocol configuration*/
} mavlink_configuration_data_t;

#define MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN 240
#define MAVLINK_MSG_ID_186_LEN 240

#define MAVLINK_MSG_ID_CONFIGURATION_DATA_CRC 108
#define MAVLINK_MSG_ID_186_CRC 108

#define MAVLINK_MSG_CONFIGURATION_DATA_FIELD_DATA_LEN 240

#define MAVLINK_MESSAGE_INFO_CONFIGURATION_DATA { \
	"CONFIGURATION_DATA", \
	1, \
	{  { "data", NULL, MAVLINK_TYPE_UINT8_T, 240, 0, offsetof(mavlink_configuration_data_t, data) }, \
         } \
}


/**
 * @brief Pack a configuration_data message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param data Serialized Protocol configuration
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_configuration_data_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       const uint8_t *data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN];

	_mav_put_uint8_t_array(buf, 0, data, 240);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN);
#else
	mavlink_configuration_data_t packet;

	mav_array_memcpy(packet.data, data, sizeof(uint8_t)*240);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_CONFIGURATION_DATA;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN, MAVLINK_MSG_ID_CONFIGURATION_DATA_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN);
#endif
}

/**
 * @brief Pack a configuration_data message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param data Serialized Protocol configuration
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_configuration_data_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           const uint8_t *data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN];

	_mav_put_uint8_t_array(buf, 0, data, 240);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN);
#else
	mavlink_configuration_data_t packet;

	mav_array_memcpy(packet.data, data, sizeof(uint8_t)*240);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_CONFIGURATION_DATA;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN, MAVLINK_MSG_ID_CONFIGURATION_DATA_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN);
#endif
}

/**
 * @brief Encode a configuration_data struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param configuration_data C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_configuration_data_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_configuration_data_t* configuration_data)
{
	return mavlink_msg_configuration_data_pack(system_id, component_id, msg, configuration_data->data);
}

/**
 * @brief Encode a configuration_data struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param configuration_data C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_configuration_data_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_configuration_data_t* configuration_data)
{
	return mavlink_msg_configuration_data_pack_chan(system_id, component_id, chan, msg, configuration_data->data);
}

/**
 * @brief Send a configuration_data message
 * @param chan MAVLink channel to send the message
 *
 * @param data Serialized Protocol configuration
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_configuration_data_send(mavlink_channel_t chan, const uint8_t *data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN];

	_mav_put_uint8_t_array(buf, 0, data, 240);
#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_DATA, buf, MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN, MAVLINK_MSG_ID_CONFIGURATION_DATA_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_DATA, buf, MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN);
#endif
#else
	mavlink_configuration_data_t packet;

	mav_array_memcpy(packet.data, data, sizeof(uint8_t)*240);
#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_DATA, (const char *)&packet, MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN, MAVLINK_MSG_ID_CONFIGURATION_DATA_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_DATA, (const char *)&packet, MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_configuration_data_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  const uint8_t *data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;

	_mav_put_uint8_t_array(buf, 0, data, 240);
#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_DATA, buf, MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN, MAVLINK_MSG_ID_CONFIGURATION_DATA_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_DATA, buf, MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN);
#endif
#else
	mavlink_configuration_data_t *packet = (mavlink_configuration_data_t *)msgbuf;

	mav_array_memcpy(packet->data, data, sizeof(uint8_t)*240);
#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_DATA, (const char *)packet, MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN, MAVLINK_MSG_ID_CONFIGURATION_DATA_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_DATA, (const char *)packet, MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE CONFIGURATION_DATA UNPACKING


/**
 * @brief Get field data from configuration_data message
 *
 * @return Serialized Protocol configuration
 */
static inline uint16_t mavlink_msg_configuration_data_get_data(const mavlink_message_t* msg, uint8_t *data)
{
	return _MAV_RETURN_uint8_t_array(msg, data, 240,  0);
}

/**
 * @brief Decode a configuration_data message into a struct
 *
 * @param msg The message to decode
 * @param configuration_data C-struct to decode the message contents into
 */
static inline void mavlink_msg_configuration_data_decode(const mavlink_message_t* msg, mavlink_configuration_data_t* configuration_data)
{
#if MAVLINK_NEED_BYTE_SWAP
	mavlink_msg_configuration_data_get_data(msg, configuration_data->data);
#else
	memcpy(configuration_data, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_CONFIGURATION_DATA_LEN);
#endif
}
