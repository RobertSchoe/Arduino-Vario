using System;
using System.Collections.Generic;
using System.Text;

namespace VarioApp.Models
{
    public class KMLFile
    {
        string path;

        public KMLFile(string _path)
        {
            path = _path;
        }
        public string Path { get { return path; } }

    }
}
