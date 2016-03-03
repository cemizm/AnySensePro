using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace xeniC.AnySense.Controls.Converter
{
    public class PercentageConverter : IValueConverter
    {

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            float val = 0;
            float max = 100;
            float erg = 0;

            try
            {
                if (IsNumber(value))
                    val = float.Parse(value.ToString());

                if (value is Enum)
                    val = (int)value;

                if (IsNumber(parameter))
                    max = float.Parse(parameter.ToString());

                if (parameter is Enum)
                    max = (int)parameter;
            }
            catch { }

            bool invert = max < 0;

            if (max < 0)
                max *= -1;

            erg = (val / max) * 100f;

            if (erg > 100)
                erg = 100;

            if (invert)
                erg = 100 - erg;

            if (erg < 0)
                erg *= -1;

            return erg;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        private bool IsNumber(object value)
        {
            return value is sbyte
                    || value is byte
                    || value is short
                    || value is ushort
                    || value is int
                    || value is uint
                    || value is long
                    || value is ulong
                    || value is float
                    || value is double
                    || value is decimal;
        }
    }
}
