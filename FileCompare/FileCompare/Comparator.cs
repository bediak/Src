using System;
using System.Threading.Tasks;
using System.IO;
using System.Collections.ObjectModel;
using System.Security.Cryptography;
using System.Data.SQLite;
using System.Windows;
using System.Data;
using FileCompare.Common;
using FileCompare.Model;

namespace FileCompare
{
    public class Comparator : PropertyChangedBase
    {
        public ButtonCommand AnalyzeCommand { get; set; }
        public ButtonCommand AbortCommand { get; set; }
        public ButtonCommand EraseDBCommand { get; set; }
        public ButtonCommand CompareCommand { get; set; }

        private const string _StatusMessageDefaultValue = "Select folder and run Compare.";
        private ObservableCollection<LogEntry> _LogEntries = new ObservableCollection<LogEntry>();
        private uint _FolderCount = 0, _FileCount = 0;
        private string _RootFolder = String.Empty, _StatusMessage = _StatusMessageDefaultValue, _dbFile = String.Empty;
        private ComparatorStatus _Status = ComparatorStatus.Idle;
        public enum ComparatorStatus
        {
            Idle,
            Ready,
            Running,
            Aborting
        }
        public ComparatorStatus Status
        {
            get { return _Status; }
            private set
            {
                if (value == _Status) return;
                _Status = value;
                OnPropertyChanged();
            }
        }
        public ObservableCollection<LogEntry> LogEntries
        {
            get { return _LogEntries; }
            private set
            {
                if (value == _LogEntries) return;
                LogEntries = value;
                //OnPropertyChanged();
            }
        }
        public string DatabaseFile
        {
            get
            {
                if (String.IsNullOrEmpty(_dbFile))
                    return "*";
                else
                    return _dbFile;
            }
            set
            {
                if (value == _dbFile) return;
                _dbFile = value;
                OnPropertyChanged();
            }
        }
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
        public String RootFolder {
            get { return _RootFolder; }
            set
            {
                if (value == _RootFolder) return;
                _RootFolder = value;
                UpdateStatus();
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

        private void UpdateStatus()
        {
            if (_db != null && _db.State == ConnectionState.Open && !String.IsNullOrEmpty(_RootFolder) && Directory.Exists(_RootFolder))
                Status = ComparatorStatus.Ready;
            else
                Status = ComparatorStatus.Idle;
        }

        public Comparator()
        {
            AnalyzeCommand = new ButtonCommand(this, AnalyzeAction, RunActionCondition);
            AbortCommand = new ButtonCommand(this, Abort, AbortActionCondition);
            EraseDBCommand = new ButtonCommand(this, Reset, RunActionCondition);
            CompareCommand = new ButtonCommand(this, FindDuplicates, RunActionCondition);
            UpdateStatus();
            //_db = new SQLiteConnection("Data Source=:memory:;Version=3;New=True;");
        }

        public void Abort()
        {
            if (_Status != ComparatorStatus.Running)
                throw new InvalidOperationException("Comparator is not running. You can not abort.");
            if (MessageBox.Show("Are you sure?", "Abort folder processing", MessageBoxButton.YesNo) != MessageBoxResult.Yes) return;
            _Status = ComparatorStatus.Aborting;
        }

        public void FindDuplicates()
        {
            StatusMessage = "Searching for duplicates...";
            SQLiteCommand cmd = _db.CreateCommand();
            cmd.CommandText = "SELECT name, path, size, date_mod, date_created, COUNT(*) as c FROM files INNER JOIN folders ON files.parent_folder_id = folders.rowid GROUP BY md5 HAVING c > 1";
            var dr = cmd.ExecuteReader();
            if (!dr.HasRows)
            {
                StatusMessage = "No duplicates found.";
                return;
            }
            LogEntries.Clear();
            while (dr.Read())
            {
                LogEntries.Add(new LogEntry() { MainItem = string.Format("{0}\\{1}", dr.GetString(1), dr.GetString(0)) });
            }
            StatusMessage = String.Format("Found {0} duplicates.", LogEntries.Count);
        }

        private bool AbortActionCondition()
        {
            return Status == ComparatorStatus.Ready;
        }

        private async void AnalyzeAction()
        {
            Reset();
            await InitRootAsync();
        }

        private bool RunActionCondition()
        {
            return !string.IsNullOrEmpty(RootFolder) && Directory.Exists(RootFolder) && !string.IsNullOrEmpty(DatabaseFile) && Status==ComparatorStatus.Ready;
        }

        public void OpenDB(bool CreateIfNeeded = true)
        {
            bool new_db = false;

            if (!File.Exists(DatabaseFile))
            {
                if (CreateIfNeeded)
                {
                    SQLiteConnection.CreateFile(DatabaseFile);
                    new_db = true;
                }
                else throw new FileNotFoundException("DB file does not exists", DatabaseFile);
            }
            _db = new SQLiteConnection(string.Format("Data Source={0};Version=3;", DatabaseFile));
            _db.Open();
            if (new_db)
            {
                SQLiteCommand cmd = new SQLiteCommand("CREATE TABLE folders (path TEXT(255) UNIQUE)", _db);
                cmd.ExecuteNonQuery();
                cmd = new SQLiteCommand("CREATE TABLE files (name VARCHAR(255), md5 TEXT, size INT, date_created DATETIME, date_mod DATETIME, parent_folder_id INT REFERENCES folders(rowid))", _db);
                cmd.ExecuteNonQuery();
            }
            UpdateStatus();
        }

        public void LoadFromFile()
        {
            using (var cmd = _db.CreateCommand())
            {
                cmd.CommandText = "SELECT path FROM folders";
                using (var dr = cmd.ExecuteReader())
                {
                    LogEntries.Clear();
                    while (dr.Read()) LogEntries.Add(new LogEntry() { MainItem = dr.GetString(0) });
                    FolderCount = (uint)LogEntries.Count;
                    if (FolderCount>0) RootFolder = LogEntries[0].MainItem;
                }
                cmd.CommandText = "SELECT COUNT(name) FROM files";
                FileCount = Convert.ToUInt32(cmd.ExecuteScalar());
                Status = ComparatorStatus.Ready;
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
            LogEntries.Clear();
            StatusMessage = _StatusMessageDefaultValue;
            SQLiteCommand cmd = _db.CreateCommand();
            cmd.CommandText = "DELETE FROM folders;";
            cmd.ExecuteNonQuery();
            cmd.CommandText = "DELETE FROM files;";
            cmd.ExecuteNonQuery();
            cmd.CommandText = "VACUUM;";
            cmd.ExecuteNonQuery();
            UpdateStatus();
        }

        private long FolderStoreInfo(DirectoryInfo dir)
        {
            SQLiteCommand cmd = _db.CreateCommand();
            long rowid;
            lock (LogEntries)
            {
                LogEntries.Add(new LogEntry() { MainItem = dir.FullName });
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
                if (_Status == ComparatorStatus.Aborting) return;
                await FileProcessAsync(rowid, fl);
            }
            foreach (var subdir in dir.EnumerateDirectories())
            {
                if (_Status == ComparatorStatus.Aborting) return;
                await ProcessFolderAsync(subdir);
            }
        }

        public async Task InitRootAsync()
        {
            if (Status != ComparatorStatus.Ready)
                throw new InvalidOperationException("Comparator is not ready. Configure it first.");

            StatusMessage = "Processing... (F8 for abort)";
            Status = ComparatorStatus.Running;

            DirectoryInfo root = new DirectoryInfo(RootFolder);

            await ProcessFolderAsync(root);

            StatusMessage = _Status==ComparatorStatus.Aborting?"Aborted.":"Done.";
            Status = ComparatorStatus.Ready;
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
