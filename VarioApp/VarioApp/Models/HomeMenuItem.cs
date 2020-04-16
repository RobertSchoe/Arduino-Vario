using System;
using System.Collections.Generic;
using System.Text;

namespace VarioApp.Models
{
    public enum MenuItemType
    {
        Browse,
        About,
        FileList,
        KMLFileList
    }
    public class HomeMenuItem
    {
        public MenuItemType Id { get; set; }

        public string Title { get; set; }
    }
}
