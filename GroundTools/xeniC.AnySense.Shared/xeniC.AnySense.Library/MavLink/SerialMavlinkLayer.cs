using MavLink;
using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MavLink
{
    public class SerialMavlinkLayer : BaseMavlinkLayer
    {
        private SerialPort serial;

        public SerialMavlinkLayer(string port)
            : this(new SerialPort(port))
        {
        }

        public SerialMavlinkLayer(SerialPort sp)
        {
            serial = sp;
            serial.BaudRate = 57600;
            serial.DataReceived += (o, e) =>
            {
                try
                {
                    byte[] data = new byte[sp.BytesToRead];
                    sp.Read(data, 0, data.Length);
                    ParseBytes(data);
                }
                catch { }
            };

            serial.Open();
        }

        public override void SendMessage(MavlinkMessage msg)
        {
            MavlinkPacket packet = CreatePacket(msg);
            byte[] bytes = Send(packet);
            try { serial.Write(bytes, 0, bytes.Length); }
            catch { }
        }

        private MavlinkPacket CreatePacket(MavlinkMessage msg)
        {
            MavlinkPacket packet = new MavlinkPacket();
            packet.ComponentId = (int)MAV_COMPONENT.MAV_COMP_ID_SYSTEM_CONTROL;
            packet.SystemId = 0xCE;
            packet.Message = msg;

            return packet;
        }

        public override void Dispose()
        {
            try
            {
                if (serial != null)
                    serial.Dispose();
            }
            catch { }
        }
    }
}
