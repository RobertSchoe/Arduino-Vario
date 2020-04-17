using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using VarioApp.Models;

namespace VarioApp.Services
{
    public class EspRequester
    {
        public static List<KMLPoint> requestRouteFile(string path)
        {
            string date = "";
            List<KMLPoint> Points = new List<KMLPoint>();
            WebRequest request = WebRequest.Create("http://192.168.4.1/routes/"+path);
            request.Credentials = CredentialCache.DefaultCredentials;

            WebResponse response = request.GetResponse();
            Console.WriteLine(((HttpWebResponse)response).StatusDescription);

            using (Stream dataStream = response.GetResponseStream())
            {
                StreamReader reader = new StreamReader(dataStream);
                while (reader.Peek() >= 0)
                {
                    string responseFromServer = reader.ReadLine();

                    if (responseFromServer.Contains('T'))
                    {
                        date = responseFromServer;
                    }
                    else
                    {
                        string[] resparts = responseFromServer.Split('-');
                        Points.Add(new KMLPoint(resparts[1], resparts[0], date));
                    }
                }
            }
            return Points;
        }
    }
}
