using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace xeniC.AnySense.Controls.Converter
{
    public class EnumConverter: IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value is Enum)
            {
                string description = value.ToString();
                FieldInfo fieldInfo = value.GetType().GetField(description);
                if (fieldInfo == null)
                    return description;

                DescriptionAttribute[] attributes =
                   (DescriptionAttribute[])
                 fieldInfo.GetCustomAttributes(typeof(DescriptionAttribute), false);

                if (attributes != null && attributes.Length > 0)
                    description = attributes[0].Description;

                return description;
            }

            return value;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
