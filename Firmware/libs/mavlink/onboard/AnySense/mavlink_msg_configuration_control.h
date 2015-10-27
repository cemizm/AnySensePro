// MESSAGE CONFIGURATION_CONTROL PACKING

#define MAVLINK_MSG_ID_CONFIGURATION_CONTROL 180

typedef struct __mavlink_configuration_control_t
{
 uint16_t param1; /*< Optional, Parameter 1 for Command.*/
 uint16_t param2; /*< Optional, Parameter 2 for Command.*/
 uint8_t command; /*< Command to execute*/
} mavlink_configuration_control_t;

#define MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN 5
#define MAVLINK_MSG_ID_180_LEN 5

#define MAVLINK_MSG_ID_CONFIGURATION_CONTROL_CRC 155
#define MAVLINK_MSG_ID_180_CRC 155



#define MAVLINK_MESSAGE_INFO_CONFIGURATION_CONTROL { \
	"CONFIGURATION_CONTROL", \
	3, \
	{  { "param1", NULL, MAVLINK_TYPE_UINT16_T, 0, 0, offsetof(mavlink_configuration_control_t, param1) }, \
         { "param2", NULL, MAVLINK_TYPE_UINT16_T, 0, 2, offsetof(mavlink_configuration_control_t, param2) }, \
         { "command", NULL, MAVLINK_TYPE_UINT8_T, 0, 4, offsetof(mavlink_configuration_control_t, command) }, \
         } \
}


/**
 * @brief Pack a configuration_control message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param command Command to execute
 * @param param1 Optional, Parameter 1 for Command.
 * @param param2 Optional, Parameter 2 for Command.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_configuration_control_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint8_t command, uint16_t param1, uint16_t param2)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN];
	_mav_put_uint16_t(buf, 0, param1);
	_mav_put_uint16_t(buf, 2, param2);
	_mav_put_uint8_t(buf, 4, command);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN);
#else
	mavlink_configuration_control_t packet;
	packet.param1 = param1;
	packet.param2 = param2;
	packet.command = command;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_CONFIGURATION_CONTROL;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN);
#endif
}

/**
 * @brief Pack a configuration_control message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param command Command to execute
 * @param param1 Optional, Parameter 1 for Command.
 * @param param2 Optional, Parameter 2 for Command.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_configuration_control_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint8_t command,uint16_t param1,uint16_t param2)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN];
	_mav_put_uint16_t(buf, 0, param1);
	_mav_put_uint16_t(buf, 2, param2);
	_mav_put_uint8_t(buf, 4, command);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN);
#else
	mavlink_configuration_control_t packet;
	packet.param1 = param1;
	packet.param2 = param2;
	packet.command = command;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_CONFIGURATION_CONTROL;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN);
#endif
}

/**
 * @brief Encode a configuration_control struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param configuration_control C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_configuration_control_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_configuration_control_t* configuration_control)
{
	return mavlink_msg_configuration_control_pack(system_id, component_id, msg, configuration_control->command, configuration_control->param1, configuration_control->param2);
}

/**
 * @brief Encode a configuration_control struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param configuration_control C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_configuration_control_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_configuration_control_t* configuration_control)
{
	return mavlink_msg_configuration_control_pack_chan(system_id, component_id, chan, msg, configuration_control->command, configuration_control->param1, configuration_control->param2);
}

/**
 * @brief Send a configuration_control message
 * @param chan MAVLink channel to send the message
 *
 * @param command Command to execute
 * @param param1 Optional, Parameter 1 for Command.
 * @param param2 Optional, Parameter 2 for Command.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_configuration_control_send(mavlink_channel_t chan, uint8_t command, uint16_t param1, uint16_t param2)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN];
	_mav_put_uint16_t(buf, 0, param1);
	_mav_put_uint16_t(buf, 2, param2);
	_mav_put_uint8_t(buf, 4, command);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_CONTROL, buf, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_CONTROL, buf, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN);
#endif
#else
	mavlink_configuration_control_t packet;
	packet.param1 = param1;
	packet.param2 = param2;
	packet.command = command;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_CONTROL, (const char *)&packet, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_CONTROL, (const char *)&packet, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_configuration_control_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t command, uint16_t param1, uint16_t param2)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint16_t(buf, 0, param1);
	_mav_put_uint16_t(buf, 2, param2);
	_mav_put_uint8_t(buf, 4, command);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_CONTROL, buf, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_CONTROL, buf, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN);
#endif
#else
	mavlink_configuration_control_t *packet = (mavlink_configuration_control_t *)msgbuf;
	packet->param1 = param1;
	packet->param2 = param2;
	packet->command = command;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_CONTROL, (const char *)packet, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIGURATION_CONTROL, (const char *)packet, MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE CONFIGURATION_CONTROL UNPACKING


/**
 * @brief Get field command from configuration_control message
 *
 * @return Command to execute
 */
static inline uint8_t mavlink_msg_configuration_control_get_command(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  4);
}

/**
 * @brief Get field param1 from configuration_control message
 *
 * @return Optional, Parameter 1 for Command.
 */
static inline uint16_t mavlink_msg_configuration_control_get_param1(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  0);
}

/**
 * @brief Get field param2 from configuration_control message
 *
 * @return Optional, Parameter 2 for Command.
 */
static inline uint16_t mavlink_msg_configuration_control_get_param2(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  2);
}

/**
 * @brief Decode a configuration_control message into a struct
 *
 * @param msg The message to decode
 * @param configuration_control C-struct to decode the message contents into
 */
static inline void mavlink_msg_configuration_control_decode(const mavlink_message_t* msg, mavlink_configuration_control_t* configuration_control)
{
#if MAVLINK_NEED_BYTE_SWAP
	configuration_control->param1 = mavlink_msg_configuration_control_get_param1(msg);
	configuration_control->param2 = mavlink_msg_configuration_control_get_param2(msg);
	configuration_control->command = mavlink_msg_configuration_control_get_command(msg);
#else
	memcpy(configuration_control, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_CONFIGURATION_CONTROL_LEN);
#endif
}
