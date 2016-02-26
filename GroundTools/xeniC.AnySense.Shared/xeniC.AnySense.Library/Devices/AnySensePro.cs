using GalaSoft.MvvmLight.CommandWpf;
using MavLink;
using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace xeniC.AnySense.Library.Devices
{
    public class AnySensePro : DeviceModelBase
    {
        private const UInt32 LatestVersion = 0x00010002;

        public AnySensePro(SerialMavlinkLayer mv, UInt32 version)
            : base(mv, version)
        {
            Firmware = string.Format("AnySense Pro {0}.{1}.{2}", (byte)(version >> 16), (byte)(version >> 8), (byte)version);

        }

        private RelayCommand _executeUpdate;
        public RelayCommand ExecuteUpdate
        {
            get
            {
                if (_executeUpdate == null)
                    _executeUpdate = new RelayCommand(DoUpdate, () => { return LatestVersion > Version; });

                return _executeUpdate;
            }
        }

        private string updateMessage;
        public string UpdateMessage
        {
            get { return updateMessage; }
            private set
            {
                if (updateMessage == value)
                    return;

                updateMessage = value;
                RaisePropertyChanged(() => UpdateMessage);
            }
        }

        private int updatePercent;
        public int UpdatePercent
        {
            get { return updatePercent; }
            private set
            {
                if (updatePercent == value)
                    return;

                updatePercent = value;
                RaisePropertyChanged(() => UpdatePercent);
            }
        }

        public async void DoUpdate()
        {
            int state = 0;
            int timeout = 0;
            int read = 0;
            int total = 0;

            Msg_command_ack msg_ack = null;
            Msg_encapsulated_data data = new Msg_encapsulated_data();
            data.seqnr = 0;
            data.data = new byte[240];

            Stream s = Assembly.GetAssembly(GetType()).GetManifestResourceStream(@"xeniC.AnySense.Library.Firmware.AnySensePro.bin");

            UpdatePercent = 0;
            UpdateMessage = "Initializing Update...";

            PacketReceivedEventHandler pr = (o, e) =>
            {
                if (e.Message.GetType() == typeof(Msg_command_ack))
                {
                    msg_ack = e.Message as Msg_command_ack;

                    timeout = 0;

                    if (msg_ack.command == (ushort)MAV_CMD_ACK.MAV_CMD_ACK_ERR_FAIL)
                        state = -1;

                    if (state < 0)
                        return;

                    if (state == 0 || state == 1)
                    {
                        if (state == 0)
                        {
                            UpdateMessage = "Uploading Firmware...";
                            state++;
                        }

                        read = s.Read(data.data, 0, data.data.Length);

                        Mavlink.SendMessage(data);

                        if (read < data.data.Length)
                            state++;

                        data.seqnr++;

                        UpdatePercent = (int)(((float)total / (float)s.Length) * 100f);

                        total += read;
                    }
                    else if (state == 2)
                    {
                        UpdateMessage = "Updating Device...";
                        state++;
                    }

                }
            };


            Mavlink.PacketReceived += pr;

            Msg_data_transmission_handshake msg = new Msg_data_transmission_handshake();
            msg.type = 0;
            msg.size = (uint)s.Length;
            Mavlink.SendMessage(msg);

            do
            {
                await Task.Delay(500);
                timeout++;
            } while (state != 3 && state != -1 && timeout < 5);

            if (timeout >= 5)
                UpdateMessage = "Error";

            s.Close();
            s.Dispose();

            Mavlink.PacketReceived -= pr;
        }
    }
}
