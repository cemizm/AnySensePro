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
    /// Interaktionslogik für AvionicGauge.xaml
    /// </summary>
    public partial class AvionicGauge : UserControl
    {
        // Using a DependencyProperty as the backing store for Pitch.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty PitchProperty =
            DependencyProperty.Register("Pitch", typeof(double), typeof(AvionicGauge), new PropertyMetadata(0d));

        // Using a DependencyProperty as the backing store for Roll.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty RollProperty =
            DependencyProperty.Register("Roll", typeof(double), typeof(AvionicGauge), new PropertyMetadata(0d));

        // Using a DependencyProperty as the backing store for Altitude.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty AltitudeProperty =
            DependencyProperty.Register("Altitude", typeof(float), typeof(AvionicGauge), new PropertyMetadata(0f));

        

        public AvionicGauge()
        {
            InitializeComponent();
        }

        public float Altitude
        {
            get { return (float)GetValue(AltitudeProperty); }
            set { SetValue(AltitudeProperty, value); }
        }

        public double Pitch
        {
            get { return (double)GetValue(PitchProperty); }
            set { SetValue(PitchProperty, value); }
        }

        public double Roll
        {
            get { return (double)GetValue(RollProperty); }
            set { SetValue(RollProperty, value); }
        }

    }
}
