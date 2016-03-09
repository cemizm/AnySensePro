#ifndef APPLICATION_INCLUDE_TELEMETRYHOTT_H_
#define APPLICATION_INCLUDE_TELEMETRYHOTT_H_

#include <USART.h>

#include "Interrupt.h"
#include "TelemetryAdapter.h"
#include "SensorStore.h"

namespace App
{

class TelemetryHoTT: public TelemetryAdapterImpl<TelemetryProtocol::HoTT>, public HAL::InterruptHandler
{
private:
	static const uint8_t EAMCells = 14;
	static const uint8_t GAMCells = 6;
	struct ProtocolConfig
	{
		uint16_t IsValid;

		uint8_t EAMEnable;
		uint8_t GAMEnable;
		uint8_t GPSEnable;
		uint8_t VarioEnable;
	};

	enum RequestType
	{
		Binary = 0x80, Text = 0x7F,
	};

	enum SensorType
	{
		SensorGAM = 0x8D,  	// General Air Module ID
		SensorEAM = 0x8E,		// Electric Air Module ID
		SensorGPS = 0x8A,		// GPS Module ID
		SensorVario = 0x89,	// Vario Sensor Module ID
	};

	enum SensorResponseType
	{
		ResponseGAM = ((uint8_t) (SensorType::SensorGAM << 4)),
		ResponseEAM = ((uint8_t) (SensorType::SensorEAM << 4)),
		ResponseGPS = ((uint8_t) (SensorType::SensorGPS << 4)),
		ResponseVario = ((uint8_t) (SensorType::SensorVario << 4))
	};

	enum AlarmToneType
	{
		AlarmToneNone = 0x00,			//No alarm
		AlarmToneUnk01 = 0x01,
		AlarmToneDiffNeg1 = 0x02,		//Negative Difference
		AlarmToneDiffNeg2 = 0x03,		//Negative Difference
		AlarmToneUnk04 = 0x04,
		AlarmToneUnk05 = 0x05,
		AlarmToneTempMin1 = 0x06,		//Min. Sensor 1 temp.
		AlarmToneTempMin2 = 0x07,		//Min. Sensor 2 temp.
		AlarmToneTempMax1 = 0x08,		//Max. Sensor 1 temp.
		AlarmToneTempMax2 = 0x09,		//Max. Sensor 2 temp.
		AlarmToneVoltMax1 = 0x0A,		//Max. Sensor 1 voltage
		AlarmToneVoltMax2 = 0x0B,		//Max. Sensor 2 voltage
		AlarmToneUnk0C = 0x0C,
		AlarmToneDiffPos1 = 0x0D,		//Positive Difference 2 B
		AlarmToneDiffPos2 = 0x0E,		//Positive Difference 1 C
		AlarmToneAltMin = 0x0F,			//Min. Altitude O
		AlarmToneVoltMin = 0x10,		//Min. Power Voltage P
		AlarmToneCellMin = 0x11,		//Min. Cell voltage Q
		AlarmToneVoltMin1 = 0x12,		//Min. Sens. 1 voltage R
		AlarmToneVoltMin2 = 0x13,		//Min. Sens. 2 voltage R
		AlarmToneRpmMin = 0x14,			//Min. RPM
		AlarmToneUnk15 = 0x15,
		AlarmToneCapMax = 0x16,			//Max. used capacity
		AlarmToneCurMax = 0x17,			//Max. Current
		AlarmTonePwrMax = 0x18,			//Max. Power Voltage X
		AlarmToneRpmMax = 0x19,			//Maximum RPM Y
		AlarmToneAltMax = 0x1A  		//Max. Altitude Z
	};

	enum AlarmDisplayType
	{
		AlarmDisplayNone = 0x0000, //No Alarm
		AlarmDisplayCells = 0x0001,	//all cell voltage
		AlarmDisplayVolt1 = 0x0002,	//Battery 1
		AlarmDisplayVolt2 = 0x0004,	//Battery 2
		AlarmDisplayTemp1 = 0x0008,	//Temperature 1
		AlarmDisplayTemp2 = 0x0010,	//Temperature 2
		AlarmDisplayFuel = 0x0020,	//Fuel
		AlarmDisplayCurr = 0x0040,	//mAh
		AlarmDisplayAlt1 = 0x0080,	//Altitude
		AlarmDisplayMainCurr = 0x0100,	//main power current
		AlarmDisplayMainVolt = 0x0200,	//main power voltage
		AlarmDisplayAlt2 = 0x0400,	//Altitude
		AlarmDisplayVarioMS = 0x0800,	//m/s
		AlarmDisplayVarioM3S = 0x1000,	//m/3s
		AlarmDisplayUnknown1 = 0x2000,	//unknown
		AlarmDisplayUnknown2 = 0x4000,	//unknown
		AlarmDisplayOnActive = 0x8000,	//"ON" sign/text msg active
	};

