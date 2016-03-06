using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.CommandWpf;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace xeniC.AnySense.Library
{
    public abstract class FlyoutViewModel : ViewModelBase
    {
        public abstract string Title { get; }
        public abstract RelayCommand CloseCommand { get; }
    }
}
