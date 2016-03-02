using Microsoft.Maps.MapControl.WPF;
using Microsoft.Maps.MapControl.WPF.Core;
using Microsoft.Maps.MapControl.WPF.Design;
using System;
using System.Collections.Generic;
using System.ComponentModel;
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
    /// Interaktionslogik für BindableMap.xaml
    /// </summary>
    public partial class BindableMap : UserControl
    {
        public BindableMap()
        {
            InitializeComponent();
        }

        public double ZoomLevel
        {
            get { return (double)GetValue(ZoomLevelProperty); }
            set { SetValue(ZoomLevelProperty, value); }
        }

        // Using a DependencyProperty as the backing store for ZoomLevel.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty ZoomLevelProperty =
            DependencyProperty.Register("ZoomLevel", typeof(double), typeof(BindableMap), new PropertyMetadata(1d, OnZoomLevelPropertyChanged));

        private static void OnZoomLevelPropertyChanged(DependencyObject sender, DependencyPropertyChangedEventArgs eventArgs)
        {
            ((BindableMap)sender).OnZoomLevelPropertyChanged((double)eventArgs.OldValue, (double)eventArgs.NewValue);
        }

        public Location Center
        {
            get { return (Location)GetValue(CenterProperty); }
            set { SetValue(CenterProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Center.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty CenterProperty =
            DependencyProperty.Register("Center", typeof(Location), typeof(BindableMap), new PropertyMetadata(null, OnCenterPropertyChanged));

        private static void OnCenterPropertyChanged(DependencyObject sender, DependencyPropertyChangedEventArgs eventArgs)
        {
            ((BindableMap)sender).OnCenterPropertyChanged((Location)eventArgs.OldValue, (Location)eventArgs.NewValue);
        }


        public double Heading
        {
            get { return (double)GetValue(HeadingProperty); }
            set { SetValue(HeadingProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Heading.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty HeadingProperty =
            DependencyProperty.Register("Heading", typeof(double), typeof(BindableMap), new PropertyMetadata(0D, OnHeadingPropertyChanged));

        private static void OnHeadingPropertyChanged(DependencyObject sender, DependencyPropertyChangedEventArgs eventArgs)
        {
            ((BindableMap)sender).OnHeadingPropertyChanged((double)eventArgs.OldValue, (double)eventArgs.NewValue);
        }

        [TypeConverter(typeof(ApplicationIdCredentialsProviderConverter))]
        public CredentialsProvider CredentialsProvider
        {
            get { return map.CredentialsProvider; }
            set { map.CredentialsProvider = value; }
        }

        private void OnZoomLevelPropertyChanged(double oldValue, double newValue)
        {
            map.ZoomLevel = ZoomLevel;
        }

        private void OnCenterPropertyChanged(Location oldValue, Location newValue)
        {
            if (Center == null)
                return;
            map.Center = Center;
        }

        private void OnHeadingPropertyChanged(double oldValue, double newValue)
        {
            map.Heading = Heading;
        }

    }
}