	struct GPSDegree
	{
		uint8_t Direction; 				//#15: 000  = E= 9° 25’9360 east = 0, west = 1
		uint16_t Minutes; 				//#16: 150 157 = 0x9D = 0x039D = 0925
		uint16_t Seconds; 				//#18: 056 144 = 0x90 0x2490 = 9360

		void Set(double degree)
		{
			if (degree < 0)
			{
				Direction = 1;
				degree *= -1;
			}

			Minutes = (((uint16_t) degree) * 100);
			degree = (degree - ((uint16_t) degree)) * 60;
			Minutes += (uint16_t) degree;
			degree = (degree - ((uint16_t) degree)) * 10000;
			Seconds = (uint16_t) degree;
		}

	}__attribute__((packed, aligned(1)));

	struct
	{
		enum ReadState
		{
			StateIdle, StateSensor,
		} State;
		SensorType Sensor;
	} m_rx;

	union
	{
		uint8_t Data[45];
		struct
		{
			uint8_t StartByte;
			SensorType Sensor;
			AlarmToneType AlarmTone;
			SensorResponseType SensorResponse;
			union
			{
				uint8_t SensorData[39];
				struct
				{
					AlarmDisplayType AlarmDisplay;

					uint8_t Cells[GAMCells];	//#07 Voltage Cells (in 2 mV increments, 210 == 4.20 V)
					uint16_t Battery1;			//#13 LSB battery 1 voltage LSB value. 0.1V steps. 50 = 5.5V only pos. voltages
					uint16_t Battery2;			//#15 LSB battery 2 voltage LSB value. 0.1V steps. 50 = 5.5V only pos. voltages
					uint8_t Temp1;				//#17 Temperature 1. Offset of 20. a value of 20 = 0°C
					uint8_t Temp2;				//#18 Temperature 2. Offset of 20. a value of 20 = 0°C
					uint8_t FuelPercent;		//#19 Fuel capacity in %. Values 0--100
					uint16_t FuelML;			//#20 LSB Fuel in ml scale. Full = 65535!
					uint16_t RPM;               //#22 RPM in 10 RPM steps. 300 = 3000rpm
					uint16_t Altitude;          //#24 altitude in meters. offset of 500, 500 = 0m
					uint16_t Climb;				//#26 climb rate in 0.01m/s. Value of 30000 = 0.00 m/s
					uint8_t Climb3s;			//#28 climb rate in m/3sec. Value of 120 = 0m/3sec
					uint16_t Current;			//#29 current in 0.1A steps 100 == 10,0A
					uint16_t Voltage;		    //#31 LSB Main power voltage using 0.1V steps 100 == 10,0V
					uint16_t Capacity;          //#33 LSB used battery capacity in 10mAh steps
					uint16_t Speed;             //#35 LSB (air?) speed in km/h(?) we are using ground speed here per default
					uint8_t LowestCell;			//#37 minimum cell voltage in 2mV steps. 124 = 2,48V
					uint8_t LowestCellNum;		//#38 number of the cell with the lowest voltage
					uint16_t RPM2;              //#39 LSB 2nd RPM in 10 RPM steps. 100 == 1000rpm
					uint8_t Errors;				//#41 General Error Number (Voice Error == 12)
					uint8_t Pressure;			//#42 High pressure up to 16bar. 0,1bar scale. 20 == 2.0bar

