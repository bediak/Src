﻿using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading;
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
using Microsoft.WindowsAPICodePack.Dialogs;


namespace FileCompare
{
    /// <summary>
    /// Interakční logika pro MainWindow.xaml
    /// </summary>
    public partial class FileComparator : Window
    {
        public Comparator engine;

        public FileComparator()
        {
            Thread.CurrentThread.CurrentUICulture = new System.Globalization.CultureInfo("en-us");

            InitializeComponent();

            //this.Height = 152;

            engine = new Comparator();
            DataContext = engine;
        }

        private void btnChangeFolder_Click(object sender, RoutedEventArgs e)
        {
            if (CommonFileDialog.IsPlatformSupported)
            {
                using (var dlg = new CommonOpenFileDialog())
                {
                    dlg.IsFolderPicker = true;
                    //dlg.InitialDirectory = 
                    if (dlg.ShowDialog() == CommonFileDialogResult.Ok)
                    {
                        tbPath.Text = dlg.FileName;
                    }
                }
            }
            else
            {
                using (var dlg = new System.Windows.Forms.FolderBrowserDialog())
                {
                    if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                    {
                        tbPath.Text = dlg.SelectedPath;
                    }
                }
            }
        }

        private void Open_DB(object sender, RoutedEventArgs e)
        {
            bool changed_path = false;
            if (CommonFileDialog.IsPlatformSupported)
            {
                using (var dlg = new CommonOpenFileDialog())
                {
                    dlg.IsFolderPicker = false;
                    dlg.EnsurePathExists = true;
                    //dlg.InitialDirectory = 
                    if (dlg.ShowDialog() == CommonFileDialogResult.Ok
                        && engine.dbFile != dlg.FileName)
                    {
                        engine.dbFile = dlg.FileName;
                        changed_path = true;
                    }
                }
            }
            else
            {
                using (var dlg = new System.Windows.Forms.OpenFileDialog())
                {
                    dlg.CheckPathExists = true;
                    if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK
                        && engine.dbFile != dlg.FileName)
                    {
                        engine.dbFile = dlg.FileName;
                        changed_path = true;
                    }
                }
            }
            if (changed_path)
            {
                engine.OpenDB();
                engine.LoadFromFile();
                btnCleanDB.IsEnabled = true;
            }
        }

        private void Clean_DB(object sender, RoutedEventArgs e)
        {
            engine.Reset();
        }
    }
}
