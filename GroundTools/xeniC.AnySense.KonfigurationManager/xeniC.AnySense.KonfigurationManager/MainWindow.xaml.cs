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
using GalaSoft.MvvmLight.Ioc;

namespace xeniC.AnySense.Studio
{
    /// <summary>
    /// Interaktionslogik für MainWindow.xaml
    /// </summary>
    public partial class MainWindow : MetroWindow, IDialogController
    {

        public MainWindow()
        {
            InitializeComponent();
            SimpleIoc.Default.Register<IDialogController>(() => { return this; });
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

                    System.Windows.Forms.Application.Restart();
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

                Title += string.Format(" (Version {0}.{1}.{2})", ad.CurrentVersion.Major, ad.CurrentVersion.Minor, ad.CurrentVersion.Build);
            }

            DataContext = new MainViewModel();
        }

        #region IDialogController implementation

        public Task<string> ShowInput(string title, string message)
        {
            return this.ShowInputAsync(title, message);
        }

        public async Task<DialogResult> ShowMessage(string title, string message, DialogStyle style = DialogStyle.Affirmative, DialogSettings settings = null)
        {
            var x = await Application.Current.Dispatcher.InvokeAsync(() =>
            {
                return this.ShowMessageAsync(title, message, (MessageDialogStyle)style, settings != null ? new BridgeMetroDialogSettings(settings) : null);
            });

            x.Wait();

            if (x.Exception != null)
                throw x.Exception;

            return (DialogResult)x.Result;
        }

        public async Task<IProgressController> ShowProgress(string title, string message, bool isCancelable = false, DialogSettings settings = null)
        {
            var x = await Application.Current.Dispatcher.InvokeAsync(() =>
            {
                return this.ShowProgressAsync(title, message, isCancelable, settings != null ? new BridgeMetroDialogSettings(settings) : null);
            });

            x.Wait();

            if (x.Exception != null)
                throw x.Exception;

            return new BridgeProgressController(x.Result);
        }

        private class BridgeMetroDialogSettings : MetroDialogSettings
        {
            public BridgeMetroDialogSettings(DialogSettings settings)
            {
                base.DefaultText = settings.DefaultText;
                base.AffirmativeButtonText = settings.AffirmativeButtonText;
                base.NegativeButtonText = settings.NegativeButtonText;
                base.FirstAuxiliaryButtonText = settings.FirstAuxiliaryButtonText;
                base.SecondAuxiliaryButtonText = settings.SecondAuxiliaryButtonText;
                base.MaximumBodyHeight = settings.MaximumBodyHeight;
                base.AnimateShow = settings.AnimateShow;
                base.AnimateHide = settings.AnimateHide;
            }
        }

        private class BridgeProgressController : IProgressController
        {
            private ProgressDialogController pd;

            public BridgeProgressController(ProgressDialogController pd)
            {
                this.pd = pd;
            }

            public bool IsCanceled { get { return pd.IsCanceled; } }
            public bool IsOpen { get { return pd.IsOpen; } }
            public Task CloseAsync() { return pd.CloseAsync(); }
            public void SetCancelable(bool value) { pd.SetCancelable(value); }
            public void SetIndeterminate() { pd.SetIndeterminate(); }
            public void SetMessage(string message) { pd.SetMessage(message); }
            public void SetProgress(double value) { pd.SetProgress(value); }
            public void SetTitle(string title) { pd.SetTitle(title); }
        }

        #endregion

    }
}
