using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;

namespace xeniC.AnySense.Controls.Converter
{
    public class VisibilityConverter : IValueConverter
    {
        public VisibilityConverter()
        {
            TrueValue = Visibility.Visible;
            FalseValue = Visibility.Hidden;
        }

        public Visibility TrueValue { get; set; }
        public Visibility FalseValue { get; set; }

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value is bool)
                return ((bool)value) ? TrueValue : FalseValue;

            return Visibility.Visible;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
