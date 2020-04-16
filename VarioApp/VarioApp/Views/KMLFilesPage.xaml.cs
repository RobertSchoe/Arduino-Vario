using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using VarioApp.ViewModels;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using VarioApp.Models;
using Xamarin.Essentials;

namespace VarioApp.Views
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class KMLFilesPage : ContentPage
    {
        KMLFileViewModel viewModel;
        public KMLFilesPage()
        {
            InitializeComponent();

            BindingContext = viewModel = new KMLFileViewModel();
        }

        async void OnFileSelected(object sender, SelectedItemChangedEventArgs args)
        {
            var file = args.SelectedItem as KMLFile;

            await Launcher.OpenAsync(new OpenFileRequest
            {
                File = new ReadOnlyFile(file.Path
                )
            });
        }
        protected override void OnAppearing()
        {
            viewModel.KMLFiles.Clear();
            base.OnAppearing();

            viewModel.LoadKMLFilesCommand.Execute(null);
        }
    }
}