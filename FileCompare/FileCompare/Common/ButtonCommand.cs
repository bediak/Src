using System;
using System.Windows.Input;

namespace FileCompare.Common
{
    public class ButtonCommand : ICommand
    {
        Action _action;
        Func<bool> _condition;
        //PropertyChangedBase _parent;
        
        public ButtonCommand(PropertyChangedBase parent, Action ExeAction, Func<bool> CanExecuteCondition)
        {
            _action = ExeAction;
            _condition = CanExecuteCondition;
            parent.PropertyChanged += delegate { CanExecuteChanged?.Invoke(this, EventArgs.Empty); };
        }

        public event EventHandler CanExecuteChanged;

        public bool CanExecute(object param)
        {
            return _condition();
            //return !string.IsNullOrEmpty(parent.RootFolder) && Directory.Exists(parent.RootFolder) && !string.IsNullOrEmpty(parent.dbFile);
        }

        public void Execute(object param)
        {
            _action();
        }
    }
}
