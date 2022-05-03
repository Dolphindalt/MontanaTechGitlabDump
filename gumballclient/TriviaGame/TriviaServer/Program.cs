/** Trivia Question Server
 *
 * F19 CSCI 466 Networks
 *
 * Trivial Server proxy to openTDB
 *
 * Phillip J. Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 406-496-4807
 * Department of Computer Science, Montana Tech
 */

using System;
using System.Configuration;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace TriviaServer
{
    public class Program 
    {
        // Default host if not specified on command line or config file
        private const string defaultHost = "localhost";

        // Default port if not specified on command line or config file 
        private const string defaultPort = "30124";

        // Default listen queue length
        private const string defaultQueue = "100";

        // Semaphore to synchronize the async calls across threads
        // start unsignaled - the false parameter
        public static ManualResetEvent allDone = new ManualResetEvent(false);

        /// <summary>
        /// Start the trivia server, bind and listen on server port, start
        /// accepting connectings using async creating new threads for each
        /// accepted client connection
        /// </summary>
        /// <param name="_serverHost">Server Host address for bind/listen</param>
        /// <param name="_serverPort">Server Port for bind/listen</param>
        /// <param name="_serverQueue">Server Listen queue</param>
        public static void StartServer(string _serverHost, int _serverPort, int _serverQueue)
        {
            // Obtain ipHostEntry from the string of the FQDN or IP Address
            IPHostEntry ipHostInfo = Dns.GetHostEntry(_serverHost);

            // Obtain the first ip address from the ipHostEntry
            IPAddress ipAddress = ipHostInfo.AddressList[0];

            // Create an enpoint from the IP Address and Port
            IPEndPoint localEndPoint = new IPEndPoint(ipAddress, _serverPort);

            // Create an Interent TCP Stream Socket - used for listening for connections
            Socket listenSocket = new Socket(ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

            try
            {
                // Bind the address and port to this server process
                listenSocket.Bind(localEndPoint);

                // begin listening for incoming client connections
                // and up to queue connections
                listenSocket.Listen(_serverQueue);

                // Log the start of the server starting
                DateTime now = DateTime.Now;
                Console.WriteLine("{0} {1}: Listening at {2} on {3}", now.ToShortDateString(), now.ToShortTimeString(), ipAddress.ToString(), _serverPort);

                // Servers run forever
                while (true)
                {
                    // reset the semaphore that synchronizes threads for client thread
                    allDone.Reset();

                    // Accept a client connection, blocking until
                    // once accepted, create a new thread in which AcceptCallback executes
                    // pass the listenSocket to the callback in the AsyncResult
                    listenSocket.BeginAccept(new AsyncCallback(AcceptCallback), listenSocket);

                    // Use the semaphore to clock until a signal is set
                    // used to synchronize from the new thread to handle the accept callback
                    allDone.WaitOne();
                }  // end while
            } // end try
            catch (Exception e)
            {
                // If an exception, write out the exception
                Console.WriteLine(e.ToString());
            } // end catch

            // Annotate the server has terminated
            Console.WriteLine("Server stopped...");
        } // end StartServer method

        /// <summary>
        /// The callback - running in a new thread - to handle the client
        /// accepted
        /// </summary>
        /// <param name="ar">any data passwd to the callback</param>
        public static void AcceptCallback(IAsyncResult ar)
        {
            // Create a new ClientState object to tracke state for connected client
            ClientState clientState = new ClientState();

            // store client socket endpoint from Accept in parent thread
            Socket clientSocket = (Socket)ar.AsyncState;

            // Signal semaphore so parent thread can continue to accept next client
            allDone.Set();

            // update client state with its socket information
            // end the async accept from parent thread
            clientState.clientSocket = clientSocket.EndAccept(ar);

            // update console with connect client information
            Console.WriteLine("{0} {1}: Client {2}",
                clientState.startTime.ToShortDateString(),
                clientState.startTime.ToShortTimeString(),
                clientState.clientSocket.RemoteEndPoint.ToString());

            // create a new client handler and process the connected client
            // interactions in an asynchronous way - returnning immediately
            _ = new HandleClient(clientState).ProcessAsync();
        } // end AcceptCallback

        /// <summary>
        /// Main entry point for trivia server
        /// Process command line arguments and start the server
        /// </summary>
        /// <param name="args">Holds command line arguments</param>
        public static void Main(string[] args)
        {
            // Reference to the AppSetting k,v Collection
            var AppSettings = ConfigurationManager.AppSettings;

            // Set ServerHost to defaultHost if not defined in AppSettings
            string ServerHost = (AppSettings["Host"] == null) ? defaultHost : AppSettings["Host"];

            // Set ServerPortStr to defaultPort if not defined in AppSettings
            string ServerPortStr = (AppSettings["Port"] == null) ? defaultPort : AppSettings["Port"];

            // Set ServerQueueStr to defaultQueue if not defined in AppSettings
            string ServerQueueStr = (AppSettings["ListenQueue"] == null) ? defaultQueue : AppSettings["ListenQueue"];

            // Result of parsing port from string
            int ServerPort;

            // Result of parsing listen queue length from string
            int ServerQueue;

            // Cycle through all command line arguments looking for host and port
            for (int ndx = 0; ndx < args.Length; ndx ++)
            {
                // If the host flag is found, set ServerHost to next argument
                if (args[ndx] == "-h" && ndx+1 < args.Length)
                {
                    ServerHost = args[++ndx];
                    continue;
                } // end if

                // If the port flag is found, set ServerPortStr to next argument
                if (args[ndx] == "-p" && ndx+1 < args.Length)
                {
                    ServerPortStr = args[++ndx];
                    continue;
                } // end if

                // If the listen queue flag is found, set ServerQeuueStr to next arguement
                if (args[ndx] == "-q" && ndx+1 < args.Length)
                {
                    ServerQueueStr = args[++ndx];
                    continue;
                } // end if
            } // end for

            // Parse the ServerPortStr into a 32-bit integer
            Int32.TryParse(ServerPortStr, out ServerPort);

            // Parse the ServerQueueStr into 32-bit integer
            Int32.TryParse(ServerQueueStr, out ServerQueue);

            // Start the Trivia Server passing the host and port
            StartServer(ServerHost, ServerPort, ServerQueue);

        } // end Main entry point
    } // end class
} // end namespace
