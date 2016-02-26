using MavLink;
using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using xeniC.AnySense.Library.IO;

namespace xeniC.AnySense.Library.Devices
{
    public class DeviceManager : IDisposable
    {
        private bool isPolling;
        private SerialPortManager portManager;

        private Dictionary<SerialPortManager.SerialPortInfo, CancellationTokenSource> tasks;

        private Dictionary<SerialPortManager.SerialPortInfo, DeviceModelBase> connected;

        public DeviceManager()
        {
            isPolling = false;

            tasks = new Dictionary<SerialPortManager.SerialPortInfo, CancellationTokenSource>();
            connected = new Dictionary<SerialPortManager.SerialPortInfo, DeviceModelBase>();

            portManager = new SerialPortManager();
            portManager.DeviceConnected += (o, e) => { StartPollDevice(e.SerialPort); };
            portManager.DeviceDisconnected += (o, e) => { StopPollDevice(e.SerialPort); };

        }

        public void StartPolling()
        {
            isPolling = true;
            portManager.Ports.ForEach(m => StartPollDevice(m));
        }
        public void StopPolling()
        {
            isPolling = false;
            tasks.Keys.ToList().ForEach(m => StopPollDevice(m));
        }

        private async void StartPollDevice(SerialPortManager.SerialPortInfo port)
        {
            if (!isPolling)
                return;

            if (tasks.ContainsKey(port))
                return;

            tasks.Add(port, new CancellationTokenSource());

            try
            {
                DeviceModelBase device = await Connect(port, tasks[port].Token);
                if (device != null)
                    AddDevice(port, device);
            }
            catch { }

            tasks.Remove(port);
        }
        private void StopPollDevice(SerialPortManager.SerialPortInfo port)
        {
            if (connected.ContainsKey(port))
                RemoveDevice(port);

            if (!tasks.ContainsKey(port))
                return;

            CancellationTokenSource t = tasks[port];

            t.Cancel();
        }

        private async Task<DeviceModelBase> Connect(SerialPortManager.SerialPortInfo port, CancellationToken ct)
        {
            DeviceModelBase device = null;
            SerialMavlinkLayer mavlink = new SerialMavlinkLayer(port.Device);

            Msg_configuration_control configCMD = new Msg_configuration_control();
            configCMD.command = (byte)CONFIG_COMMAND.CONFIG_COMMAND_GET_VERSION;
            Msg_data_transmission_handshake handshake = new Msg_data_transmission_handshake();
            
            PacketReceivedEventHandler pr = (o, e) =>
            {
                if (e.Message.GetType() == typeof(Msg_configuration_version2))
                {
                    Msg_configuration_version2 msg = e.Message as Msg_configuration_version2;
                    device = new AnySense(mavlink, msg.fw_version);
                }
                else if (e.Message.GetType() == typeof(Msg_configuration_version3))
                {
                    Msg_configuration_version3 msg = e.Message as Msg_configuration_version3;

                    switch (msg.hw_version)
                    {
                        case 1:
                            device = new AnySensePro(mavlink, msg.fw_version);
                            break;
                        default:
                            break;
                    }
                }
            };

            mavlink.PacketReceived += pr;

            do
            {
                ct.ThrowIfCancellationRequested();
                mavlink.SendMessage(configCMD);
                await Task.Delay(500, ct);
            } while (device == null);

            mavlink.PacketReceived -= pr;

            return device;
        }


        private void AddDevice(SerialPortManager.SerialPortInfo port, DeviceModelBase device)
        {
            if (connected.ContainsKey(port))
                return;

            connected.Add(port, device);

            OnDeviceConnected(new DeviceEventArgs(device));
        }

        private void RemoveDevice(SerialPortManager.SerialPortInfo port)
        {
            if (!connected.ContainsKey(port))
                return;

            OnDeviceDisconnected(new DeviceEventArgs(connected[port]));
            connected[port].Dispose();
            connected.Remove(port);
        }

        public void Dispose()
        {
            StopPolling();

            if (portManager != null)
                portManager.Dispose();
        }

        #region Events

        public event EventHandler<DeviceEventArgs> DeviceConnected;

        public event EventHandler<DeviceEventArgs> DeviceDisconnected;

        protected void OnDeviceConnected(DeviceEventArgs e)
        {
            if (DeviceConnected != null)
                DeviceConnected(this, e);
        }

        protected void OnDeviceDisconnected(DeviceEventArgs e)
        {
            if (DeviceDisconnected != null)
                DeviceDisconnected(this, e);
        }

        #endregion

        #region Nested classes

        public class DeviceEventArgs : EventArgs
        {
            public DeviceEventArgs(DeviceModelBase device)
            {
                Device = device;
            }

            public DeviceModelBase Device { get; private set; }
        }

        #endregion
    }
}
