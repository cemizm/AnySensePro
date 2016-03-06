using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.CommandWpf;
using GalaSoft.MvvmLight.Ioc;
using MavLink;
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using xeniC.AnySense.Library.Extensions;

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


        private RelayCommand _showConfiguration;
        public RelayCommand ShowConfiguration
        {
            get
            {
                if (_showConfiguration == null)
                {
                    _showConfiguration = new RelayCommand(() =>
                    {
                        ShowFlyout(new SettingsModel(CloseFlyout, Mavlink));
                    }, () => Version == LatestVersion);
                }

                return _showConfiguration;
            }
        }

        #region Nested Types

        public class SettingsModel : FlyoutViewModel
        {
            private BaseMavlinkLayer mavlink;
            private Action closeAction;

            public SettingsModel(Action closeAction, BaseMavlinkLayer mavlink)
            {
                this.closeAction = closeAction;
                this.mavlink = mavlink;

                Load();
            }

            #region Loading/Saving

            private void Load()
            {
                IsLoading = true;

                Task.Run(() =>
                {
                    EventWaitHandle ev = new EventWaitHandle(false, EventResetMode.ManualReset);
                    PacketReceivedEventHandler h = new PacketReceivedEventHandler((o, e) =>
                    {
                        if (e.Message is Msg_configuration_data)
                        {
                            Deserialize(e.Message as Msg_configuration_data);
                            ev.Set();
                        }
                    });

                    mavlink.PacketReceived += h;

                    mavlink.SendMessage(new Msg_configuration_control() { command = (byte)CONFIG_COMMAND.CONFIG_COMMAND_GET_CONFIGURATION });

                    if (!ev.WaitOne(5000))
                        closeAction.Invoke();

                    mavlink.PacketReceived -= h;
                    IsLoading = false;
                });
            }

            private async Task Save()
            {
                IsLoading = true;

                bool success = false;

                await Task.Run(() =>
                {
                    EventWaitHandle ev = new EventWaitHandle(false, EventResetMode.ManualReset);
                    PacketReceivedEventHandler h = new PacketReceivedEventHandler((o, e) =>
                    {
                        if (e.Message is Msg_command_ack)
                        {
                            Msg_command_ack msg = e.Message as Msg_command_ack;
                            success = msg.command == (byte)MAV_CMD_ACK.MAV_CMD_ACK_OK;
                            ev.Set();
                        }
                    });

                    mavlink.PacketReceived += h;

                    Msg_configuration_data data;
                    Serialize(out data);
                    mavlink.SendMessage(data);

                    if (!ev.WaitOne(5000))
                        success = false;

                    mavlink.PacketReceived -= h;
                });

                if (success)
                    closeAction.Invoke();

                IsLoading = false;
            }

            private bool isLoading;
            public bool IsLoading
            {
                get { return isLoading; }
                private set
                {
                    if (isLoading == value)
                        return;

                    isLoading = value;
                    RaisePropertyChanged(() => IsLoading);
                    RaisePropertyChanged(() => CloseCommand);
                }
            }

            #endregion

            public override string Title
            {
                get { return "Settings"; }
            }

            private RelayCommand closeCommand;
            public override RelayCommand CloseCommand
            {
                get
                {
                    if (closeCommand == null)
                    {
                        closeCommand = new RelayCommand(async () =>
                        {
                            await Save();
                            closeAction.Invoke();
                        }, () => !IsLoading);
                    }

                    return closeCommand;
                }
            }

            #region Serialize/Deserialize

            private void Serialize(out Msg_configuration_data msg)
            {
                int offset = 0;
                msg = new Msg_configuration_data();
                msg.data = new byte[240];
                msg.data[offset++] = (byte)Protocol;

                if (Settings != null)
                    Settings.Serialize(msg.data, offset);
            }

            private void Deserialize(Msg_configuration_data msg)
            {
                int offset = 0;
                Protocol = (TelemetryProtocol)msg.data[offset++];

                if (Settings != null)
                    Settings.DeSerialize(msg.data, offset);
            }

            #endregion

            #region Settings

            private List<EnumExtensions.EnumItem> protocolSource;
            public List<EnumExtensions.EnumItem> ProtocolSource
            {
                get
                {
                    if (protocolSource == null)
                        protocolSource = typeof(TelemetryProtocol).GetDataSource();

                    return protocolSource;
                }
            }

            private TelemetryProtocol protocol;
            public TelemetryProtocol Protocol
            {
                get { return protocol; }
                set
                {
                    if (protocol == value)
                        return;

                    protocol = value;
                    Settings = GetSettings(value);
                    RaisePropertyChanged(() => Protocol);
                }
            }

            private ProtocolSettingsModel settings;
            public ProtocolSettingsModel Settings
            {
                get { return settings; }
                private set
                {
                    if (settings == value)
                        return;

                    settings = value;
                    RaisePropertyChanged(() => Settings);
                }
            }

            private ProtocolSettingsModel GetSettings(TelemetryProtocol protocol)
            {
                switch (protocol)
                {
                    case TelemetryProtocol.FrSky:
                        return new SettingsFrSkyModel();
                    case TelemetryProtocol.MAVLink:
                        break;
                    case TelemetryProtocol.HoTT:
                        break;
                    case TelemetryProtocol.Jeti:
                        break;
                    case TelemetryProtocol.Futaba:
                        break;
                    case TelemetryProtocol.Spektrum:
                        break;
                    case TelemetryProtocol.Multiplex:
                        break;
                }

                return null;
            }

            #endregion
        }

        public enum TelemetryProtocol : byte
        {
            [Description("Disabled")]
            None = 0,
            [Description("FrSky S.Port")]
            FrSky = 1,
            [Description("Mavlink")]
            MAVLink = 2,
            [Description("Graupner/SJ HoTT")]
            HoTT = 3,
            [Description("Jeti Duplex EX")]
            Jeti = 4,
            [Description("Futaba SBUS2")]
            Futaba = 5,
            [Description("Spektrum X-Bus")]
            Spektrum = 6,
            [Description("Multiplex MSB")]
            Multiplex = 7,
        }

        public abstract class ProtocolSettingsModel : ViewModelBase
        {
            private MavLink.FrameworkBitConverter converter;

            public ProtocolSettingsModel()
            {
                converter = new MavLink.FrameworkBitConverter();
                converter.SetDataIsLittleEndian(MavLink.MavlinkSettings.IsLittleEndian);
            }

            internal MavLink.FrameworkBitConverter Converter { get { return converter; } }

            public abstract void DeSerialize(byte[] data, int offset);
            public abstract void Serialize(byte[] data, int offset);
        }

        public class SettingsFrSkyModel : ProtocolSettingsModel
        {
            public override void DeSerialize(byte[] data, int offset)
            {
            }

            public override void Serialize(byte[] data, int offset)
            {
            }
        }

        #endregion
    }
}
