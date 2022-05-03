using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Configuration;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Extensions.Configuration;
using System.IO;

namespace GumballServer
{
    class Program
    {
        static void Main(string[] args)
        {
            GumballServer gs = new GumballServer();
            var builder = new ConfigurationBuilder()
                .SetBasePath(Directory.GetCurrentDirectory())
                .AddJsonFile("appsettings.json", optional: false);
            IConfigurationRoot config = builder.Build();
            string hostname = config.GetSection("hostname").Value;
            int port = Int32.Parse(config.GetSection("port").Value);
            int queueSize = Int32.Parse(config.GetSection("queueSize").Value);
            gs.Start(hostname, port, queueSize);
        }
    }

    class ClientState
    {
        private static readonly int _bufferSize = 500;
        public byte[] buffer { get; }
        public DateTime startTime { get; }
        public Socket clientSocket { get; }

        public ClientState(Socket clientSocket)
        {
            this.buffer = new byte[_bufferSize];
            this.clientSocket = clientSocket;
            this.startTime = DateTime.Now;
        }
    }

    class ClientHandler
    {
        private ClientState _clientState { get; set; }
        public ClientHandler(ClientState clientState)
        {
            this._clientState = clientState;
        }

        public void ProcessAsync()
        {
            // One machine for each user approach.
            new FiniteStateMachine(this._clientState);
        }
    }

    class GumballServer
    {
        // I read about semaphores in a book once. Why have I not seen them until now?
        public ManualResetEvent _allDone = new ManualResetEvent(false);

        public void Start(string host, int port, int queueSize)
        {
            IPHostEntry ipHostInfo = Dns.GetHostEntry(host);
            IPAddress ipAddress = ipHostInfo.AddressList[0];
            IPEndPoint localEndPoint = new IPEndPoint(ipAddress, port);
            Socket listenSocket = new Socket(ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                listenSocket.Bind(localEndPoint);
                listenSocket.Listen(queueSize);
                DateTime now = DateTime.Now;
                Console.WriteLine("{0} {1}: Listening at {2} on {3}", now.ToShortDateString(), now.ToShortTimeString(), ipAddress.ToString(), port.ToString());

                while (true)
                {
                    _allDone.Reset();

                    listenSocket.BeginAccept(new AsyncCallback(AcceptCallback), listenSocket);

                    _allDone.WaitOne();
                }
            }
            catch (Exception) {}
            Console.WriteLine("Stopping server...");
        }

        public void AcceptCallback(IAsyncResult result)
        {
            Socket clientSocket = (Socket)result.AsyncState;
            _allDone.Set();
            ClientState clientState = new ClientState(clientSocket.EndAccept(result));
            Console.WriteLine("{0} {1}: Client {2}",
                clientState.startTime.ToShortDateString(),
                clientState.startTime.ToShortTimeString(),
                clientState.clientSocket.RemoteEndPoint.ToString());
            new ClientHandler(clientState).ProcessAsync();
        }
    }
}
