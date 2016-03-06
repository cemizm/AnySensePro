using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace xeniC.AnySense.Library
{
    public interface IDialogController
    {
        Task<string> ShowInput(string title, string message);
        Task<DialogResult> ShowMessage(string title, string message, DialogStyle style = DialogStyle.Affirmative, DialogSettings settings = null);
        Task<IProgressController> ShowProgress(string title, string message, bool isCancelable = false, DialogSettings settings = null);
        
    }

    public enum DialogResult
    {
        Negative = 0,
        Affirmative = 1,
        FirstAuxiliary = 2,
        SecondAuxiliary = 3,
    }

    public enum DialogStyle
    {
        Affirmative = 0,
        AffirmativeAndNegative = 1,
        AffirmativeAndNegativeAndSingleAuxiliary = 2,
        AffirmativeAndNegativeAndDoubleAuxiliary = 3,
    }

    public class DialogSettings
    {
        public string AffirmativeButtonText { get; set; }
        public bool AnimateHide { get; set; }
        public bool AnimateShow { get; set; }
        public string DefaultText { get; set; }
        public string FirstAuxiliaryButtonText { get; set; }
        public double MaximumBodyHeight { get; set; }
        public string NegativeButtonText { get; set; }
        public string SecondAuxiliaryButtonText { get; set; }
    }

    public interface IProgressController
    {
        bool IsCanceled { get; }
        bool IsOpen { get; }
        Task CloseAsync();
        void SetCancelable(bool value);
        void SetIndeterminate();
        void SetMessage(string message);
        void SetProgress(double value);
        void SetTitle(string title);
    }

}
