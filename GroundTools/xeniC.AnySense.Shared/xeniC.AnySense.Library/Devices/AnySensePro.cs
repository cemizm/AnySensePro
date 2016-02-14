using MavLink;
using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace xeniC.AnySense.Library.Devices
{
    public class AnySensePro : DeviceModelBase
    {
        public AnySensePro(SerialMavlinkLayer mv, UInt32 version)
            : base(mv, version)
        {
            Firmware = string.Format("AnySense Pro {0}.{1}.{2}", (byte)(version >> 16), (byte)(version >> 8), (byte)version);

        }

    }
}
