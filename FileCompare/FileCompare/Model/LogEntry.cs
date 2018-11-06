using FileCompare.Common;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FileCompare.Model
{
    public class LogEntry : PropertyChangedBase
    {
        public uint Rowid { get; set; }
        public string MainItem { get; set; }
        public uint DuplicatesCount { get; set; }
        public int FileSize { get; set; }
        public List<ItemDuplicates> Duplicates { get; set; }
    }

    public class ItemDuplicates : PropertyChangedBase
    {
        public uint Rowid { get; set; }
        public bool IsSelected { get; set; }
        public string FilePath { get; set; }
    }
}
