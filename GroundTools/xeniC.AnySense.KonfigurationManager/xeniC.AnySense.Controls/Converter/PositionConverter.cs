using Microsoft.Maps.MapControl.WPF;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using xeniC.AnySense.Library.Devices;

namespace xeniC.AnySense.Controls.Converter
{
    public class PositionConverter : IValueConverter
    {

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            Location location = null;

            if (value is DeviceModelBase.PositionData)
            {
                DeviceModelBase.PositionData pos = (DeviceModelBase.PositionData)value;
                location = new Location(pos.Latitude, pos.Longitude);
            }

            return location;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
