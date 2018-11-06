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
using System.Windows.Threading;
using System.Threading;

namespace FileCompare
{
    public class Comparator : PropertyChangedBase
    {
        public AsyncCommand AnalyzeCommand { get; set; }
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
            AnalyzeCommand = new AsyncCommand(this, AnalyzeAction, RunActionCondition);
            AbortCommand = new ButtonCommand(this, Abort, AbortActionCondition);
            EraseDBCommand = new ButtonCommand(this, Reset, RunActionCondition);
            CompareCommand = new ButtonCommand(this, FindDuplicates, CompareActionCondition);
            UpdateStatus();
            //_db = new SQLiteConnection("Data Source=:memory:;Version=3;New=True;");
        }

        public void Abort()
        {
            if (_Status != ComparatorStatus.Running)
                throw new InvalidOperationException("Comparator is not running. You can not abort.");
            MessageBoxHelper.PrepToCenterMessageBoxOnForm(Application.Current.MainWindow);
            if (MessageBox.Show(Application.Current.MainWindow, "Are you sure?", "Abort folder processing", MessageBoxButton.YesNo, MessageBoxImage.Question) != MessageBoxResult.Yes) return;
            _Status = ComparatorStatus.Aborting;
        }

        private bool AbortActionCondition()
        {
            return Status == ComparatorStatus.Running;
        }

        private async Task AnalyzeAction()
        {
            Reset();
            await InitRootAsync();
        }

        private bool RunActionCondition()
        {
            return !string.IsNullOrEmpty(RootFolder) && Directory.Exists(RootFolder) && !string.IsNullOrEmpty(DatabaseFile) && Status==ComparatorStatus.Ready;
        }

