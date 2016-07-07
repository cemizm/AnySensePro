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
        private const UInt32 LatestVersion = 0x02020200;

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

            Thread.Sleep(5000);

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

        #region Nested Types

        public class SettingsModel : FlyoutViewModel
        {
            private BaseMavlinkLayer mavlink;
            private Action closeAction;
            private MavLink.FrameworkBitConverter converter;

            public SettingsModel(Action closeAction, BaseMavlinkLayer mavlink)
            {
                this.closeAction = closeAction;
                this.mavlink = mavlink;

                Load();

                converter = new MavLink.FrameworkBitConverter();
                converter.SetDataIsLittleEndian(MavLink.MavlinkSettings.IsLittleEndian);
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
                int offset = 1;
                msg = new Msg_configuration_data();
                msg.data = new byte[240];
                msg.data[offset++] = (byte)Protocol;
                msg.data[offset++] = AlarmCharge;
                converter.GetBytes(alarmDistance, msg.data, offset); offset += 2;

                offset = 32;
                if (Settings != null)
                    Settings.Serialize(msg.data, offset);
            }

            private void Deserialize(Msg_configuration_data msg)
            {
                int offset = 1;

                Protocol = (TelemetryProtocol)msg.data[offset++];
                AlarmCharge = (byte)msg.data[offset++];
                AlarmDistance = converter.ToUInt16(msg.data, offset); offset += 2; 
                offset = 32;

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
                        return new SettingsMAVLinkModel();
                    case TelemetryProtocol.HoTT:
                        return new SettingsHoTTModel();
                    case TelemetryProtocol.Jeti:
                        return new SettingsJetiModel();
                    case TelemetryProtocol.Futaba:
                        return new SettingsFutabaModel();
                    case TelemetryProtocol.Spektrum:
                        return new SettingsSpektrumModel();
                    case TelemetryProtocol.Multiplex:
                        return new SettingsMultiplexModel();
                }

                return null;
            }

            private byte alarmCharge;

            public byte AlarmCharge
            {
                get { return alarmCharge; }
                set
                {
                    if (alarmCharge == value)
                        return;

                    alarmCharge = value;
                    RaisePropertyChanged(() => AlarmCharge);
                }
            }

            private UInt16 alarmDistance;
            public UInt16 AlarmDistance
            {
                get { return alarmDistance; }
                set
                {
                    if (alarmDistance == value)
                        return;

                    alarmDistance = value;
                    RaisePropertyChanged(() => AlarmDistance);
                }
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
            private UInt16 isValid;
            public SettingsFrSkyModel()
            {
                isValid = 0xCB00 + (byte)TelemetryProtocol.FrSky;

                SensorId = FrSkySensorId.Id_0xCB;
                VarioEnable = true;
                FLVSSEnable = true;
                FCSEnable = true;
                GPSEnable = true;
                RPMEnable = true;
                T1 = SensorValueMapping.Satellites;
                T2 = SensorValueMapping.GPSFix;
                Fuel = SensorValueMapping.Combined;
            }

            #region Properties

            private List<EnumExtensions.EnumItem> sensorIdSource;
            public List<EnumExtensions.EnumItem> SensorIdSource
            {
                get
                {
                    if (sensorIdSource == null)
                        sensorIdSource = typeof(FrSkySensorId).GetDataSource();

                    return sensorIdSource;
                }
            }

            private List<EnumExtensions.EnumItem> sensorValueSource;
            public List<EnumExtensions.EnumItem> SensorValueSource
            {
                get
                {
                    if (sensorValueSource == null)
                        sensorValueSource = typeof(SensorValueMapping).GetDataSource();

                    return sensorValueSource;
                }
            }

            private FrSkySensorId sensorId;
            public FrSkySensorId SensorId
            {
                get { return sensorId; }
                set
                {
                    if (sensorId == value)
                        return;

                    sensorId = value;
                    RaisePropertyChanged(() => SensorId);
                }
            }

            private bool varioEnable;
            public bool VarioEnable
            {
                get { return varioEnable; }
                set
                {
                    if (varioEnable == value)
                        return;
                    varioEnable = value;
                    RaisePropertyChanged(() => VarioEnable);
                }
            }

            private bool flvssEnable;
            public bool FLVSSEnable
            {
                get { return flvssEnable; }
                set
                {
                    if (flvssEnable == value)
                        return;
                    flvssEnable = value;
                    RaisePropertyChanged(() => FLVSSEnable);
                }
            }

            private bool fcsEnable;
            public bool FCSEnable
            {
                get { return fcsEnable; }
                set
                {
                    if (fcsEnable == value)
                        return;
                    fcsEnable = value;
                    RaisePropertyChanged(() => FCSEnable);
                }
            }

            private bool gpsEnable;
            public bool GPSEnable
            {
                get { return gpsEnable; }
                set
                {
                    if (gpsEnable == value)
                        return;
                    gpsEnable = value;
                    RaisePropertyChanged(() => GPSEnable);
                }
            }

            private bool rpmEnable;
            public bool RPMEnable
            {
                get { return rpmEnable; }
                set
                {
                    if (rpmEnable == value)
                        return;
                    rpmEnable = value;
                    RaisePropertyChanged(() => RPMEnable);
                }
            }

            private SensorValueMapping t1;
            public SensorValueMapping T1
            {
                get { return t1; }
                set
                {
                    if (t1 == value)
                        return;
                    t1 = value;
                    RaisePropertyChanged(() => T1);
                }
            }

            private SensorValueMapping t2;
            public SensorValueMapping T2
            {
                get { return t2; }
                set
                {
                    if (t2 == value)
                        return;
                    t2 = value;
                    RaisePropertyChanged(() => T2);
                }
            }

            private SensorValueMapping fuel;
            public SensorValueMapping Fuel
            {
                get { return fuel; }
                set
                {
                    if (fuel == value)
                        return;
                    fuel = value;
                    RaisePropertyChanged(() => Fuel);
                }
            }

            #endregion

            #region Serialization / Deserialization

            public override void DeSerialize(byte[] data, int offset)
            {
                isValid = Converter.ToUInt16(data, offset);
                offset += 2;

                SensorId = (FrSkySensorId)data[offset++];
                VarioEnable = data[offset++] == 1;
                FLVSSEnable = data[offset++] == 1;
                FCSEnable = data[offset++] == 1;
                GPSEnable = data[offset++] == 1;
                RPMEnable = data[offset++] == 1;
                T1 = (SensorValueMapping)data[offset++];
                T2 = (SensorValueMapping)data[offset++];
                Fuel = (SensorValueMapping)data[offset++];
            }

            public override void Serialize(byte[] data, int offset)
            {
                Converter.GetBytes(isValid, data, offset);
                offset += 2;
                data[offset++] = (byte)SensorId;
                data[offset++] = (byte)(VarioEnable ? 1 : 0);
                data[offset++] = (byte)(FLVSSEnable ? 1 : 0);
                data[offset++] = (byte)(FCSEnable ? 1 : 0);
                data[offset++] = (byte)(GPSEnable ? 1 : 0);
                data[offset++] = (byte)(RPMEnable ? 1 : 0);
                data[offset++] = (byte)T1;
                data[offset++] = (byte)T2;
                data[offset++] = (byte)Fuel;
            }

            #endregion

            #region Nested Types

            public enum SensorValueMapping : byte
            {
                [Description("Disabled / default")]
                None = 0,
                [Description("Visible Satellites")]
                Satellites = 1,
                [Description("GPS Fix")]
                GPSFix = 2,
                [Description("Flight mode")]
                FlightMode = 3,
                [Description("Combined (LUA Script)")]
                Combined = 4,
            };

            public enum FrSkySensorId : byte
            {

                [Description("Phy. Id 01 (FVAS-02)")]
                Id_0x00 = 0x00, //Physical 01 - Vario
                [Description("Phy. Id 02 (FLVSS)")]
                Id_0xA1 = 0xA1, //Physical 02 - FLVSS
                [Description("Phy. Id 03 (FCS-40A)")]
                Id_0x22 = 0x22, //Physical 03 - FCS 40A 
                [Description("Phy. Id 04 (GPS)")]
                Id_0x83 = 0x83, //Physical 04 - GPS
                [Description("Phy. Id 05 (RPMS-02)")]
                Id_0xE4 = 0xE4, //Physical 05 - RPM 
                [Description("Phy. Id 06 (SP2UART Host)")]
                Id_0x45 = 0x45, //Physical 06 - SP2UART (Host)
                [Description("Phy. Id 07 (SP2UART Remote)")]
                Id_0xC6 = 0xC6, //Physical 07 - SP2UART (Remote)
                [Description("Phy. Id 08 (FCS-150A)")]
                Id_0x67 = 0x67, //Physical 08 - FCS 150A
                [Description("Phy. Id 09")]
                Id_0x48 = 0x48, //Physical 09
                [Description("Phy. Id 10")]
                Id_0xE9 = 0xE9, //Physical 10
                [Description("Phy. Id 11")]
                Id_0x6A = 0x6A, //Physical 11
                [Description("Phy. Id 12")]
                Id_0xCB = 0xCB, //Physical 12
                [Description("Phy. Id 13")]
                Id_0xAC = 0xAC, //Physical 13
                [Description("Phy. Id 14")]
                Id_0x0D = 0x0D, //Physical 14
                [Description("Phy. Id 15")]
                Id_0x8E = 0x8E, //Physical 15
                [Description("Phy. Id 16")]
                Id_0x2F = 0x2F, //Physical 16

                [Description("Phy. Id 17")]
                Id_0xD0 = 0xD0, //Physical 17
                [Description("Phy. Id 18")]
                Id_0x71 = 0x71, //Physical 18
                [Description("Phy. Id 19")]
                Id_0xF2 = 0xF2, //Physical 19
                [Description("Phy. Id 20")]
                Id_0x53 = 0x53, //Physical 20
                [Description("Phy. Id 21")]
                Id_0x34 = 0x34, //Physical 21
                [Description("Phy. Id 22")]
                Id_0x95 = 0x95, //Physical 22
                [Description("Phy. Id 23")]
                Id_0x16 = 0x16, //Physical 23
                [Description("Phy. Id 24")]
                Id_0xB7 = 0xB7, //Physical 24
                [Description("Phy. Id 25")]
                Id_0x98 = 0x98, //Physical 25
                [Description("Phy. Id 26")]
                Id_0x39 = 0x39, //Physical 26
                [Description("Phy. Id 27")]
                Id_0xBA = 0xBA, //Physical 27
                [Description("Phy. Id 28")]
                Id_0x1B = 0x1B, //Physical 28
                [Description("Phy. Id 29")]
                Id_0x7E = 0x7E, //Physical 29
            }

            #endregion
        }
        public class SettingsMAVLinkModel : ProtocolSettingsModel
        {

            private UInt16 isValid;
            public SettingsMAVLinkModel()
            {
                isValid = 0xCB00 + (byte)TelemetryProtocol.MAVLink;
            }

            #region Serialization / Deserialization

            public override void DeSerialize(byte[] data, int offset)
            {
                isValid = Converter.ToUInt16(data, offset);
                offset += 2;

            }

            public override void Serialize(byte[] data, int offset)
            {
                Converter.GetBytes(isValid, data, offset);
                offset += 2;

            }

            #endregion
        }
        public class SettingsHoTTModel : ProtocolSettingsModel
        {
            private UInt16 isValid;
            public SettingsHoTTModel()
            {
                isValid = 0xCB00 + (byte)TelemetryProtocol.HoTT;

                GPSEnable = true;
                EAMEnable = true;
                GAMEnable = true;
                VarioEnable = true;
            }

            private bool eamEnable;
            public bool EAMEnable
            {
                get { return eamEnable; }
                set
                {
                    if (eamEnable == value)
                        return;
                    eamEnable = value;
                    RaisePropertyChanged(() => EAMEnable);
                }
            }

            private bool gamEnable;
            public bool GAMEnable
            {
                get { return gamEnable; }
                set
                {
                    if (gamEnable == value)
                        return;
                    gamEnable = value;
                    RaisePropertyChanged(() => GAMEnable);
                }
            }

            private bool varioEnable;
            public bool VarioEnable
            {
                get { return varioEnable; }
                set
                {
                    if (varioEnable == value)
                        return;
                    varioEnable = value;
                    RaisePropertyChanged(() => VarioEnable);
                }
            }

            private bool gpsEnable;
            public bool GPSEnable
            {
                get { return gpsEnable; }
                set
                {
                    if (gpsEnable == value)
                        return;
                    gpsEnable = value;
                    RaisePropertyChanged(() => GPSEnable);
                }
            }

            #region Serialization / Deserialization

            public override void DeSerialize(byte[] data, int offset)
            {
                isValid = Converter.ToUInt16(data, offset);
                offset += 2;

                EAMEnable = data[offset++] == 1;
                GAMEnable = data[offset++] == 1;
                GPSEnable = data[offset++] == 1;
                VarioEnable = data[offset++] == 1;
            }

            public override void Serialize(byte[] data, int offset)
            {
                Converter.GetBytes(isValid, data, offset);
                offset += 2;
                data[offset++] = (byte)(EAMEnable ? 1 : 0);
                data[offset++] = (byte)(GAMEnable ? 1 : 0);
                data[offset++] = (byte)(GPSEnable ? 1 : 0);
                data[offset++] = (byte)(VarioEnable ? 1 : 0);
            }

            #endregion
        }
        public class SettingsJetiModel : ProtocolSettingsModel
        {
            private UInt16 isValid;
            public SettingsJetiModel()
            {
                isValid = 0xCB00 + (byte)TelemetryProtocol.Jeti;

                Value01 = TelemetryValue.Latitude;
                Value02 = TelemetryValue.Longitude;
                Value03 = TelemetryValue.Satellite;
                Value04 = TelemetryValue.GPS_Fix;
                Value05 = TelemetryValue.Speed;
                Value06 = TelemetryValue.Distance;
                Value07 = TelemetryValue.Altitude;
                Value08 = TelemetryValue.VSpeed;
                Value09 = TelemetryValue.Compass;
                Value10 = TelemetryValue.HomeDirection;
                Value11 = TelemetryValue.Voltage;
                Value12 = TelemetryValue.Current;
                Value13 = TelemetryValue.Capacity;
                Value14 = TelemetryValue.LipoVoltage;
                Value15 = TelemetryValue.LipoTemp;
            }

            #region Properties

            private List<EnumExtensions.EnumItem> sensorValueSource;
            public List<EnumExtensions.EnumItem> SensorValueSource
            {
                get
                {
                    if (sensorValueSource == null)
                        sensorValueSource = typeof(TelemetryValue).GetDataSource();

                    return sensorValueSource;
                }
            }

            private TelemetryValue value01;
            public TelemetryValue Value01
            {
                get { return value01; }
                set
                {
                    if (value01 == value)
                        return;
                    value01 = value;
                    RaisePropertyChanged(() => Value01);
                }
            }

            private TelemetryValue value02;
            public TelemetryValue Value02
            {
                get { return value02; }
                set
                {
                    if (value02 == value)
                        return;
                    value02 = value;
                    RaisePropertyChanged(() => Value02);
                }
            }

            private TelemetryValue value03;
            public TelemetryValue Value03
            {
                get { return value03; }
                set
                {
                    if (value03 == value)
                        return;
                    value03 = value;
                    RaisePropertyChanged(() => Value03);
                }
            }

            private TelemetryValue value04;
            public TelemetryValue Value04
            {
                get { return value04; }
                set
                {
                    if (value04 == value)
                        return;
                    value04 = value;
                    RaisePropertyChanged(() => Value04);
                }
            }

            private TelemetryValue value05;
            public TelemetryValue Value05
            {
                get { return value05; }
                set
                {
                    if (value05 == value)
                        return;
                    value05 = value;
                    RaisePropertyChanged(() => Value05);
                }
            }

            private TelemetryValue value06;
            public TelemetryValue Value06
            {
                get { return value06; }
                set
                {
                    if (value06 == value)
                        return;
                    value06 = value;
                    RaisePropertyChanged(() => Value06);
                }
            }

            private TelemetryValue value07;
            public TelemetryValue Value07
            {
                get { return value07; }
                set
                {
                    if (value07 == value)
                        return;
                    value07 = value;
                    RaisePropertyChanged(() => Value07);
                }
            }

            private TelemetryValue value08;
            public TelemetryValue Value08
            {
                get { return value08; }
                set
                {
                    if (value08 == value)
                        return;
                    value08 = value;
                    RaisePropertyChanged(() => Value08);
                }
            }

            private TelemetryValue value09;
            public TelemetryValue Value09
            {
                get { return value09; }
                set
                {
                    if (value09 == value)
                        return;
                    value09 = value;
                    RaisePropertyChanged(() => Value09);
                }
            }

            private TelemetryValue value10;
            public TelemetryValue Value10
            {
                get { return value10; }
                set
                {
                    if (value10 == value)
                        return;
                    value10 = value;
                    RaisePropertyChanged(() => Value10);
                }
            }

            private TelemetryValue value11;
            public TelemetryValue Value11
            {
                get { return value11; }
                set
                {
                    if (value11 == value)
                        return;
                    value11 = value;
                    RaisePropertyChanged(() => Value11);
                }
            }

            private TelemetryValue value12;
            public TelemetryValue Value12
            {
                get { return value12; }
                set
                {
                    if (value12 == value)
                        return;
                    value12 = value;
                    RaisePropertyChanged(() => Value12);
                }
            }

            private TelemetryValue value13;
            public TelemetryValue Value13
            {
                get { return value13; }
                set
                {
                    if (value13 == value)
                        return;
                    value13 = value;
                    RaisePropertyChanged(() => Value13);
                }
            }

            private TelemetryValue value14;
            public TelemetryValue Value14
            {
                get { return value14; }
                set
                {
                    if (value14 == value)
                        return;
                    value14 = value;
                    RaisePropertyChanged(() => Value14);
                }
            }

            private TelemetryValue value15;
            public TelemetryValue Value15
            {
                get { return value15; }
                set
                {
                    if (value15 == value)
                        return;
                    value15 = value;
                    RaisePropertyChanged(() => Value15);
                }
            }

            #endregion

            #region Serialization / Deserialization

            public override void DeSerialize(byte[] data, int offset)
            {
                isValid = Converter.ToUInt16(data, offset);
                offset += 2;

                Value01 = (TelemetryValue)data[offset++];
                Value02 = (TelemetryValue)data[offset++];
                Value03 = (TelemetryValue)data[offset++];
                Value04 = (TelemetryValue)data[offset++];
                Value05 = (TelemetryValue)data[offset++];
                Value06 = (TelemetryValue)data[offset++];
                Value07 = (TelemetryValue)data[offset++];
                Value08 = (TelemetryValue)data[offset++];
                Value09 = (TelemetryValue)data[offset++];
                Value10 = (TelemetryValue)data[offset++];
                Value11 = (TelemetryValue)data[offset++];
                Value12 = (TelemetryValue)data[offset++];
                Value13 = (TelemetryValue)data[offset++];
                Value14 = (TelemetryValue)data[offset++];
                Value15 = (TelemetryValue)data[offset++];

            }

            public override void Serialize(byte[] data, int offset)
            {
                Converter.GetBytes(isValid, data, offset);
                offset += 2;

                data[offset++] = (byte)Value01;
                data[offset++] = (byte)Value02;
                data[offset++] = (byte)Value03;
                data[offset++] = (byte)Value04;
                data[offset++] = (byte)Value05;
                data[offset++] = (byte)Value06;
                data[offset++] = (byte)Value07;
                data[offset++] = (byte)Value08;
                data[offset++] = (byte)Value09;
                data[offset++] = (byte)Value10;
                data[offset++] = (byte)Value11;
                data[offset++] = (byte)Value12;
                data[offset++] = (byte)Value13;
                data[offset++] = (byte)Value14;
                data[offset++] = (byte)Value15;

            }

            #endregion

            #region Nested Types

            public enum TelemetryValue : byte
            {
                [Description("Disabled")]
                None = 0,
                Latitude = 1,
                Longitude = 2,
                [Description("Satellites")]
                Satellite = 3,
                [Description("GPS Fix")]
                GPS_Fix = 4,
                Flightmode = 5,
                Speed = 6,
                Distance = 7,
                Altitude = 8,
                [Description("Vertical Speed (Vario)")]
                VSpeed = 9,
                Compass = 10,
                [Description("Flightdirection (COG)")]
                COG = 11,
                [Description("Home Direction")]
                HomeDirection = 12,
                Voltage = 13,
                Current = 14,
                Capacity = 15,
                [Description("Lipo Voltage (lowest)")]
                LipoVoltage = 16,
                [Description("Lipo Temperature")]
                LipoTemp = 17,
                [Description("Home Latitude")]
                Home_Latitude = 18,
                [Description("Home Longitude")]
                Home_Longitude = 19,
                [Description("RPM")]
                RPM = 20,

            };

            #endregion

        }
        public class SettingsFutabaModel : ProtocolSettingsModel
        {
            private const byte SlotCount = 32;
            private UInt16 isValid;
            private SlotValues[] slots;

            public SettingsFutabaModel()
            {
                isValid = 0xCB00 + (byte)TelemetryProtocol.Futaba;

                slots = new SlotValues[SlotCount];

                slots[1] = SlotValues.Vario;
                slots[3] = SlotValues.Cell;
                slots[5] = SlotValues.Current;
                slots[8] = SlotValues.GPS;

                UpdateValue();
            }

            private byte slotVario;
            public byte SlotVario
            {
                get { return slotVario; }
                set
                {
                    if (slotVario == value)
                        return;

                    slotVario = value;
                    UpdateSlots();
                    RaisePropertyChanged(() => SlotVario);
                }
            }

            private List<SlotType> mapSlotVario;
            public List<SlotType> MapSlotVario
            {
                get { return mapSlotVario; }
                set
                {
                    if (mapSlotVario == value)
                        return;

                    mapSlotVario = value;
                    RaisePropertyChanged(() => MapSlotVario);
                }
            }

            private byte slotCurrent;
            public byte SlotCurrent
            {
                get { return slotCurrent; }
                set
                {
                    if (slotCurrent == value)
                        return;

                    slotCurrent = value;
                    UpdateSlots();
                    RaisePropertyChanged(() => SlotCurrent);
                }
            }

            private List<SlotType> mapSlotCurrent;
            public List<SlotType> MapSlotCurrent
            {
                get { return mapSlotCurrent; }
                set
                {
                    if (mapSlotCurrent == value)
                        return;

                    mapSlotCurrent = value;
                    RaisePropertyChanged(() => MapSlotCurrent);
                }
            }

            private byte slotGPS;
            public byte SlotGPS
            {
                get { return slotGPS; }
                set
                {
                    if (slotGPS == value)
                        return;

                    slotGPS = value;
                    UpdateSlots();
                    RaisePropertyChanged(() => SlotGPS);
                }
            }

            private List<SlotType> mapSlotGPS;
            public List<SlotType> MapSlotGPS
            {
                get { return mapSlotGPS; }
                set
                {
                    if (mapSlotGPS == value)
                        return;

                    mapSlotGPS = value;
                    RaisePropertyChanged(() => MapSlotGPS);
                }
            }

            private byte slotCell;
            public byte SlotCell
            {
                get { return slotCell; }
                set
                {
                    if (slotCell == value)
                        return;

                    slotCell = value;
                    UpdateSlots();
                    RaisePropertyChanged(() => SlotCell);
                }
            }

            private List<SlotType> mapSlotCell;
            public List<SlotType> MapSlotCell
            {
                get { return mapSlotCell; }
                set
                {
                    if (mapSlotCell == value)
                        return;

                    mapSlotCell = value;
                    RaisePropertyChanged(() => MapSlotCell);
                }
            }

            private byte slotCompass;
            public byte SlotCompass
            {
                get { return slotCompass; }
                set
                {
                    if (slotCompass == value)
                        return;

                    slotCompass = value;
                    UpdateSlots();
                    RaisePropertyChanged(() => SlotCompass);
                }
            }

            private List<SlotType> mapSlotCompass;
            public List<SlotType> MapSlotCompass
            {
                get { return mapSlotCompass; }
                set
                {
                    if (mapSlotCompass == value)
                        return;

                    mapSlotCompass = value;
                    RaisePropertyChanged(() => MapSlotCompass);
                }
            }

            private byte slotSats;
            public byte SlotSats
            {
                get { return slotSats; }
                set
                {
                    if (slotSats == value)
                        return;

                    slotSats = value;
                    UpdateSlots();
                    RaisePropertyChanged(() => SlotSats);
                }
            }

            private List<SlotType> mapSlotSats;
            public List<SlotType> MapSlotSats
            {
                get { return mapSlotSats; }
                set
                {
                    if (mapSlotSats == value)
                        return;

                    mapSlotSats = value;
                    RaisePropertyChanged(() => MapSlotSats);
                }
            }

            private byte slotFlightmode;
            public byte SlotFlightmode
            {
                get { return slotFlightmode; }
                set
                {
                    if (slotFlightmode == value)
                        return;

                    slotFlightmode = value;
                    UpdateSlots();
                    RaisePropertyChanged(() => SlotFlightmode);
                }
            }

            private List<SlotType> mapSlotFlightMode;
            public List<SlotType> MapSlotFlightMode
            {
                get { return mapSlotFlightMode; }
                set
                {
                    if (mapSlotFlightMode == value)
                        return;

                    mapSlotFlightMode = value;
                    RaisePropertyChanged(() => MapSlotFlightMode);
                }
            }

            private byte slotTemperatur;
            public byte SlotTemperatur
            {
                get { return slotTemperatur; }
                set
                {
                    if (slotTemperatur == value)
                        return;

                    slotTemperatur = value;
                    UpdateSlots();
                    RaisePropertyChanged(() => SlotTemperatur);
                }
            }

            private List<SlotType> mapSlotTemperatur;
            public List<SlotType> MapSlotTemperatur
            {
                get { return mapSlotTemperatur; }
                set
                {
                    if (mapSlotTemperatur == value)
                        return;

                    mapSlotTemperatur = value;
                    RaisePropertyChanged(() => MapSlotTemperatur);
                }
            }

            void UpdateSlots()
            {
                for (byte i = 0; i < SlotCount; i++)
                    slots[i] = SlotValues.None;

                if (SlotVario != 0)
                    slots[SlotVario] = SlotValues.Vario;

                if (SlotCurrent != 0)
                    slots[SlotCurrent] = SlotValues.Current;

                if (SlotCell != 0)
                    slots[SlotCell] = SlotValues.Cell;

                if (SlotGPS != 0)
                    slots[SlotGPS] = SlotValues.GPS;

                if (SlotSats != 0)
                    slots[SlotSats] = SlotValues.Sats;

                if (SlotFlightmode != 0)
                    slots[SlotFlightmode] = SlotValues.Flightmode;

                if (SlotTemperatur != 0)
                    slots[SlotTemperatur] = SlotValues.Temperatur;

                if (SlotCompass != 0)
                    slots[SlotCompass] = SlotValues.Kompass;

                MapSlotVario = GetSlots(SlotValues.Vario);
                MapSlotCurrent = GetSlots(SlotValues.Current);
                MapSlotCell = GetSlots(SlotValues.Cell);
                MapSlotGPS = GetSlots(SlotValues.GPS);
                MapSlotCompass = GetSlots(SlotValues.Kompass);
                MapSlotFlightMode = GetSlots(SlotValues.Flightmode);
                MapSlotSats = GetSlots(SlotValues.Sats);
                MapSlotTemperatur = GetSlots(SlotValues.Temperatur);
            }

            List<SlotType> GetSlots(SlotValues slotValue)
            {
                List<SlotType> types = SlotType.GetSlots();
                List<SlotType> remaining = new List<SlotType>();
                List<SlotType> tmp = new List<SlotType>();

                byte size = GetSlotSize(slotValue);

                remaining.Add(types[0]);

                for (byte i = 1; i < SlotCount; )
                {
                    if (i % 8 == 0)
                        tmp.Clear();

                    if (slots[i] == SlotValues.None || slots[i] == slotValue)
                        tmp.Add(types[i]);
                    else
                        tmp.Clear();

                    if (tmp.Count >= size)
                    {
                        remaining.Add(tmp[0]);
                        tmp.RemoveAt(0);
                    }

                    i += GetSlotSize(slots[i] == slotValue ? SlotValues.None : slots[i]);
                }

                return remaining;
            }

            void UpdateValue()
            {
                for (byte i = 0; i < SlotCount; i++)
                {
                    switch (slots[i])
                    {
                        case SlotValues.Vario:
                            slotVario = i;
                            break;
                        case SlotValues.Kompass:
                            slotCompass = i;
                            break;
                        case SlotValues.Current:
                            slotCurrent = i;
                            break;
                        case SlotValues.GPS:
                            slotGPS = i;
                            break;
                        case SlotValues.Cell:
                            slotCell = i;
                            break;
                        case SlotValues.Sats:
                            slotSats = i;
                            break;
                        case SlotValues.Flightmode:
                            slotFlightmode = i;
                            break;
                        case SlotValues.Temperatur:
                            slotTemperatur = i;
                            break;
                    }
                }

                UpdateSlots();
            }

            byte GetSlotSize(SlotValues value)
            {
                switch (value)
                {
                    case SlotValues.Vario:
                        return 2;
                    case SlotValues.Current:
                        return 3;
                    case SlotValues.GPS:
                        return 8;
                    case SlotValues.Cell:
                        return 2;
                    default:
                        return 1;
                }
            }

            #region Serialize/Deserialize

            public override void DeSerialize(byte[] data, int offset)
            {
                isValid = Converter.ToUInt16(data, offset);
                offset += 2;

                slotVario = 0;
                slotCompass = 0;
                slotCurrent = 0;
                slotGPS = 0;
                slotCell = 0;
                slotSats = 0;
                slotFlightmode = 0;
                slotTemperatur = 0;

                for (int i = 0; i < SlotCount; i++)
                    slots[i] = (SlotValues)data[offset++];

                UpdateValue();
            }

            public override void Serialize(byte[] data, int offset)
            {
                Converter.GetBytes(isValid, data, offset);
                offset += 2;

                for (int i = 0; i < SlotCount; i++)
                    data[offset++] = (byte)slots[i];
            }

            #endregion

            #region Nested Types

            public class SlotType
            {
                public byte Number { get; set; }
                public string Name { get; set; }

                private static List<SlotType> slots;
                public static List<SlotType> GetSlots()
                {
                    if (slots != null)
                        return slots;

                    slots = new List<SlotType>();
                    slots.Add(new SlotType() { Number = 0, Name = "Disabled" });
                    for (byte i = 1; i < SlotCount; i++)
                        slots.Add(new SlotType() { Number = i, Name = string.Format("Slot {0:00}", i) });

                    return slots;
                }
            }

            public enum SlotValues
            {
                None = 0,
                Vario = 1,
                Kompass = 2,
                Current = 3,
                GPS = 4,
                Cell = 5,
                Sats = 6,
                Flightmode = 7,
                Temperatur = 8,
            }

            #endregion

        }
        public class SettingsSpektrumModel : ProtocolSettingsModel
        {

            private UInt16 isValid;
            public SettingsSpektrumModel()
            {
                isValid = 0xCB00 + (byte)TelemetryProtocol.Spektrum;

                SensorCurrent = true;
                SensorSpeed = true;
                SensorAltitude = true;
                SensorGPS = true;
                SensorDualEnergy = true;
                SensorBatteryGauge = true;
                SensorLipo = true;
                SensorVario = true;
                SensorPowerbox = true;
                SensorVoltage = true;
            }

            #region Properties

            private bool sensorCurrent;
            public bool SensorCurrent
            {
                get { return sensorCurrent; }
                set
                {
                    if (sensorCurrent == value)
                        return;

                    sensorCurrent = value;
                    RaisePropertyChanged(() => SensorCurrent);
                }
            }

            private bool sensorSpeed;
            public bool SensorSpeed
            {
                get { return sensorSpeed; }
                set
                {
                    if (sensorSpeed == value)
                        return;

                    sensorSpeed = value;
                    RaisePropertyChanged(() => SensorSpeed);
                }
            }

            private bool sensorAltitude;
            public bool SensorAltitude
            {
                get { return sensorAltitude; }
                set
                {
                    if (sensorAltitude == value)
                        return;

                    sensorAltitude = value;
                    RaisePropertyChanged(() => SensorAltitude);
                }
            }

            private bool sensorGPS;
            public bool SensorGPS
            {
                get { return sensorGPS; }
                set
                {
                    if (sensorGPS == value)
                        return;

                    sensorGPS = value;
                    RaisePropertyChanged(() => SensorGPS);
                }
            }

            private bool sensorDualEnergy;
            public bool SensorDualEnergy
            {
                get { return sensorDualEnergy; }
                set
                {
                    if (sensorDualEnergy == value)
                        return;

                    sensorDualEnergy = value;
                    RaisePropertyChanged(() => SensorDualEnergy);
                }
            }

            private bool sensorBatteryGauge;
            public bool SensorBatteryGauge
            {
                get { return sensorBatteryGauge; }
                set
                {
                    if (sensorBatteryGauge == value)
                        return;

                    sensorBatteryGauge = value;
                    RaisePropertyChanged(() => SensorBatteryGauge);
                }
            }

            private bool sensorLipo;
            public bool SensorLipo
            {
                get { return sensorLipo; }
                set
                {
                    if (sensorLipo == value)
                        return;

                    sensorLipo = value;
                    RaisePropertyChanged(() => SensorLipo);
                }
            }

            private bool sensorVario;
            public bool SensorVario
            {
                get { return sensorVario; }
                set
                {
                    if (sensorVario == value)
                        return;

                    sensorVario = value;
                    RaisePropertyChanged(() => SensorVario);
                }
            }

            private bool sensorPowerbox;
            public bool SensorPowerbox
            {
                get { return sensorPowerbox; }
                set
                {
                    if (sensorPowerbox == value)
                        return;

                    sensorPowerbox = value;
                    RaisePropertyChanged(() => SensorPowerbox);
                }
            }

            private bool sensorVoltage;
            public bool SensorVoltage
            {
                get { return sensorVoltage; }
                set
                {
                    if (sensorVoltage == value)
                        return;

                    sensorVoltage = value;
                    RaisePropertyChanged(() => SensorVoltage);
                }
            }

            #endregion

            #region Serialization / Deserialization

            public override void DeSerialize(byte[] data, int offset)
            {
                isValid = Converter.ToUInt16(data, offset);
                offset += 2;

                SensorCurrent = data[offset++] == 1;
                SensorSpeed = data[offset++] == 1;
                SensorAltitude = data[offset++] == 1;
                SensorGPS = data[offset++] == 1;
                SensorDualEnergy = data[offset++] == 1;
                SensorBatteryGauge = data[offset++] == 1;
                SensorLipo = data[offset++] == 1;
                SensorVario = data[offset++] == 1;
                SensorPowerbox = data[offset++] == 1;
                SensorVoltage = data[offset++] == 1;
            }

            public override void Serialize(byte[] data, int offset)
            {
                Converter.GetBytes(isValid, data, offset);
                offset += 2;

                data[offset++] = (byte)(SensorCurrent ? 1 : 0);
                data[offset++] = (byte)(SensorSpeed ? 1 : 0);
                data[offset++] = (byte)(SensorAltitude ? 1 : 0);
                data[offset++] = (byte)(SensorGPS ? 1 : 0);
                data[offset++] = (byte)(SensorDualEnergy ? 1 : 0);
                data[offset++] = (byte)(SensorBatteryGauge ? 1 : 0);
                data[offset++] = (byte)(SensorLipo ? 1 : 0);
                data[offset++] = (byte)(SensorVario ? 1 : 0);
                data[offset++] = (byte)(SensorPowerbox ? 1 : 0);
                data[offset++] = (byte)(SensorVoltage ? 1 : 0);
            }

            #endregion
        }
        public class SettingsMultiplexModel : ProtocolSettingsModel
        {
            private UInt16 isValid;
            public SettingsMultiplexModel()
            {
                isValid = 0xCB00 + (byte)TelemetryProtocol.Multiplex;

                Address00 = SensorValueMapping.None;
                Address01 = SensorValueMapping.None;
                Address02 = SensorValueMapping.Distance;
                Address03 = SensorValueMapping.Altitude;
                Address04 = SensorValueMapping.Speed;
                Address05 = SensorValueMapping.VSpeed;
                Address06 = SensorValueMapping.Compass;
                Address07 = SensorValueMapping.COG;
                Address08 = SensorValueMapping.HomeDirection;
                Address09 = SensorValueMapping.Flightmode;
                Address10 = SensorValueMapping.Satellite;
                Address11 = SensorValueMapping.GPS_Fix;
                Address12 = SensorValueMapping.Voltage;
                Address13 = SensorValueMapping.LipoVoltage;
                Address14 = SensorValueMapping.Current;
                Address15 = SensorValueMapping.Capacity;
            }

            #region Properties

            private List<EnumExtensions.EnumItem> sensorValueSource;
            public List<EnumExtensions.EnumItem> SensorValueSource
            {
                get
                {
                    if (sensorValueSource == null)
                        sensorValueSource = typeof(SensorValueMapping).GetDataSource();

                    return sensorValueSource;
                }
            }

            private SensorValueMapping address00;
            public SensorValueMapping Address00
            {
                get { return address00; }
                set
                {
                    if (address00 == value)
                        return;
                    address00 = value;
                    RaisePropertyChanged(() => Address00);
                }
            }

            private SensorValueMapping address01;
            public SensorValueMapping Address01
            {
                get { return address01; }
                set
                {
                    if (address01 == value)
                        return;
                    address01 = value;
                    RaisePropertyChanged(() => Address01);
                }
            }

            private SensorValueMapping address02;
            public SensorValueMapping Address02
            {
                get { return address02; }
                set
                {
                    if (address02 == value)
                        return;
                    address02 = value;
                    RaisePropertyChanged(() => Address02);
                }
            }

            private SensorValueMapping address03;
            public SensorValueMapping Address03
            {
                get { return address03; }
                set
                {
                    if (address03 == value)
                        return;
                    address03 = value;
                    RaisePropertyChanged(() => Address03);
                }
            }

            private SensorValueMapping address04;
            public SensorValueMapping Address04
            {
                get { return address04; }
                set
                {
                    if (address04 == value)
                        return;
                    address04 = value;
                    RaisePropertyChanged(() => Address04);
                }
            }

            private SensorValueMapping address05;
            public SensorValueMapping Address05
            {
                get { return address05; }
                set
                {
                    if (address05 == value)
                        return;
                    address05 = value;
                    RaisePropertyChanged(() => Address05);
                }
            }

            private SensorValueMapping address06;
            public SensorValueMapping Address06
            {
                get { return address06; }
                set
                {
                    if (address06 == value)
                        return;
                    address06 = value;
                    RaisePropertyChanged(() => Address06);
                }
            }

            private SensorValueMapping address07;
            public SensorValueMapping Address07
            {
                get { return address07; }
                set
                {
                    if (address07 == value)
                        return;
                    address07 = value;
                    RaisePropertyChanged(() => Address07);
                }
            }

            private SensorValueMapping address08;
            public SensorValueMapping Address08
            {
                get { return address08; }
                set
                {
                    if (address08 == value)
                        return;
                    address08 = value;
                    RaisePropertyChanged(() => Address08);
                }
            }

            private SensorValueMapping address09;
            public SensorValueMapping Address09
            {
                get { return address09; }
                set
                {
                    if (address09 == value)
                        return;
                    address09 = value;
                    RaisePropertyChanged(() => Address09);
                }
            }

            private SensorValueMapping address10;
            public SensorValueMapping Address10
            {
                get { return address10; }
                set
                {
                    if (address10 == value)
                        return;
                    address10 = value;
                    RaisePropertyChanged(() => Address10);
                }
            }

            private SensorValueMapping address11;
            public SensorValueMapping Address11
            {
                get { return address11; }
                set
                {
                    if (address11 == value)
                        return;
                    address11 = value;
                    RaisePropertyChanged(() => Address11);
                }
            }

            private SensorValueMapping address12;
            public SensorValueMapping Address12
            {
                get { return address12; }
                set
                {
                    if (address12 == value)
                        return;
                    address12 = value;
                    RaisePropertyChanged(() => Address12);
                }
            }

            private SensorValueMapping address13;
            public SensorValueMapping Address13
            {
                get { return address13; }
                set
                {
                    if (address13 == value)
                        return;
                    address13 = value;
                    RaisePropertyChanged(() => Address13);
                }
            }

            private SensorValueMapping address14;
            public SensorValueMapping Address14
            {
                get { return address14; }
                set
                {
                    if (address14 == value)
                        return;
                    address14 = value;
                    RaisePropertyChanged(() => Address14);
                }
            }

            private SensorValueMapping address15;
            public SensorValueMapping Address15
            {
                get { return address15; }
                set
                {
                    if (address15 == value)
                        return;
                    address15 = value;
                    RaisePropertyChanged(() => Address15);
                }
            }

            #endregion

            #region Serialization / Deserialization

            public override void DeSerialize(byte[] data, int offset)
            {
                isValid = Converter.ToUInt16(data, offset);
                offset += 2;

                Address00 = (SensorValueMapping)data[offset++];
                Address01 = (SensorValueMapping)data[offset++];
                Address02 = (SensorValueMapping)data[offset++];
                Address03 = (SensorValueMapping)data[offset++];
                Address04 = (SensorValueMapping)data[offset++];
                Address05 = (SensorValueMapping)data[offset++];
                Address06 = (SensorValueMapping)data[offset++];
                Address07 = (SensorValueMapping)data[offset++];
                Address08 = (SensorValueMapping)data[offset++];
                Address09 = (SensorValueMapping)data[offset++];
                Address10 = (SensorValueMapping)data[offset++];
                Address11 = (SensorValueMapping)data[offset++];
                Address12 = (SensorValueMapping)data[offset++];
                Address13 = (SensorValueMapping)data[offset++];
                Address14 = (SensorValueMapping)data[offset++];
                Address15 = (SensorValueMapping)data[offset++];
            }

            public override void Serialize(byte[] data, int offset)
            {
                Converter.GetBytes(isValid, data, offset);
                offset += 2;
                data[offset++] = (byte)Address00;
                data[offset++] = (byte)Address01;
                data[offset++] = (byte)Address02;
                data[offset++] = (byte)Address03;
                data[offset++] = (byte)Address04;
                data[offset++] = (byte)Address05;
                data[offset++] = (byte)Address06;
                data[offset++] = (byte)Address07;
                data[offset++] = (byte)Address08;
                data[offset++] = (byte)Address09;
                data[offset++] = (byte)Address10;
                data[offset++] = (byte)Address11;
                data[offset++] = (byte)Address12;
                data[offset++] = (byte)Address13;
                data[offset++] = (byte)Address14;
                data[offset++] = (byte)Address15;
            }

            #endregion

            #region Nested Types

            public enum SensorValueMapping : byte
            {
                [Description("Disabled")]
                None = 0,
                [Description("Satellites")]
                Satellite = 1,
                [Description("GPS Fix")]
                GPS_Fix = 2,
                Flightmode = 3,
                Speed = 4,
                Distance = 5,
                Altitude = 6,
                [Description("Vertical Speed (Vario)")]
                VSpeed = 7,
                Compass = 8,
                [Description("Flightdirection (COG)")]
                COG = 9,
                [Description("Home Direction")]
                HomeDirection = 10,
                Voltage = 11,
                Current = 12,
                Capacity = 13,
                [Description("Lipo Voltage (lowest)")]
                LipoVoltage = 14,
                [Description("Temperature 1")]
                Temperatur1 = 15,
                [Description("Temperature 2")]
                Temperatur2 = 16,
                Charge = 17,
                RPM = 18,
            }

            #endregion
        }

        #endregion
    }
}
