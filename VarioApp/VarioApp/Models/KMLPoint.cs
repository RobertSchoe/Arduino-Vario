using System;
using System.Collections.Generic;
using System.Text;

namespace VarioApp.Models
{
    public class KMLPoint
    {
        string Position;
        string Time;
        string Date;
        public KMLPoint(string position, string time, string date)
        {
            Position = position;
            Time = time;
            Date = date;
        }

        public string createDateString()
        {
            return "<when>" + Date + Time + "</when>";
        }

        public string createPositionString()
        {
            return "<gx:coord>" + Position + "</gx:coord>";
        }
    }
}