        private bool CompareActionCondition()
        {
            return _db != null && !string.IsNullOrEmpty(DatabaseFile) && Status == ComparatorStatus.Ready && FileCount>0;
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
            if (FolderCount == 0 && FileCount == 0) return;

            if (_Status == ComparatorStatus.Running)
                throw new InvalidOperationException("Comparator is running. You can not erase the database. Abort processing first.");
            MessageBoxHelper.PrepToCenterMessageBoxOnForm(Application.Current.MainWindow);
            if (MessageBox.Show(Application.Current.MainWindow, "Are you sure?", "Erase database", MessageBoxButton.YesNo, MessageBoxImage.Question) != MessageBoxResult.Yes) return;

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

        private async Task<long> FolderStoreInfoAsync(DirectoryInfo dir)
        {
            SQLiteCommand cmd = _db.CreateCommand();
            long rowid;
            cmd.CommandText = "INSERT INTO folders (path) VALUES (@path)";
            cmd.Parameters.AddWithValue("@path", dir.FullName);
            await cmd.ExecuteNonQueryAsync();
            rowid = _db.LastInsertRowId;
            LogEntries.Add(new LogEntry() { MainItem = dir.FullName });
            FolderCount++;
            return rowid;
        }

        private async Task FileProcessAsync (long rowid, FileInfo fl)
        {
            //StatusMessage = "Processing " + fl.Name;
            SQLiteCommand cmd = _db.CreateCommand();

            //cmd.CommandText = "INSERT INTO files (name, md5, size, date_created, date_mod, parent_folder_id) VALUES (@name, @md5, @size, @date_created, @date_mod, @parent_folder_id)";
            cmd.CommandText = "INSERT INTO files (name, size, date_created, date_mod, parent_folder_id) VALUES (@name, @size, @date_created, @date_mod, @parent_folder_id)";
            cmd.Parameters.AddWithValue("@name", fl.Name);
            //cmd.Parameters.AddWithValue("@md5", md5.Result);
            cmd.Parameters.AddWithValue("@size", fl.Length);
            cmd.Parameters.AddWithValue("@date_created", fl.CreationTimeUtc.ToString("yyyy-MM-dd HH:mm:ss"));
            cmd.Parameters.AddWithValue("@date_mod", File.GetLastWriteTimeUtc(fl.FullName).ToString("yyyy-MM-dd HH:mm:ss"));
            cmd.Parameters.AddWithValue("@parent_folder_id", rowid);
            await cmd.ExecuteNonQueryAsync();
            FileCount++;
            Application.Current.Dispatcher.Invoke(DispatcherPriority.Background, new ThreadStart(delegate { }));
        }

        private async Task BrowseFolderAsync(DirectoryInfo dir)
        {
            SQLiteCommand cmd = _db.CreateCommand();
            long rowid = await FolderStoreInfoAsync(dir);

            foreach (var fl in dir.EnumerateFiles())
            {
                if (_Status == ComparatorStatus.Aborting) return;
                await FileProcessAsync(rowid, fl);
            }
            foreach (var subdir in dir.EnumerateDirectories())
            {
                if (_Status == ComparatorStatus.Aborting) return;
                await BrowseFolderAsync(subdir);
            }
        }

        public async Task InitRootAsync()
        {
            if (Status != ComparatorStatus.Ready)
                throw new InvalidOperationException("Comparator is not ready. Configure it first.");

            StatusMessage = "Processing...";
            Status = ComparatorStatus.Running;

            DirectoryInfo root = new DirectoryInfo(RootFolder);

            await BrowseFolderAsync(root);

            StatusMessage = "Generating hashes...";

            await DuplicatesCalculateMD5();

            StatusMessage = _Status==ComparatorStatus.Aborting?"Aborted.":"Done.";
            Status = ComparatorStatus.Ready;
        }

        public async Task InitRootAsync(string RootFolderPath)
        {
            RootFolder = RootFolderPath;
            await InitRootAsync();
        }

        public void FindDuplicates()
        {
            StatusMessage = "Searching for duplicates...";
            SQLiteCommand cmd = _db.CreateCommand();
            //cmd.CommandText = "SELECT name, path, size, date_mod, date_created, COUNT(*) as c FROM files INNER JOIN folders ON files.parent_folder_id = folders.rowid GROUP BY md5 HAVING c > 1";
            cmd.CommandText = @"
                SELECT files.rowid, files.name, folders.path, f.c, files.md5, files.size FROM files
                INNER JOIN folders ON files.parent_folder_id = folders.rowid
                JOIN ( SELECT rowid, md5, COUNT(*) as c FROM files GROUP BY md5 HAVING c > 1) f ON files.md5 = f.md5 ORDER BY files.size DESC";
            var dr = cmd.ExecuteReader();
            if (!dr.HasRows)
            {
                StatusMessage = "No duplicates found.";
                return;
            }
            LogEntries.Clear();
            string md5 = String.Empty;
            LogEntry item = null;
            while (dr.ReadAsync().Result)
            {
                if (md5 != dr.GetString(4))
                {
                    if (item != null) LogEntries.Add(item);
                    item = new LogEntry() { Rowid = (uint)dr.GetInt32(0), MainItem = Path.Combine(dr.GetString(2), dr.GetString(1)), DuplicatesCount = (uint)dr.GetInt32(3), FileSize = dr.GetInt32(5), Duplicates = new System.Collections.Generic.List<ItemDuplicates>() };
                    md5 = dr.GetString(4);
                }
                else
                {
                    item.Duplicates.Add(new ItemDuplicates() { Rowid = (uint)dr.GetInt32(0), FilePath = Path.Combine(dr.GetString(2), dr.GetString(1)), IsSelected = false });
                }
            }
            StatusMessage = String.Format("Found {0} duplicates.", LogEntries.Count);
        }

        private async Task DuplicatesCalculateMD5()
        {
            Task<string> md5;
            SQLiteCommand cmd = _db.CreateCommand();
            SQLiteCommand cmd2 = _db.CreateCommand();
            cmd.CommandText = @"
                SELECT files.rowid, files.name, folders.path FROM files
                INNER JOIN folders ON files.parent_folder_id = folders.rowid
                JOIN ( SELECT size FROM files GROUP BY size HAVING count(*) > 1) f ON files.size = f.size ORDER BY files.size DESC";
            cmd2.CommandText = "UPDATE files SET md5 = @md5 WHERE rowid = @id";
            using (SQLiteDataAdapter adapter = new SQLiteDataAdapter(cmd))
            {
                using (DataTable dups = new DataTable("duplicates"))
                {
                    adapter.Fill(dups);
                    int rowCount = dups.Rows.Count;
                    uint rowIndex = 0;
                    if (rowCount==0) return;
                    foreach (DataRow row in dups.Rows)
                    {
                        StatusMessage = String.Format("Generating {0} of {1} hashes.",++rowIndex, rowCount);
                        Application.Current.Dispatcher.Invoke(DispatcherPriority.Background, new ThreadStart(delegate { }));
                        md5 = Task.Run(() => CalculateMD5(Path.Combine(row[2].ToString(), row[1].ToString())));
                        await Task.WhenAll(md5);
                        cmd2.Parameters.Clear();
                        cmd2.Parameters.AddWithValue("@id", Convert.ToInt32(row[0]));
                        cmd2.Parameters.AddWithValue("@md5", md5.Result);
                        await cmd2.ExecuteNonQueryAsync();
                    }
                }
            }
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
