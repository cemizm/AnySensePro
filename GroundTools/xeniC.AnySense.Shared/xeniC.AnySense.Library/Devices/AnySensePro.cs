using GalaSoft.MvvmLight.CommandWpf;
using GalaSoft.MvvmLight.Ioc;
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
        private const UInt32 LatestVersion = 0x01000103;

        public AnySensePro(BaseMavlinkLayer mv, UInt32 version)
            : base(mv, version)
        {
            Firmware = string.Format("AnySense Pro {0}.{1}.{2}", (byte)(version >> 24), (byte)(version >> 16), (byte)(version >> 8));
            CheckForUpdate();
        }

        private RelayCommand _execUpdate;
        public RelayCommand ExecUpdate
        {
            get
            {
                if (_execUpdate == null)
                {
                    _execUpdate = new RelayCommand(() =>
                    {
                        Task.Run(() => DoUpdate());
                    });
                }
                return _execUpdate;
            }
        }

        private void CheckForUpdate()
        {
            if (Version >= LatestVersion)
                return;

            Task.Run(() => DoUpdate());
        }

        public async void DoUpdate()
        {
            IDialogController dlg = SimpleIoc.Default.GetInstance<IDialogController>();

            if (dlg == null)
                throw new InvalidOperationException("Kein Dialog Controller vorhanden!");

            IProgressController progress = await dlg.ShowProgress("Firmware Update", "Initializing Update...");
            progress.SetIndeterminate();


            int state = 0;
            int timeout = 0;
            int read = 0;
            int total = 0;

            Msg_command_ack msg_ack = null;
            Msg_encapsulated_data data = new Msg_encapsulated_data();
            data.seqnr = 0;
            data.data = new byte[240];

            Stream s = Assembly.GetAssembly(GetType()).GetManifestResourceStream(@"xeniC.AnySense.Library.Firmware.AnySensePro.bin");

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
                            progress.SetMessage("Uploading Firmware...");
                            state++;
                        }

                        read = s.Read(data.data, 0, data.data.Length);

                        Mavlink.SendMessage(data);

                        if (read < data.data.Length)
                            state++;

                        data.seqnr++;

                        progress.SetProgress((double)total / (double)s.Length);

                        //UpdatePercent = (int)(( * 100f);

                        total += read;
                    }
                    else if (state == 2)
                    {
                        progress.SetMessage("Updating Device...");
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

            s.Close();
            s.Dispose();

            Mavlink.PacketReceived -= pr;

            if (timeout >= 5)
            {
                await progress.CloseAsync();
                await dlg.ShowMessage("Firmware Update", "Error while updating!", DialogStyle.Affirmative);
                /*
                progress.SetProgress(1d);
                progress.SetMessage("Error while updating!");

                await Task.Delay(3000);*/
            }
            else
            {
                await Task.Delay(1500);
                await progress.CloseAsync();
            }
        }
    }
}
