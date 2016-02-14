using GalaSoft.MvvmLight;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using xeniC.AnySense.Library.Devices;
using xeniC.AnySense.Library.IO;

namespace xeniC.AnySense.Library
{
    public class MainViewModel : ViewModelBase
    {
        private DeviceManager deviceManager;

        private DeviceModelBase device;

        public MainViewModel()
        {
            deviceManager = new DeviceManager();
            deviceManager.DeviceConnected += (o, e) => { if(Device == null) Device = e.Device; };
            deviceManager.DeviceDisconnected += (o, e) => { if(Device == e.Device) Device = null; };
            deviceManager.StartPolling();
        }

        #region Properties

        public DeviceModelBase Device
        {
            get
            {
                return device;
            }
            private set
            {
                if (device != value)
                {
                    device = value;
                    RaisePropertyChanged(() => Device);
                    RaisePropertyChanged(() => Connected);
                }
            }
        }

        public bool Connected { get { return Device != null; } }

        #endregion
    }
}
