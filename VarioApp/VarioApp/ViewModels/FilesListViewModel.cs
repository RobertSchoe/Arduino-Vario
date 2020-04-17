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

namespace VarioApp.ViewModels
{
    class FilesListViewModel : BaseViewModel
    {
        public ObservableCollection<TxtFile> Files { get; set; }
        public Command LoadFilesCommand { get; set; }
        public string files;

        public FilesListViewModel()
        {
            Title = "Parse to KML";
            Files = new ObservableCollection<TxtFile>();
            LoadFilesCommand = new Command(async () => await ExecuteLoadFilesCommand());
        }
        async Task ExecuteLoadFilesCommand()
        {
            WebRequest request = WebRequest.Create("http://192.168.4.1/list?dir=routes");
            request.Credentials = CredentialCache.DefaultCredentials;

            WebResponse response = request.GetResponse();
            Console.WriteLine(((HttpWebResponse)response).StatusDescription);

            using (Stream dataStream = response.GetResponseStream())
            {
                StreamReader reader = new StreamReader(dataStream);
                string responseFromServer = reader.ReadToEnd();
                files = responseFromServer;
            }

            response.Close();

            var list = TxtFile.FromJson(files);
            foreach (var txt in list)
                Files.Add(txt);
        }
    }
}