					uint8_t Version; 			//#43: 00 version number
				}__attribute__((packed, aligned(1))) GAM;
				struct
				{
					AlarmDisplayType AlarmDisplay;
					uint8_t Cells[EAMCells];	//Cell Voltage in 2mV steps: Low 1-7, High 8 - 14 */
					uint16_t Battery1; /* Battery 1 LSB/MSB in 100mv steps; 50 == 5V */
					uint16_t Battery2; /* Battery 2 LSB/MSB in 100mv steps; 50 == 5V */
					uint8_t Temp1; /* Temp 1; Offset of 20. 20 == 0C */
					uint8_t Temp2; /* Temp 2; Offset of 20. 20 == 0C */
					uint16_t Altitude; /* Height. Offset -500. 500 == 0 */
					uint16_t Current; /* 1 = 0.1A */
					uint16_t Voltage; /* Main battery */
					uint16_t Capacity; /* mAh */
					uint16_t Climb2s; /* climb rate in 0.01m/s; 120 == 0 */
					uint8_t Climb3s; /* climb rate in m/3s; 120 == 0 WATCH the 255 (uint8) as max*/
					uint16_t RPM; /* RPM. 10er steps; 300 == 3000rpm */
					uint8_t Minutes;
					uint8_t Seconds;
					uint16_t Speed; /* Air speed in km.h */    // moet dit niet een uint8_t zijn!!!!

				}__attribute__((packed, aligned(1))) EAM;
				struct
				{
					AlarmDisplayType AlarmDisplay;
					uint8_t FlightDirection; 	//#07: 119 = Flightdir./dir. 1 = 2°; 0°(North), 90°(East), 180°(South), 270°(West)
					uint16_t Speed; 			//#08: 8 = /GPS speed low byte 8km/h
					GPSDegree Latitude;
					GPSDegree Longitude;
					uint16_t Distance; 			//#20: 027 123 = /distance low byte 6 = 6 m
					uint16_t Altitude; 			//#22: 243 244 = /Altitude low byte 500 = 0m
					uint16_t Climb1s;           //#24: climb rate in 0.01m/s. Value of 30000 = 0.00 m/s
					uint8_t Climb3s;			//#26: climb rate in m/3sec. Value of 120 = 0m/3sec
					uint8_t Satellites;			//#27: GPS.Satelites (number of satellites) (1 byte)
					uint8_t FixChar;			//#28: GPS.FixChar. (GPS fix character. display, if DGPS, 2D oder 3D) (1 byte)
					uint8_t HomeDirection;	 	//#29: HomeDirection (direction from starting point to Model position) (1 byte)
					struct
					{
						uint8_t X;	 			//#30: angle x-direction (1 byte)
						uint8_t Y; 				//#31: angle y-direction (1 byte)
						uint8_t Z;			 	//#32: angle z-direction (1 byte)
					}__attribute__((packed, aligned(1))) Angle;
					struct
					{
						uint8_t Hours;			//#33 UTC time hours
						uint8_t Minutes;		//#34 UTC time minutes
						uint8_t Seconds;		//#35 UTC time seconds
						uint8_t Milliseconds;	//#36 UTC time milliseconds
						void Set(const GPSTime& time)
						{
							Hours = time.Hour;
							Minutes = time.Minute;
							Seconds = time.Second;
						}
					}__attribute__((packed, aligned(1))) GPSTime;
					uint16_t GPSAltitude;  		//#37 mean sea level altitude
					uint8_t Vibration; 			//#39: vibration (1 bytes)
					uint8_t Ascii4; 			//#40: 00 ASCII Free Character [4]
					uint8_t Ascii5; 			//#41: 00 ASCII Free Character [5]
					uint8_t Fix; 				//#42: 00 ASCII Free Character [6], we use it for GPS FIX

					uint8_t Version; 			//#43: 00 version number
				}__attribute__((packed, aligned(1))) GPS;
				struct
				{
					uint8_t AlarmDisplay;
					int16_t Altitude; /* altitude in meters. offset of 500, 500 = 0m */
					int16_t AltitudeMax; /* altitude in meters. offset of 500, 500 = 0m */
					int16_t AltitudeMin; /* altitude in meters. offset of 500, 500 = 0m */
					uint16_t Climb; /* climb rate in 0.01m/s; 120= 0 */
					uint16_t Climb3s; /* climb rate in 0.01m/3s; 30000= 0 */
					uint16_t Climb10s; /* climb rate in 0.01m/10s; 30000 = 0 */
					char Message[21]; /* text index, should be 21 characters */
					uint8_t FreeChar1; /* appears right to home distance */
					uint8_t FreeChar2; /* appears right to home distance */
					uint8_t FreeChar3; /* GPS ASCII D=DGPS 2=2D 3=3D -=No Fix */
					uint8_t FlightDirection; /* angle heading in 2 degree steps */
					uint8_t Version; /* version number */
				}__attribute__((packed, aligned(1))) Vario;
			};
			uint8_t EndByte;
			uint8_t Parity;
		}__attribute__((packed, aligned(1)));
	} m_tx;

	HAL::USART& m_usart;
	ProtocolConfig* m_config;
	uint8_t m_run;

	void Reset();
	void UpdateGAM();
	void UpdateEAM();
	void UpdateVario();
	void UpdateGPS();
	void SendData();
protected:
	void Init() override;
	void Run(void) override;
	void ISR() override;
	void DeInit() override;
public:
	TelemetryHoTT(HAL::USART& usart) :
			TelemetryAdapterImpl(), m_usart(usart), m_config(), m_run(1)
	{
		m_tx.StartByte = 0x7C;
		m_tx.EndByte = 0x7D;
	}
}
;

} /* namespace Application*/

#endif /* APPLICATION_INCLUDE_TELEMETRYHOTT_H_ */
