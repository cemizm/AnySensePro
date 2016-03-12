using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace xeniC.AnySense.Controls.Converter
{
    class AltitudeConverter : IValueConverter
    {

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            double ret;
            if (value != null && double.TryParse(value.ToString(), out ret))
                return ((int)(ret * 10)) % 100 * 4;

            return value;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();

        }
    }
}
