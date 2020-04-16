using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using VarioApp.Models;
using Xamarin.Forms;
using System.Threading.Tasks;

namespace VarioApp.Services
{
    public static class KMLCreator
    {
        const string kmlheader = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\"><Document><name>Tracklog File</name><Schema id=\"schema\"><gx:SimpleArrayField name=\"Climb\" type=\"float\"><displayName>CLimb (m/s)</displayName></gx:SimpleArrayField></Schema><Placemark id='timeTrack'><name>Tracklog</name><Style id='Track1'><LineStyle><color>ff0000ff</color><width>2</width></LineStyle><IconStyle><Icon><href>http://earth.google.com/images/kml-icons/track-directional/track-0.png</href></Icon></IconStyle></Style><gx:MultiTrack id=\"MultiTrack\"><gx:Track><altitudeMode>absolute</altitudeMode>";
        const string KMLfooter = "</gx:Track></gx:MultiTrack></Placemark></Document></kml>";

        public static string GetKMLFileString(List<KMLPoint> points)
        {
            string kmlfile = kmlheader;

            foreach (KMLPoint p in points)
            {
                kmlfile += p.createDateString();
                kmlfile += "\n";
            }
            foreach (KMLPoint p in points)
            {
                kmlfile += p.createPositionString();
                kmlfile += "\n";
            }
            kmlfile += KMLfooter;

            return kmlfile;
        }
        public static async System.Threading.Tasks.Task SaveKMLFileAsync(string kmlFile)
        {
            if (Device.RuntimePlatform == Device.Android)
            {
                if (File.Exists(System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments) + ".NOMEDIA"))
                {
                    using (var writer = File.CreateText(System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments) + ".NOMEDIA"))
                    {
                        await writer.WriteLineAsync("test");
                    }
                }
                string path = "";
                for (int i = 0; i < 999999; i++)
                {
                    path = System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments);
                    if (!File.Exists(path + i.ToString() + ".kml"))
                    {
                        path += i;
                        path += ".kml";
                        break;
                    }
                }
                using (var writer = File.CreateText(path))
                {
                    await writer.WriteLineAsync(kmlFile);
                }
            }
            if (Device.RuntimePlatform == Device.UWP)
            {
                //var path = Windows.Storage.ApplicationData.Current.LocalFolder.Path;
                //string fullPath = Path.Combine(path, fileName);
            }
        }
        public static async Task<int> ReadCountAsync()
        {
            var backingFile = Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.MyDocuments), "0.kml");

            if (backingFile == null || !File.Exists(backingFile))
            {
                return 0;
            }

            var count = 0;
            using (var reader = new StreamReader(backingFile, true))
            {
                string line;
                while ((line = await reader.ReadLineAsync()) != null)
                {
                    if (int.TryParse(line, out var newcount))
                    {
                        count = newcount;
                    }
                }
            }

            return count;
        }
    }
}
