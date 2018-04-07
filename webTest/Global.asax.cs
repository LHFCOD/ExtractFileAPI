using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Web;
using System.Web.Security;
using System.Web.SessionState;

namespace webTest
{
    public class Global : System.Web.HttpApplication
    {
        [DllImport("..\\..\\x64\\Release\\Release_x64_ExtractFileAPI.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void CreateMDSFile(string path);
       
        protected void Application_Start(object sender, EventArgs e)
        {
           
        }

        protected void Session_Start(object sender, EventArgs e)
        {
            string sPath = System.Web.HttpContext.Current.Request.MapPath("/");
            CreateMDSFile("F:\\document\\haoge\\bigfile\\5.mds");
            int b = 0;
            b++;
        }

        protected void Application_BeginRequest(object sender, EventArgs e)
        {

        }

        protected void Application_AuthenticateRequest(object sender, EventArgs e)
        {

        }

        protected void Application_Error(object sender, EventArgs e)
        {

        }

        protected void Session_End(object sender, EventArgs e)
        {

        }

        protected void Application_End(object sender, EventArgs e)
        {

        }
    }
}