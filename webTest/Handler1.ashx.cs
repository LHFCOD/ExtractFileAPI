using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Runtime.InteropServices;
using System.Text;

namespace webTest
{
    /// <summary>
    /// Summary description for Handler1
    /// </summary>
    public class Handler1 : IHttpHandler
    {
        [DllImport("..\\..\\x64\\Release\\Release_x64_ExtractFileAPI.dll", CallingConvention=CallingConvention.Cdecl)]
        public static extern void CreateMDSFile(string path);
        [DllImport("..\\..\\x64\\Release\\Release_x64_ExtractFileAPI.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetTileData(int Level, int x, int y);
        [DllImport("..\\..\\x64\\Release\\Release_x64_ExtractFileAPI.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetTileSize(int Level, int x, int y);
        [DllImport("..\\..\\x64\\Release\\Release_x64_ExtractFileAPI.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int TestDll(string path);
        int i = 0;
        public void ProcessRequest(HttpContext context)
        {
            int x = Convert.ToInt32(HttpContext.Current.Request.QueryString["x"]);
            int y = Convert.ToInt32(HttpContext.Current.Request.QueryString["y"]);
            int level = Convert.ToInt32(HttpContext.Current.Request.QueryString["level"]);
           
            int len = GetTileSize(level, x, y);
         
            if (len > 0)
            {
                IntPtr data = GetTileData(level, x, y);

                byte[] byteData=new byte[len];
                context.Response.ContentType = "image/jpeg";
                Marshal.Copy(data, byteData, 0, len);
                context.Response.BinaryWrite(byteData);

            }
            else
            {
                int z1 = 0;
                z1++;
                int len1 = GetTileSize(level, x, y);
                z1++;
            }
          
           // context.Response.Write("111");
        }

        public bool IsReusable
        {
            get
            {
                return false;
            }
        }
    }
}