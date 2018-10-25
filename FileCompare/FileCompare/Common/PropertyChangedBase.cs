using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace FileCompare.Common
{
    public abstract class PropertyChangedBase : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName]string caller = null)
        {
            if (PropertyChanged != null) PropertyChanged(this, new PropertyChangedEventArgs(caller));

            //PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(caller));

            //Application.Current.Dispatcher.BeginInvoke((Action)(() =>
            //{
            //    PropertyChangedEventHandler handler = PropertyChanged;
            //    if (handler != null) handler(this, new PropertyChangedEventArgs(caller));
            //}));
        }
    }
}
