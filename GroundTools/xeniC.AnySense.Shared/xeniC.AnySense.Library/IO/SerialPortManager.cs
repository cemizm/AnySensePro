using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Management;
using System.Text;
using System.Threading.Tasks;

namespace xeniC.AnySense.Library.IO
{
    public class SerialPortManager : IDisposable
    {
        private ManagementEventWatcher eventWatcher;

        public SerialPortManager()
        {
            Ports = new List<SerialPortInfo>(GetPortInformations());
            eventWatcher = new ManagementEventWatcher(new WqlEventQuery("SELECT * FROM Win32_DeviceChangeEvent WHERE EventType = 2 or EventType = 3"));

            eventWatcher.EventArrived += (o, args) =>
            {
                try
                {
                    UpdateDeviceList();
                }
                catch { }
            };

            eventWatcher.Start();
        }

        private void UpdateDeviceList()
        {
            List<SerialPortInfo> ports = GetPortInformations();

            List<SerialPortInfo> added = (from p in ports
                                          where !Ports.Any(m => m.Device == p.Device)
                                          select p).ToList();

            List<SerialPortInfo> deleted = (from p in Ports
                                            where !ports.Any(m => m.Device == p.Device)
                                            select p).ToList();

            added.ForEach(m => AddPort(m));
            deleted.ForEach(m => RemovePort(m));
        }

        private List<SerialPortInfo> GetPortInformations()
        {
            string[] portnames = SerialPort.GetPortNames();

            try
            {
                using (var searcher = new ManagementObjectSearcher("SELECT * FROM Win32_PnPEntity WHERE Name LIKE '%(COM[0-9]%'"))
                using (var erg = searcher.Get())
                {
                    var ports = erg.Cast<ManagementBaseObject>().Select((p) => p.GetPropertyValue("name") != null ? p.GetPropertyValue("name").ToString() : "").ToList();
                    
                    ports.ForEach((p) =>
                    {
                        System.Diagnostics.Debug.WriteLine(string.Format("Device: {0}", p));
                    });

                    var tList = (from n in portnames
                                 from p in ports
                                 where p != null && p.ToLower().Contains("(" + n.ToLower() + ")")
                                 select new SerialPortInfo { Device = n, Description = (p != null ? p : n) }).Distinct().ToList();


                    return tList;
                }
            }
            catch { }

            return (from n in portnames
                    select new SerialPortInfo { Device = n, Description = n }).ToList();
        }

        public void Dispose()
        {
            if (eventWatcher != null)
                eventWatcher.Dispose();
        }

        #region Ports

        public List<SerialPortInfo> Ports { get; private set; }

        private void AddPort(SerialPortInfo m)
        {
            Ports.Add(m);
            OnDeviceConnected(new SerialPortEventArgs(m));
        }

        private void RemovePort(SerialPortInfo m)
        {
            Ports.Remove(m);
            OnDeviceDisconnected(new SerialPortEventArgs(m));
        }

        #endregion

        #region Events

        public event EventHandler<SerialPortEventArgs> DeviceConnected;

        public event EventHandler<SerialPortEventArgs> DeviceDisconnected;

        protected void OnDeviceConnected(SerialPortEventArgs e)
        {
            if (DeviceConnected != null)
                DeviceConnected(this, e);
        }

        protected void OnDeviceDisconnected(SerialPortEventArgs e)
        {
            if (DeviceDisconnected != null)
                DeviceDisconnected(this, e);
        }

        #endregion

        #region Nested Classes

        public class SerialPortEventArgs : EventArgs
        {
            public SerialPortEventArgs(SerialPortInfo e)
            {
                SerialPort = e;
            }

            public SerialPortInfo SerialPort { get; private set; }
        }

        public class SerialPortInfo
        {
            public string Device { get; set; }

            public string Description { get; set; }
            /*
            public override int GetHashCode()
            {
                return Device.GetHashCode();
            }

            public override bool Equals(object obj)
            {
                return GetHashCode() == obj.GetHashCode();
            }

            public bool Equals(SerialPortInfo x, SerialPortInfo y)
            {
                return x.Equals(y);
            }

            public int GetHashCode(SerialPortInfo obj)
            {
                return obj.GetHashCode();
            }*/
        }

        #endregion

    }
}
