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
using MahApps.Metro.Controls;
using xeniC.AnySense.Library;
using System.Deployment.Application;
using MahApps.Metro.Controls.Dialogs;

namespace xeniC.AnySense.Studio
{
    /// <summary>
    /// Interaktionslogik für MainWindow.xaml
    /// </summary>
    public partial class MainWindow : MetroWindow
    {

        public MainWindow()
        {
            InitializeComponent();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            var flyout = this.Flyouts.Items[0] as Flyout;
            if (flyout == null)
                return;

            flyout.IsOpen = !flyout.IsOpen;
        }

        private async void MetroWindow_ContentRendered(object sender, EventArgs e)
        {
            if (ApplicationDeployment.IsNetworkDeployed)
            {
                ProgressDialogController pd = await this.ShowProgressAsync("Software Update", "Checking for updates...", false);
                ApplicationDeployment ad = ApplicationDeployment.CurrentDeployment;

                pd.SetIndeterminate();

                ad.UpdateProgressChanged += (o, args) => { pd.SetProgress(args.ProgressPercentage / 100f); };
                ad.UpdateCompleted += (o, args) =>
                {
                    if (args.Error != null)
                        return;

                    System.Diagnostics.Process.Start(Application.ResourceAssembly.Location);
                    Application.Current.Shutdown();
                };

                UpdateCheckInfo info = await Task.Run(() =>
                {
                    try
                    {
                        return ad.CheckForDetailedUpdate(false);
                    }
                    catch { }

                    return null;
                });

                await pd.CloseAsync();

                if (info != null && info.UpdateAvailable)
                {
                    MessageDialogResult res = await this.ShowMessageAsync("Software Update", string.Format("AnySense Studio {0}.{1}.{2} " +
                                                                          "is available and is ready to install.",
                                                                          info.AvailableVersion.Major,
                                                                          info.AvailableVersion.Minor,
                                                                          info.AvailableVersion.Build),
                                                           MessageDialogStyle.AffirmativeAndNegative,
                                                           new MetroDialogSettings()
                                                           {
                                                               AffirmativeButtonText = "Install Now",
                                                               NegativeButtonText = "Later"
                                                           });

                    if (res == MessageDialogResult.Affirmative)
                    {
                        pd = await this.ShowProgressAsync("Software Update", "Updating...", false);

                        try { ad.UpdateAsync(); }
                        catch { }

                        return;
                    }
                }
            }

            DataContext = new MainViewModel();
        }

    }
}
