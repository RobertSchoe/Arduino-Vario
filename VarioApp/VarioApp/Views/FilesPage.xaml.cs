using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using VarioApp.ViewModels;
using VarioApp.Services;

namespace VarioApp.Views
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class FilesPage : ContentPage
    {
        FilesListViewModel viewModel;
        public FilesPage()
        {
            InitializeComponent();

            BindingContext = viewModel = new FilesListViewModel();
        }

        async void OnFileSelected(object sender, SelectedItemChangedEventArgs args)
        {
            var file = args.SelectedItem as TxtFile;
            if (file == null)
                return;

            string asd = KMLCreator.GetKMLFileString(EspRequester.requestRouteFile(file.Name));
            await KMLCreator.SaveKMLFileAsync(asd);
            await DisplayAlert("Converted", "this file has been converted", "accept");
            FilesListView.SelectedItem = null;
        }
        protected override void OnAppearing()
        {
            viewModel.Files.Clear();
            base.OnAppearing();

            if (viewModel.Files.Count == 0)
                viewModel.LoadFilesCommand.Execute(null);
        }
    }
}