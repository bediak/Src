using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace FileCompare.Common
{
    public interface IAsyncCommand : ICommand
    {
        Task ExecuteAsync(object parameter);
    }

    public abstract class CommandBaseAsync : IAsyncCommand
    {
        public abstract bool CanExecute(object parameter);
        public abstract Task ExecuteAsync(object parameter);
        public async void Execute(object parameter)
        {
            await ExecuteAsync(parameter);
        }
        //public event EventHandler CanExecuteChanged
        //{
        //    add { CommandManager.RequerySuggested += value; }
        //    remove { CommandManager.RequerySuggested -= value; }
        //}
        public abstract event EventHandler CanExecuteChanged;
        protected void RaiseCanExecuteChanged()
        {
            CommandManager.InvalidateRequerySuggested();
        }
    }

    public class AsyncCommand : CommandBaseAsync
    {
        private readonly Func<Task> _command;
        private readonly Func<bool> _condition;

        public AsyncCommand(PropertyChangedBase parent, Func<Task> command, Func<bool> CanExecuteCondition)
        {
            _command = command;
            _condition = CanExecuteCondition;
            parent.PropertyChanged += delegate { CanExecuteChanged?.Invoke(this, EventArgs.Empty); };
        }
        public override event EventHandler CanExecuteChanged;
        public override bool CanExecute(object parameter)
        {
            return _condition();
        }
        public override Task ExecuteAsync(object parameter)
        {
            return _command();
        }
    }
}
