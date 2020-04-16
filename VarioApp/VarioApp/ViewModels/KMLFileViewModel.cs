using System;
using System.Collections.Generic;
using System.Text;
using Xamarin.Forms;
using System.Threading.Tasks;
using VarioApp.Models;
using VarioApp.Views;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Net;
using System.IO;
using VarioApp.Services;
using System.Windows.Input;
using System.Linq;

namespace VarioApp.ViewModels
{
    class KMLFileViewModel : BaseViewModel
    {
        public ObservableCollection<KMLFile> KMLFiles { get; set; }
        public Command LoadKMLFilesCommand { get; set; }

        public KMLFileViewModel()
        {
            Title = "KML Files";
            KMLFiles = new ObservableCollection<KMLFile>();
            LoadKMLFilesCommand = new Command(async () => await ExecuteLoadKMLFilesCommand());
        }
        async Task ExecuteLoadKMLFilesCommand()
        {
            IsBusy = true;

            KMLFiles.Clear();
            //var files = Directory.GetFiles(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments));
            /*var files = Directory.EnumerateFiles(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "*.*", SearchOption.AllDirectories).Where(s => s.EndsWith(".kml"));
            if (files == null)
                return;

            foreach (var file in files)
            {
                if (file.Contains(".kml"))
                {
                    KMLFiles.Add(new KMLFile(file));
                }
            }*/
            string path;
            for (int i = 0; i < 999999; i++)
            {
                path = System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments);
                if (!File.Exists(path + i.ToString() + ".kml"))
                {
                    break;
                }
                KMLFiles.Add(new KMLFile(path + i.ToString() + ".kml"));
            }
            IsBusy = false;
        }
    }
}
