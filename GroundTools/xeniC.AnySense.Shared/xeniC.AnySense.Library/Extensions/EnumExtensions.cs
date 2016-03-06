using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace xeniC.AnySense.Library.Extensions
{
    /// <span class="code-SummaryComment"><summary></span>
    /// Provides a static utility object of methods and properties to interact
    /// with enumerated types.
    /// <span class="code-SummaryComment"></summary></span>
    public static class EnumExtensions
    {
        /// <span class="code-SummaryComment"><summary></span>
        /// Gets the <span class="code-SummaryComment"><see cref="DescriptionAttribute" /> of an <see cref="Enum" /> </span>
        /// type value.
        /// <span class="code-SummaryComment"></summary></span>
        /// <span class="code-SummaryComment"><param name="value">The <see cref="Enum" /> type value.</param></span>
        /// <span class="code-SummaryComment"><returns>A string containing the text of the</span>
        /// <span class="code-SummaryComment"><see cref="DescriptionAttribute"/>.</returns></span>
        public static string GetDescription(this Enum value)
        {
            if (value == null)
                throw new ArgumentNullException("value");

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

        /// <span class="code-SummaryComment"><summary></span>
        /// Converts the <span class="code-SummaryComment"><see cref="Enum" /> type to an <see cref="IList" /> </span>
        /// compatible object.
        /// <span class="code-SummaryComment"></summary></span>
        /// <span class="code-SummaryComment"><param name="type">The <see cref="Enum"/> type.</param></span>
        /// <span class="code-SummaryComment"><returns>An <see cref="IList"/> containing the enumerated</span>
        /// type value and description.<span class="code-SummaryComment"></returns></span>
        public static List<EnumItem> GetDataSource(this Type type)
        {
            if (type == null)
                throw new ArgumentNullException("type");

            List<EnumItem> list = new List<EnumItem>();
            Array enumValues = Enum.GetValues(type);

            foreach (Enum value in enumValues)
                list.Add(new EnumItem(value, value.GetDescription()));

            return list;
        }

        public class EnumItem
        {
            private Enum value;
            private string description;

            public EnumItem(Enum value, string description)
            {
                this.value = value;
                this.description = description;
            }

            public Enum Value { get { return value; } }
            public string Description { get { return description; } }

            public override string ToString()
            {
                return Description;
            }
        }
    }
}
