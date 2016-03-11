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
                int offset = 1;
                msg = new Msg_configuration_data();
                msg.data = new byte[240];
                msg.data[offset++] = (byte)Protocol;

                offset = 32;
                if (Settings != null)
                    Settings.Serialize(msg.data, offset);
            }

            private void Deserialize(Msg_configuration_data msg)
            {
                int offset = 1;

                Protocol = (TelemetryProtocol)msg.data[offset++];
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


            public override void DeSerialize(byte[] data, int offset)
            {
            }

            public override void Serialize(byte[] data, int offset)
            {
            }
        }
        public class SettingsHoTTModel : ProtocolSettingsModel
        {
            private UInt16 isValid;
            public SettingsHoTTModel()
            {
                isValid = 0xCB00 + (byte)TelemetryProtocol.FrSky;

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
            public override void DeSerialize(byte[] data, int offset)
            {
            }

            public override void Serialize(byte[] data, int offset)
            {
            }
        }
        public class SettingsFutabaModel : ProtocolSettingsModel
        {
            public override void DeSerialize(byte[] data, int offset)
            {
            }

            public override void Serialize(byte[] data, int offset)
            {
            }
        }
        public class SettingsSpektrumModel : ProtocolSettingsModel
        {
            public override void DeSerialize(byte[] data, int offset)
            {
            }

            public override void Serialize(byte[] data, int offset)
            {
            }
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
