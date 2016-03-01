using MahApps.Metro;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

namespace xeniC.AnySense.Studio
{
    /// <summary>
    /// Interaktionslogik für "App.xaml"
    /// </summary>
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);
            ThemeManager.AddAccent("AnySenseAccent", new Uri("pack://application:,,,/AnySenseStudio;component/styles/AnySenseAccent.xaml"));

            // get the theme from the current application
            var theme = ThemeManager.DetectAppStyle(Application.Current);

            // now use the custom accent
            ThemeManager.ChangeAppStyle(Application.Current,
                                    ThemeManager.GetAccent("AnySenseAccent"),
                                    theme.Item1);

        }
    }
}
