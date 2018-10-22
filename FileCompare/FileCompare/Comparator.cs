using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Collections.ObjectModel;
using System.Security.Cryptography;
using System.Data.SQLite;
using System.Threading;
using System.ComponentModel;
using System.Windows;
using System.Runtime.CompilerServices;
using System.Windows.Input;
using System.Windows.Threading;
using System.Data;

namespace FileCompare
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

    public class LogEntry: PropertyChangedBase
    {
        public string DirPath { get; set; }
    }

    public class Comparator : PropertyChangedBase
    {

        class RunComparatorCommand : ICommand
        {
            Comparator parent;

            public RunComparatorCommand(Comparator parent)
            {
                this.parent = parent;
                parent.PropertyChanged += delegate { CanExecuteChanged?.Invoke(this, EventArgs.Empty); };
            }

            public event EventHandler CanExecuteChanged;

            public bool CanExecute(object param)
            {
                return !string.IsNullOrEmpty(parent.RootFolder) && Directory.Exists(parent.RootFolder) && !string.IsNullOrEmpty(parent.dbFile);
            }

            public void Execute(object param)
            {
                parent.Reset();
                parent.InitRootAsync();
            }
        }

        public ICommand RunCommand { get; set; }

        private uint _FolderCount = 0, _FileCount = 0;
        private string _RootFolder = string.Empty, _StatusMessage = "Select folder and run Compare.";
        private ObservableCollection<LogEntry> _Folders = new ObservableCollection<LogEntry>();

        public string dbFile;
        public uint FolderCount {
            get { return _FolderCount; }
            private set
            {
                if (value == _FolderCount) return;
                _FolderCount = value;
                OnPropertyChanged();
            }
        }
        public uint FileCount
        {
            get { return _FileCount; }
            private set
            {
                if (value == _FileCount) return;
                _FileCount = value;
                OnPropertyChanged();
            }
        }
        public ObservableCollection<LogEntry> Folders
        {
            get { return _Folders; }
            private set
            {
                if (value == _Folders) return;
                _Folders = value;
                OnPropertyChanged();
            }
        }
        public String RootFolder {
            get { return _RootFolder; }
            set
            {
                if (value == _RootFolder) return;
                _RootFolder = value;
                OnPropertyChanged();
            }
        }
        public String StatusMessage
        {
            get { return _StatusMessage; }
            set
            {
                if (value == _StatusMessage) return;
                _StatusMessage = value;
                OnPropertyChanged();
            }
        }

        private SQLiteConnection _db;

        public Comparator()
        {
            RunCommand = new RunComparatorCommand(this);
            //_db = new SQLiteConnection("Data Source=:memory:;Version=3;New=True;");
        }

        public void OpenDB(bool CreateIfNeeded = true)
        {
            bool new_db = false;

            if (!File.Exists(dbFile))
            {
                if (CreateIfNeeded)
                {
                    SQLiteConnection.CreateFile(dbFile);
                    new_db = true;
                }
                else throw new FileNotFoundException("DB file does not exists", dbFile);
            }
            _db = new SQLiteConnection(string.Format("Data Source={0};Version=3;", dbFile));
            _db.Open();
            if (new_db)
            {
                SQLiteCommand cmd = new SQLiteCommand("CREATE TABLE folders (path TEXT(255) UNIQUE)", _db);
                cmd.ExecuteNonQuery();
                cmd = new SQLiteCommand("CREATE TABLE files (name VARCHAR(255), md5 TEXT, size INT, date_created DATETIME, date_mod DATETIME, parent_folder_id INT REFERENCES folders(rowid))", _db);
                cmd.ExecuteNonQuery();
            }
        }

        public void LoadFromFile()
        {
            using (var cmd = _db.CreateCommand())
            {
                cmd.CommandText = "SELECT path FROM folders";
                using (var dr = cmd.ExecuteReader())
                {
                    Folders.Clear();
                    while (dr.Read())
                        Folders.Add(new LogEntry() { DirPath = dr.GetString(0) });
                    FolderCount = (uint)Folders.Count;
                }
                cmd.CommandText = "SELECT COUNT(name) FROM files";
                FileCount = Convert.ToUInt32(cmd.ExecuteScalar());
            }
        }

        ~Comparator()
        {
            try
            {
                //if (_db.State != System.Data.ConnectionState.Closed) _db.Close();
                _db.Dispose();
            }
            catch { }
        }

        public void Reset()
        {
            FolderCount = 0;
            FileCount = 0;
            Folders.Clear();
            StatusMessage = "Select folder and run Compare.";
            SQLiteCommand cmd = _db.CreateCommand();
            cmd.CommandText = "DELETE FROM folders;";
            cmd.ExecuteNonQuery();
            cmd.CommandText = "DELETE FROM files;";
            cmd.ExecuteNonQuery();
            cmd.CommandText = "VACUUM;";
            cmd.ExecuteNonQuery();
        }

        private long FolderStoreInfo(DirectoryInfo dir)
        {
            SQLiteCommand cmd = _db.CreateCommand();
            long rowid;
            lock (Folders)
            {
                Folders.Add(new LogEntry() { DirPath = dir.FullName });
                cmd.CommandText = "INSERT INTO folders (path) VALUES (@path)";
                cmd.Parameters.AddWithValue("@path", dir.FullName);
                cmd.ExecuteNonQuery();
                rowid = _db.LastInsertRowId;
            }
            FolderCount++;
            return rowid;
        }

        private async Task FileProcessAsync (long rowid, FileInfo fl)
        {
            //StatusMessage = "Processing " + fl.Name;
            SQLiteCommand cmd = _db.CreateCommand();
            Task<string> md5 = Task.Run(() => CalculateMD5(fl.FullName));
            await Task.WhenAll(md5);

            cmd.CommandText = "INSERT INTO files (name, md5, size, date_created, date_mod, parent_folder_id) VALUES (@name, @md5, @size, @date_created, @date_mod, @parent_folder_id)";
            cmd.Parameters.AddWithValue("@name", fl.Name);
            cmd.Parameters.AddWithValue("@md5", md5.Result);
            cmd.Parameters.AddWithValue("@size", fl.Length);
            cmd.Parameters.AddWithValue("@date_created", fl.CreationTimeUtc.ToString("yyyy-MM-dd HH:mm:ss"));
            cmd.Parameters.AddWithValue("@date_mod", File.GetLastWriteTimeUtc(fl.FullName).ToString("yyyy-MM-dd HH:mm:ss"));
            cmd.Parameters.AddWithValue("@parent_folder_id", rowid);
            await cmd.ExecuteNonQueryAsync();
            FileCount++;
        }

        private async Task ProcessFolderAsync(DirectoryInfo dir)
        {
            SQLiteCommand cmd = _db.CreateCommand();
            long rowid = FolderStoreInfo(dir);

            foreach (var fl in dir.EnumerateFiles())
            {
                await FileProcessAsync(rowid, fl);
            }
            foreach (var subdir in dir.EnumerateDirectories())
            {
                await ProcessFolderAsync(subdir);
            }
        }

        public async Task InitRootAsync()
        {
            StatusMessage = "Processing...";

            DirectoryInfo root = new DirectoryInfo(RootFolder);

            await ProcessFolderAsync(root);

            StatusMessage = "Done.";
        }

        public async Task InitRootAsync(string RootFolderPath)
        {
            RootFolder = RootFolderPath;
            await InitRootAsync();
        }

        private string CalculateMD5(string filename)
        {
            using (var md5 = MD5.Create())
            {
                using (var stream = File.OpenRead(filename))
                {
                    var hash = md5.ComputeHash(stream);
                    return BitConverter.ToString(hash).Replace("-", "").ToLowerInvariant();
                }
            }
        }
    }
}
