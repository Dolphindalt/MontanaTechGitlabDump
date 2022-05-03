/** Client State Object
 * Containes state data for each client connected
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
using System.Text;
using System.Net.Sockets;

namespace TriviaServer
{
    /** represents the state of each connected client
     */
    public class ClientState
    {
        // default buffer size if not configured in settings
        private const string defaultBufferSize = "1024";

        // store the actual buffer size
        public int BufferSize;

        // timestamp for client connection
        public DateTime startTime;

        // token string used to ensure no duplicates
        public string Token;

        // buffer used to store data between client/server and web service
        public byte[] buffer;

        // string building for constructing response to client
        public StringBuilder sb;

        // hold client socket endpoint information
        public Socket clientSocket;

        // holds the color for this connected client to decorate 
        // information logged to the server console
        public int colorNdx;

        /// <summary>
        /// Constructor for Client State
        /// </summary>
        public ClientState()
        {
            // reference to the appsettings section in the app.config
            var AppSettings = ConfigurationManager.AppSettings;

            // if the BufferSize is defined in appSettings use it, otherwise default
            string BufferSizeStr = (AppSettings["BufferSize"] == null) ? defaultBufferSize : AppSettings["BufferSize"];

            // parse buffersizestr into 32-bit int
            Int32.TryParse(BufferSizeStr, out this.BufferSize);

            // allocate a byte buffer of size BufferSize
            this.buffer = new byte[this.BufferSize];

            // instantiate a stringbuilder  object
            this.sb = new StringBuilder();

            // initialize token to null
            // actual toekn retrieved from trivia  db server
            this.Token = null;

            // initialize clientsocket to null
            this.clientSocket = null;

            // set start time for the client
            this.startTime = DateTime.Now;
        } // end constructor
    } // end ClientState class
} // end namespace

