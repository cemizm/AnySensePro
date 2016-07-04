using GalaSoft.MvvmLight;
using MavLink;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace xeniC.AnySense.Library.Devices
{
    public abstract class DeviceModelBase : ViewModelBase, IDisposable
    {

        public DeviceModelBase(BaseMavlinkLayer mv, UInt32 version)
        {
            this.version = version;
            mavlink = mv;
            mavlink.PacketReceived += (o, e) =>
            {
                ProcessMessage(e.Message);
            };
        }

        #region General

        private BaseMavlinkLayer mavlink;
        protected BaseMavlinkLayer Mavlink { get { return mavlink; } }

        private UInt32 version;
        public UInt32 Version { get { return version; } }

        public string Firmware { get; protected set; }

        private FlyoutViewModel flyout;
        public FlyoutViewModel Flyout
        {
            get { return flyout; }
            private set
            {
                if (flyout == value)
                    return;
                flyout = value;
                RaisePropertyChanged(() => Flyout);
                RaisePropertyChanged(() => FlyoutVisible);
            }
        }

        public bool FlyoutVisible
        {
            get { return Flyout != null; }
        }

        protected void ShowFlyout(FlyoutViewModel model)
        {
            Flyout = model;
        }

        protected void CloseFlyout()
        {
            Flyout = null;
        }

        #endregion

        #region Heartbeat

        private void Process(Msg_heartbeat msg)
        {
            FlightMode = (FlightModeType)msg.custom_mode;
            FlightController = (FlightControllerType)msg.autopilot;
        }

        private FlightModeType flightMode;
        public FlightModeType FlightMode
        {
            get { return flightMode; }
            set
            {
                if (flightMode == value)
                    return;

                flightMode = value;
                RaisePropertyChanged(() => FlightMode);
            }
        }

        private FlightControllerType flightController;
        public FlightControllerType FlightController
        {
            get { return flightController; }
            set
            {
                if (flightController == value)
                    return;
                flightController = value;
                RaisePropertyChanged(() => FlightController);
            }
        }


        #endregion

        #region System Status

        private void Process(Msg_sys_status msg)
        {
            Battery = msg.voltage_battery / 1000f;
            Current = msg.current_battery / 100f;
            Charge = msg.battery_remaining;
        }

        private float battery;
        /// <summary>
        /// Battery in 0.00 V
        /// </summary>
        public float Battery
        {
            get { return battery; }
            set
            {
                if (battery == value)
                    return;

                battery = value;
                RaisePropertyChanged(() => Battery);
            }
        }

        private float current;
        /// <summary>
        /// Current in 0.00 A
        /// </summary>
        public float Current
        {
            get { return current; }
            set
            {
                if (current == value)
                    return;

                current = value;
                RaisePropertyChanged(() => Current);
            }
        }

        private int charge;
        /// <summary>
        /// Battery Charge in %
        /// </summary>
        public int Charge
        {
            get { return charge; }
            set
            {
                if (charge == value)
                    return;

                charge = value;
                RaisePropertyChanged(() => Charge);
            }
        }



        #endregion

        #region GPS

        private void Process(Msg_gps_raw_int gps)
        {
            GPSFix = (GPSFixType)gps.fix_type;
            Position = new PositionData() { Latitude = gps.lat / 10000000d, Longitude = gps.lon / 10000000d };
            Altitude = (gps.alt / 1000f);
            HDOP = (gps.eph / 100f);
            VDOP = (gps.epv / 100f);
            Speed = (gps.vel / 100f);
            COG = (gps.cog / 100f);
            Sats = gps.satellites_visible;
        }

        private PositionData position;
        public PositionData Position
        {
            get { return position; }
            set
            {
                if (position != value)
                {
                    position = value;
                    RaisePropertyChanged(() => Position);
                }
            }
        }

        private float cog;

        /// <summary>
        /// Course over Ground in 0.00 °
        /// </summary>
        public float COG
        {
            get { return cog; }
            set
            {
                if (cog == value)
                    return;

                cog = value;
                RaisePropertyChanged(() => COG);
            }
        }

        private float altitude;
        /// <summary>
        /// GPS based Altitude in 0.00 m
        /// </summary>
        public float Altitude
        {
            get { return altitude; }
            set
            {
                if (altitude == value)
                    return;
                altitude = value;
                RaisePropertyChanged(() => Altitude);
            }
        }

        private GPSFixType gpsFix;
        public GPSFixType GPSFix
        {
            get { return gpsFix; }
            set
            {
                if (gpsFix == value)
                    return;
                gpsFix = value;
                RaisePropertyChanged(() => GPSFix);
            }
        }

        private float hdop;
        public float HDOP
        {
            get { return hdop; }
            set
            {
                if (hdop == value)
                    return;
                hdop = value;
                RaisePropertyChanged(() => HDOP);
            }
        }

        private float vdop;
        public float VDOP
        {
            get { return vdop; }
            set
            {
                if (vdop == value)
                    return;
                vdop = value;
                RaisePropertyChanged(() => VDOP);
            }
        }

        private float speed;
        /// <summary>
        /// Speed in 0.00 m/s
        /// </summary>
        public float Speed
        {
            get { return speed; }
            set
            {
                if (speed == value)
                    return;
                speed = value;
                RaisePropertyChanged(() => Speed);
            }
        }

        private int sats;
        public int Sats
        {
            get { return sats; }
            set
            {
                if (sats == value)
                    return;
                sats = value;
                RaisePropertyChanged(() => Sats);
            }
        }

        #endregion

        #region VFR HUD

        private void Process(Msg_vfr_hud msg)
        {
            ClimbRate = msg.climb;
            Throttle = msg.throttle;
        }

        private float climbRate;
        /// <summary>
        /// Climb rate in 0.00 m/s
        /// </summary>
        public float ClimbRate
        {
            get { return climbRate; }
            set
            {
                if (climbRate == value)
                    return;
                climbRate = value;
                RaisePropertyChanged(() => ClimbRate);
            }
        }

        private int throttle;
        /// <summary>
        /// Throttle in %
        /// </summary>
        public int Throttle
        {
            get { return throttle; }
            set
            {
                if (throttle == value)
                    return;

                throttle = value;
                RaisePropertyChanged(() => Throttle);
            }
        }


        #endregion

        #region Attitude

        private void Process(Msg_attitude msg)
        {
            Roll = msg.roll * 180.0 / Math.PI;
            Pitch = msg.pitch * 180.0 / Math.PI;
            Heading = msg.yaw * 180.0 / Math.PI;
        }

        private double roll;
        public double Roll
        {
            get { return roll; }
            set
            {
                if (roll != value)
                {
                    roll = value;
                    RaisePropertyChanged(() => Roll);
                }
            }
        }

        private double pitch;
        public double Pitch
        {
            get { return pitch; }
            set
            {
                if (pitch != value)
                {
                    pitch = value;
                    RaisePropertyChanged(() => Pitch);
                }
            }
        }

        private double heading;
        public double Heading
        {
            get { return heading; }
            set
            {
                if (heading != value)
                {
                    heading = value;
                    RaisePropertyChanged(() => Heading);
                }
            }
        }

        #endregion

        #region RC Channels

        private void Process(Msg_rc_channels_raw msg)
        {
            Channel1 = msg.chan1_raw;
            Channel2 = msg.chan2_raw;
            Channel3 = msg.chan3_raw;
            Channel4 = msg.chan4_raw;
            Channel5 = msg.chan5_raw;
            Channel6 = msg.chan6_raw;
            Channel7 = msg.chan7_raw;
            Channel8 = msg.chan8_raw;
        }

        private int channel1;
        /// <summary>
        /// Aileron
        /// </summary>
        public int Channel1
        {
            get { return channel1; }
            set
            {
                if (channel1 == value)
                    return;

                channel1 = value;
                RaisePropertyChanged(() => Channel1);
            }
        }

        private int channel2;
        /// <summary>
        /// Elevator
        /// </summary>
        public int Channel2
        {
            get { return channel2; }
            set
            {
                if (channel2 == value)
                    return;

                channel2 = value;
                RaisePropertyChanged(() => Channel2);
            }
        }

        private int channel3;
        /// <summary>
        /// Throttle
        /// </summary>
        public int Channel3
        {
            get { return channel3; }
            set
            {
                if (channel3 == value)
                    return;

                channel3 = value;
                RaisePropertyChanged(() => Channel3);
            }
        }

        private int channel4;
        /// <summary>
        /// Roll
        /// </summary>
        public int Channel4
        {
            get { return channel4; }
            set
            {
                if (channel4 == value)
                    return;

                channel4 = value;
                RaisePropertyChanged(() => Channel4);
            }
        }

        private int channel5;
        /// <summary>
        /// X1
        /// </summary>
        public int Channel5
        {
            get { return channel5; }
            set
            {
                if (channel5 == value)
                    return;

                channel5 = value;
                RaisePropertyChanged(() => Channel5);
            }
        }

        private int channel6;
        /// <summary>
        /// X2
        /// </summary>
        public int Channel6
        {
            get { return channel6; }
            set
            {
                if (channel6 == value)
                    return;

                channel6 = value;
                RaisePropertyChanged(() => Channel6);
            }
        }

        private int channel7;
        /// <summary>
        /// U
        /// </summary>
        public int Channel7
        {
            get { return channel7; }
            set
            {
                if (channel7 == value)
                    return;

                channel7 = value;
                RaisePropertyChanged(() => Channel7);
            }
        }

        private int channel8;
        /// <summary>
        /// 
        /// </summary>
        public int Channel8
        {
            get { return channel8; }
            set
            {
                if (channel8 == value)
                    return;

                channel8 = value;
                RaisePropertyChanged(() => Channel8);
            }
        }

        #endregion

        #region Battery Status

        private void Process(Msg_battery_status msg)
        {
            Temperature = msg.temperature;

            Cell1 = msg.voltages[0] / 1000f;
            Cell2 = msg.voltages[1] / 1000f;
            Cell3 = msg.voltages[2] / 1000f;
            Cell4 = msg.voltages[3] / 1000f;
            Cell5 = msg.voltages[4] / 1000f;
            Cell6 = msg.voltages[5] / 1000f;
            Cell7 = msg.voltages[6] / 1000f;
            Cell8 = msg.voltages[7] / 1000f;
            Cell9 = msg.voltages[8] / 1000f;
            Cell10 = msg.voltages[9] / 1000f;
        }

        private int temperature;
        public int Temperature
        {
            get { return temperature; }
            set
            {
                if (temperature == value)
                    return;

                temperature = value;
                RaisePropertyChanged(() => Temperature);
            }
        }

        private float cell1;
        public float Cell1
        {
            get { return cell1; }
            set
            {
                if (cell1 == value)
                    return;
                cell1 = value;
                RaisePropertyChanged(() => Cell1);
            }
        }

        private float cell2;
        public float Cell2
        {
            get { return cell2; }
            set
            {
                if (cell2 == value)
                    return;
                cell2 = value;
                RaisePropertyChanged(() => Cell2);
            }
        }

        private float cell3;
        public float Cell3
        {
            get { return cell3; }
            set
            {
                if (cell3 == value)
                    return;
                cell3 = value;
                RaisePropertyChanged(() => Cell3);
            }
        }

        private float cell4;
        public float Cell4
        {
            get { return cell4; }
            set
            {
                if (cell4 == value)
                    return;
                cell4 = value;
                RaisePropertyChanged(() => Cell4);
            }
        }

        private float cell5;
        public float Cell5
        {
            get { return cell5; }
            set
            {
                if (cell5 == value)
                    return;
                cell5 = value;
                RaisePropertyChanged(() => Cell5);
            }
        }

        private float cell6;
        public float Cell6
        {
            get { return cell6; }
            set
            {
                if (cell6 == value)
                    return;
                cell6 = value;
                RaisePropertyChanged(() => Cell6);
            }
        }

        private float cell7;
        public float Cell7
        {
            get { return cell7; }
            set
            {
                if (cell7 == value)
                    return;
                cell7 = value;
                RaisePropertyChanged(() => Cell7);
            }
        }

        private float cell8;
        public float Cell8
        {
            get { return cell8; }
            set
            {
                if (cell8 == value)
                    return;
                cell8 = value;
                RaisePropertyChanged(() => Cell8);
            }
        }

        private float cell9;
        public float Cell9
        {
            get { return cell9; }
            set
            {
                if (cell9 == value)
                    return;
                cell9 = value;
                RaisePropertyChanged(() => Cell9);
            }
        }

        private float cell10;
        public float Cell10
        {
            get { return cell10; }
            set
            {
                if (cell10 == value)
                    return;
                cell10 = value;
                RaisePropertyChanged(() => Cell10);
            }
        }

        #endregion

        #region Logging

        private void Process(Msg_log_data msg)
        {
            System.Diagnostics.Trace.WriteLine(ASCIIEncoding.UTF8.GetString(msg.data));
        }

        #endregion

        #region Message Process

        protected void ProcessMessage(MavlinkMessage msg)
        {
            if (msg.GetType() == typeof(Msg_heartbeat))
                Process(msg as Msg_heartbeat);
            else if (msg.GetType() == typeof(Msg_sys_status))
                Process(msg as Msg_sys_status);
            else if (msg.GetType() == typeof(Msg_gps_raw_int))
                Process(msg as Msg_gps_raw_int);
            else if (msg.GetType() == typeof(Msg_vfr_hud))
                Process(msg as Msg_vfr_hud);
            else if (msg.GetType() == typeof(Msg_attitude))
                Process(msg as Msg_attitude);
            else if (msg.GetType() == typeof(Msg_rc_channels_raw))
                Process(msg as Msg_rc_channels_raw);
            else if (msg.GetType() == typeof(Msg_battery_status))
                Process(msg as Msg_battery_status);
            else if (msg.GetType() == typeof(Msg_log_data))
                Process(msg as Msg_log_data);
            else
            {
            }
        }

        #endregion

        public void Dispose()
        {
            try
            {
                if (mavlink != null)
                    mavlink.Dispose();
            }
            catch { }
        }

        #region Nested Types

        public enum FlightControllerType
        {
            [Description("Not connected")]
            Unknown = 8,
            [Description("DJI Naza V1/V2 Phantom V1")]
            Naza = 18,
            [Description("DJI Phantom V2")]
            Phantom = 19,
            [Description("DJI Wookong M")]
            Wookong = 20,
            [Description("DJI A2")]
            A2 = 21,
            [Description("Pixhawk/APM")]
            Pixhawk = 12,
            [Description("Tarot ZYX-M")]
            ZYX_M = 22,
            [Description("Align APS-M")]
            APS_M = 23,
        }

        public enum FlightModeType
        {
            Manual = 1,
            GPS = 0,
            Failsafe = 6,
            Attitude = 2
        }

        public enum GPSFixType
        {
            [Description("No Fix")]
            None = 0,
            [Description("2D")]
            FIX_2D = 2,
            [Description("3D")]
            FIX_3D = 3,
            [Description("DGPS")]
            FIX_DGPS = 4,
        }

        public class PositionData
        {
            public double Longitude { get; set; }
            public double Latitude { get; set; }

            public override string ToString()
            {
                return string.Format(CultureInfo.GetCultureInfo("en-US"), "{0:0.000000} {1:0.000000}", Latitude, Longitude);
            }
        }

        #endregion
    }
}
