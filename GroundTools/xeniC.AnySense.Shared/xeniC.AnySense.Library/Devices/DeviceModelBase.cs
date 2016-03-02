using GalaSoft.MvvmLight;
using MavLink;
using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace xeniC.AnySense.Library.Devices
{
    public abstract class DeviceModelBase : ViewModelBase, IDisposable
    {
        private BaseMavlinkLayer mavlink;

        private UInt32 version;

        private double roll;
        private double pitch;
        private double heading;
        private PositionData position;

        public DeviceModelBase(BaseMavlinkLayer mv, UInt32 version)
        {
            this.version = version;
            mavlink = mv;
            mavlink.PacketReceived += (o, e) =>
            {
                ProcessMessage(e.Message);
            };
        }

        #region Properties

        protected BaseMavlinkLayer Mavlink { get { return mavlink; } }

        public UInt32 Version { get { return version; } }

        public string Firmware { get; protected set; }

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

        #endregion

        #region Message Process

        protected void ProcessMessage(MavlinkMessage msg)
        {
            if (msg.GetType() == typeof(Msg_vfr_hud))
                Process(msg as Msg_vfr_hud);
            else if (msg.GetType() == typeof(Msg_attitude))
                Process(msg as Msg_attitude);
            else if (msg.GetType() == typeof(Msg_gps_raw_int))
                Process(msg as Msg_gps_raw_int);
        }

        private void Process(Msg_attitude msg)
        {
            Roll = msg.roll * 180.0 / Math.PI;
            Pitch = msg.pitch * 180.0 / Math.PI;
            Heading = msg.yaw * 180.0 / Math.PI;
        }

        private void Process(Msg_vfr_hud hud)
        {
        }

        private void Process(Msg_gps_raw_int gps)
        {
            Position = new PositionData() { Latitude = gps.lat / 10000000d, Longitude = gps.lon / 10000000d };
        }

        #endregion

        public void Dispose()
        {
            if (mavlink != null)
                mavlink.Dispose();
        }

        public class PositionData
        {
            public double Longitude { get; set; }
            public double Latitude { get; set; }
        }

    }
}
