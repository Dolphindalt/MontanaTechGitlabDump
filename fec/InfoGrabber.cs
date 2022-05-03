using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;

namespace FECClient
{

    class InfoGrabber
    {
        public string[] readContent { get; }

        public InfoGrabber(string host, int port)
        {
            Socket socket = null;
            IPHostEntry hostEntry = null;
            
            try 
            {
                hostEntry = Dns.GetHostEntry(host);
                IPAddress address = hostEntry.AddressList[0];
                IPEndPoint ipe = new IPEndPoint(address, port);
                socket = new Socket(ipe.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
                socket.Connect(ipe);
            } catch (Exception e)
            {
                Console.WriteLine("Could not connect to TCP server: " + e);
                System.Environment.Exit(0);
            }

            string result = this.read(socket);
            readContent = this.parseResult(result);
        }

        private string read(Socket socket)
        {
            byte[] buff = new byte[2048];
            int read = socket.Receive(buff);
            if (read < 0)
            {
                return null;
            }
            return System.Text.Encoding.Default.GetString(buff);
        }

        private string[] parseResult(string result)
        {
            string[] lines = result.Split(new string[] { "\r\n" }, StringSplitOptions.None);
			lines = lines[0..(lines.Length-2)]; // Trim last newline.
            return lines;
        }
    }

}
