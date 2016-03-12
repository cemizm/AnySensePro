using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace xeniC.AnySense.Controls
{
    /// <summary>
    /// Interaktionslogik für Compass.xaml
    /// </summary>
    public partial class Compass : UserControl
    {
        // Using a DependencyProperty as the backing store for Heading.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty HeadingProperty =
            DependencyProperty.Register("Heading", typeof(double), typeof(Compass), new PropertyMetadata(0d));


        public Compass()
        {
            InitializeComponent();
        }

        public double Heading
        {
            get { return (double)GetValue(HeadingProperty); }
            set { SetValue(HeadingProperty, value); }
        }

    }
}
