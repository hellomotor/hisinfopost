using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace WindowsApplication1
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        [DllImport("hisinfopost.dll", EntryPoint = "postPres", CharSet = CharSet.Ansi, CallingConvention=CallingConvention.StdCall)]
        public static extern int postPres(String url, String xml, int post_type, String encoding);

        [DllImport("hisinfopost.dll", EntryPoint = "confirmPres", CharSet = CharSet.Ansi, CallingConvention=CallingConvention.StdCall)]
        public static extern bool confirmPres(String url, String pres_no, int post_type);

        private void button1_Click(object sender, EventArgs e)
        {
            String url = "http://192.168.2.4:8081/index";
            String xml = "<xml version=\"1.0\"></xml>";

            int rc = postPres(url, xml, 0, "utf-8");
            if (rc == 8 || rc == 9)
            {
                confirmPres(url, "pres_no100", 1);
            }
        }
    }
}