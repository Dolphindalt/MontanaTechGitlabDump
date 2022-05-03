/** Handle Client
 * Retrieves trivia questions from a source
 * formats these questions
 * send these questions to the connected client
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
using System.Web;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;
using System.Threading.Tasks;

namespace TriviaServer
{
    /** performs operatons for each connected client
     */
    public class HandleClient
    {
        // array of colors used to decoate logging information
        // presented to the server console
        private static readonly ConsoleColor[] logColors =
        {
            ConsoleColor.Blue,
            ConsoleColor.Green,
            ConsoleColor.Cyan,
            ConsoleColor.Magenta,
            ConsoleColor.Yellow,
            ConsoleColor.Red,
            ConsoleColor.DarkBlue,
            ConsoleColor.DarkGreen,
            ConsoleColor.DarkCyan,
            ConsoleColor.DarkMagenta,
            ConsoleColor.DarkYellow,
            ConsoleColor.DarkRed
        };
        // static counter and length of array 
        // used to cycle through colors for Console
        private static int consoleColorNdx = 0;
        private static readonly int consoleColorSize = logColors.Length;

        // represents the state of the connected client
        private ClientState clientState;

        // instance of the protocol for the open trivia db 
        private OpentdbWebService opentdbWs;

        // constructor method 
        // _clientState - reference to state of connected client
        public HandleClient(ClientState _clientState)
        {
            // set client state reference
            this.clientState = _clientState;

            // create a new instance of the protocol for open trivia db
            this.opentdbWs = new OpentdbWebService();

            // set color used for Console logging for this 
            // connected client
            this.clientState.colorNdx = (++consoleColorNdx) % consoleColorSize;
        } // end constructor

        // destructor
        ~HandleClient()
        {
            // destroy token for this connecte client on the
            // open trivia database server
            _ = opentdbWs.DestroyTokenAsync(clientState);
        } // end destructor

        // process loop with the connected cleint
        public async Task ProcessAsync()
        {
            // controls when to stop processing commands from connected client
            bool continueProcessing = true;

            // attempt to get a token from web service
            // token is used to avoid getting duplicate questions
            if (!(await opentdbWs.GetTokenAsync(clientState)))
            {
                // close the connection and return
                clientState.clientSocket.Shutdown(SocketShutdown.Both);
                clientState.clientSocket.Close();
                return;
            } // end if

            // read a command from connected client
            // perform the requested action
            // send the results of the action to the connected client
            while (continueProcessing)
            {
                // read from connected client and store in clientSocket.buffer
                // if socket is closed - returning zero - then end processing
                // if client sends first character a ., then end session
                int readSz;
                if ((readSz = clientState.clientSocket.Receive(clientState.buffer)) == 0 ||
                    clientState.buffer[0] == '.')
                {
                    continueProcessing = false;
                    continue;
                } // end if

                // extract the command from the buffer read from client
                string command = string.Empty;
                for (int ndx = 0; ndx < readSz; ++ndx)
                {
                    if (clientState.buffer[ndx] == '/' ||
                        clientState.buffer[ndx] == '\n')
                    {
                        command = Encoding.ASCII.GetString(clientState.buffer, 0, ndx);
                        command = command.Trim().ToLower();
                        break;
                    } // end if
                } // end for

                // hold the individual process task based on 
                // command issued by the connected client
                Task<bool> processTask = null;

                // get a string representation of the command
                // from the connected client from the byte buffer
                string commandMsg = Encoding.ASCII.GetString(clientState.buffer, 0, readSz);
                
                // based on the command from the connected client
                switch (command)
                {
                    case "categories":
                        {
                            Log("retrieve categories");
                            processTask = process_categoriesAsync(commandMsg);
                            break;
                        } // end categories
                    case "count":
                        {
                            Log("retrieve count");
                            processTask = process_countAsync(commandMsg);
                            break;
                        } // end count
                    case "q":
                        {
                            Log("retrieve questions");
                            processTask = process_qAsync(commandMsg);
                            break;
                        } // end q
                    default:
                        {
                            Log(String.Format("unknown commnad = {0}", command));
                            processTask = process_error(commandMsg);
                            break;
                        } // end default
                } // end switch

                // send the result of the task to the connected client
                // if the process was completed correctly
                if (await processTask == true)
                {
                    // get byte array from response message from opentdb
                    byte[] clientMsg = Encoding.ASCII.GetBytes(clientState.sb.ToString());
                    // send byte array to connected client
                    clientState.clientSocket.Send(clientMsg);
                } // end if 
                else
                { // if process task errored, send error message
                    byte[] clientMsg = Encoding.ASCII.GetBytes(String.Format("err({0}):{1}", command, commandMsg));
                    clientState.clientSocket.Send(clientMsg);
                } // end if
            } // end while 

            // destroy the token for the connecte client
            _ = await opentdbWs.DestroyTokenAsync(clientState);

            // shutdown the socket for communication in both directions
            clientState.clientSocket.Shutdown(SocketShutdown.Both);

            // update console with connected client information
            DateTime endTime = DateTime.Now;
            TimeSpan duration = endTime - clientState.startTime;
            Console.WriteLine("{0} {1}: Client {2} session ended ({3:F2}s)",
                endTime.ToShortDateString(),
                endTime.ToShortTimeString(),
                clientState.clientSocket.RemoteEndPoint.ToString(),
                duration.TotalSeconds);

            // close the socket
            clientState.clientSocket.Close();
        } // end process

        // process an error
        private async Task<bool> process_error(string _msg)
        {
            // clear the string builder
            clientState.sb.Clear();

            // store the error message in the string builder
            clientState.sb.AppendFormat("err(command):{0}", _msg);

            // return success
            return true;
        } // end process error

        // process question command
        private async Task<bool> process_qAsync(string _msg)
        {
            // split the command line from the client into tokens
            string[] command = _msg.Split('/');

            // iterate over commands and trim of whitespaces and convert to
            // upper case for textual values
            for (int ndx = 0; ndx < command.Length; ++ndx)
            {
                command[ndx] = command[ndx].Trim();
            }// end for

            // list for holding the response
            List<Result> questionList;

            // issue the correct command to the opentdb based on 
            // command options from the connected client
            switch (command.Length)
            {
                case 1:
                    { // empty option set
                        questionList = await opentdbWs.QuestionsAsync(clientState);
                        break;
                    }
                case 2:
                    { // include category option 
                        Log(String.Format("   category = {0}", command[1]));
                        questionList = await opentdbWs.QuestionsAsync(clientState, _category: command[1]);
                        break;
                    }
                case 3:
                    { // include both the cateogry and quantity options
                        Log(String.Format("   category = {0} amount = {1}", command[1], command[2]));
                        int amount = int.Parse(command[2]);

                        questionList = await opentdbWs.QuestionsAsync(clientState, _category: command[1], _amount: amount);
                        break;
                    }
                default:
                    { // otherwise, return false to the caller
                        return false;
                    }
            } // end swtich

            // if not questions returned, error from the opentdb
            // therefore, return and error
            if (questionList == null)
            {
                return false;
            } // end if

            // clear the string builder
            clientState.sb.Clear();

            // add the number of questions to the string builder
            clientState.sb.AppendFormat("{0}\n", questionList.Count);

            // cycle through each question and add it to the string builder
            foreach (var q in questionList)
            {
                clientState.sb.AppendFormat("{0}/{1}/{2}/{3}",
                    q.type, q.difficulty,
                    HttpUtility.HtmlDecode(q.question),
                    HttpUtility.HtmlDecode(q.correct_answer));
                foreach (var ica in q.incorrect_answers)
                {
                    clientState.sb.AppendFormat("/{0}", HttpUtility.HtmlDecode(ica));
                } // end foreach
                clientState.sb.Append('\n');
            } // end foreach

            // indicate success;
            return true;
        } // end process question

        /// <summary>
        /// request the current categories (id/name) from the web service
        /// sets reponse in clientState.sb
        /// </summary>
        /// <param name="_msg">the string sent by client</param>
        /// <returns>true - if able to process web service, false otherwise</returns>
        private async Task<bool> process_categoriesAsync(string _msg)
        {
            // obtain the category list from the web service
            var catList = await opentdbWs.CategoriesAsync();

            // if the list is empty, return failure
            if (catList == null)
            {
                return false;
            } // end if

            // clear the string builder for the response
            // append the number of categories as the fist line
            // append each subsequent line with id/name lines
            clientState.sb.Clear();
            clientState.sb.AppendFormat("{0}\n", catList.trivia_categories.Count);
            foreach (var category in catList.trivia_categories) {
                clientState.sb.AppendFormat("{0}/{1}\n", category.id, category.name);
            } // end foreach

            // indicate success
            return true;
        } // end process_categoriesAsync

        /// <summary>
        /// request the count for a category (or for all) from the web service
        /// sets response in clientState.sb
        /// </summary>
        /// <param name="_msg">the string sent by client</param>
        /// <returns>true - if able to proess web service, false otherwise</returns>
        private async Task<bool> process_countAsync(string _msg)
        {
            // split the command line from the client into tokens
            string[] command = _msg.Split('/');

            // if there are not two (2) token, failure
            if (command.Length != 2)
            {
                return false;
            } // end if

            // iterate over commands and trim of whitespaces and convert to
            // upper case for textual values
            for (int ndx = 0; ndx < command.Length; ++ndx)
            {
                command[ndx] = command[ndx].Trim().ToUpper();
            }// end for

            // log for which category we are retrieving the count
            Log(String.Format("   category = {0}", command[1]));

            // obtain the count for the category from the web service
            var countTuple = await opentdbWs.CountAsync(command[1]);

            // if there is an error, failure
            if (countTuple == (-1, -1, -1, -1))
            {
                return false;
            } // end if

            // clear the string builder for the response
            // append the tuple
            // - for count/all: total-questions/pending/verified/rejected
            // - for count/xx: total/num_easy/num_medium/num_hard
            clientState.sb.Clear();
            clientState.sb.AppendFormat("{0}/{1}/{2}/{3}\n",
                countTuple.Item1, countTuple.Item2, countTuple.Item3, countTuple.Item4);

            // indicate success
            return true;
        } // end process_count

        // log messages to the Console
        private void Log(string _msg, bool newline = true)
        {
            // current date/time of message
            DateTime timeStamp = DateTime.Now;

            // string building for formatting Console message
            StringBuilder sb = new StringBuilder();

            // clear the string builder
            sb.Clear();

            // add the message to the string builder
            sb.AppendFormat("{0} {1}: Client {2} {3}",
                timeStamp.ToShortDateString(),
                timeStamp.ToShortTimeString(),
                clientState.clientSocket.RemoteEndPoint.ToString(),
                _msg);

            // output message to the console in the color 
            // associated with the connected cleint
            Console.ForegroundColor = logColors[clientState.colorNdx];
            Console.WriteLine(sb.ToString());
            Console.ResetColor();
        } // end log messge

    } // end Handle Client
} // end namespace
