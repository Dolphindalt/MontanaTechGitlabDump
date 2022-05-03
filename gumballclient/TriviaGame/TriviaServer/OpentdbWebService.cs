/** Open Trivia Database Web Service
 * Communications with the opentdb.com web service
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
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Configuration;
using System.Net.Http;
using Newtonsoft.Json;
using System.Threading.Tasks;

namespace TriviaServer
{
    /// <summary>
    /// Web Service calls to open trivia database
    /// </summary>
    public class OpentdbWebService
    {
        // object for managing HTTP req/res transactions
        static readonly HttpClient opentdbWS = new HttpClient();

        // Named value pair for holding setting from .config
        // specific to the opentdb web service
        private NameValueCollection opentdbSettings;

        // response codes to meaning mapping
        private Dictionary<int,string> responseCodes;

        /// <summary>
        /// constructor
        /// </summary>
        public OpentdbWebService()
        {
            // obtain name value collection from web service .config file
            this.opentdbSettings =
                (NameValueCollection) ConfigurationManager.GetSection("opentdbSettings");

            // initialize the response code dictionary
            this.responseCodes =
                new Dictionary<int, string>() {
                    {0, "Success" },
                    {1, "No Resulst" },
                    {2, "Invalid Parameter" },
                    {3, "Token Not Found" },
                    {4, "Token Empty" }
                }; // end dictionary
        } // end constructor

        /// <summary>
        /// Get a token from web service and store in client state object
        /// </summary>
        /// <param name="_clientState">client state object for holding token</param>
        /// <returns>a task that returns a bool upon completion</returns>
        public async Task<bool> GetTokenAsync(ClientState _clientState)
        {
            // if the current client state has a token, return true
            if (_clientState.Token != null)
            {
                return true;
            } // end if

            // obtain a token from the web service,
            // store in the client state object,
            // return the token to the caller

            // response body from the web service call
            string responseBody = null;

            // request for a new token
            string request = opentdbSettings["baseURL"] +
                             opentdbSettings["tokenURI"] + "?" +
                             opentdbSettings["tokenRequest"];

            try
            {
                // process the web service request for a token
                responseBody = await opentdbWS.GetStringAsync(request);
            }
            catch (HttpRequestException e)
            {
                Console.Error.WriteLine("Exception caught:");
                Console.Error.WriteLine("Message: {0}", e.Message);

                // return false due to the error
                return false;
            } // end catch

            // obtain an object from the JSON response form the web service call
            Token respObj = JsonConvert.DeserializeObject<Token>(responseBody);

            // verify the status of the web service call
            if (respObj.response_code != 0)
            {
                Console.Error.WriteLine("Token Error: {0}",
                    responseCodes[respObj.response_code]);
                return false;
            } // end if

            // set token in client state to token returned by web service
            _clientState.Token = respObj.token;

            // return success
            return true;
        } // end GetTokenAsync

        /// <summary>
        /// destroys the token on the web service stored in the clientstate object
        /// </summary>
        /// <param name="_clientState">instance holding the token</param>
        /// <returns>a task that returns a bool upon completion</returns>
        public async Task<bool> DestroyTokenAsync(ClientState _clientState)
        {
            // if client state does not have a token, return true
            if (_clientState.Token == null)
            {
                return true;
            } // end if

            // response body from web service call
            string responseBody = null;

            // request to destroy current token
            string request = opentdbSettings["baseURL"] +
                             opentdbSettings["tokenURI"] + "?" +
                             opentdbSettings["tokenReset"] + "&" +
                             opentdbSettings["tokenToken"];

            try
            {
                // process the web service request for a token
                responseBody =
                    await opentdbWS.GetStringAsync(request.Replace("{_TOKEN}", _clientState.Token));
            }
            catch (HttpRequestException e)
            {
                Console.Error.WriteLine("Exception caught:");
                Console.Error.WriteLine("Message: {0}", e.Message);

                // return false due to the error
                return false;
            } // end catch

            // obtain an object from the JSON response from the web service call
            Token respObj = JsonConvert.DeserializeObject<Token>(responseBody);

            // verify the status of the web service call
            if (respObj.response_code != 0)
            {
                Console.Error.WriteLine("Token Error: {0}",
                    responseCodes[respObj.response_code]);
                return false;
            } // end if

            // set the token in the client state to null
            _clientState.Token = null;

            // return success
            return true;
        } // end DestroyTokenAsync

        /// <summary>
        /// retrieve a 4-tuple representing the count of all q's or from specific category
        /// </summary>
        /// <param name="_category">category filter</param>
        /// <returns>4-tuple representing the count</returns>
        public async Task<(int,int,int,int)> CountAsync(string _category = "ALL")
        {
            // response body for web service call
            string responseBody = null;

            // request to obtain count of questions in a category
            // or all categories if _category = "ALL"
            string request = opentdbSettings["baseURL"];

            // if the category filter is set to ALL|all
            if (_category == "ALL")
            {
                // base od the reqeust for glocal count of questions
                request += opentdbSettings["countGlobalURI"];
            }
            else
            { // category filter is a specific category

                // determine if the category provided is valid
                bool categoryValid = false;
                var catList = await CategoriesAsync();
                foreach (var category in catList.trivia_categories)
                    if (category.id.ToString() == _category)
                    {
                        categoryValid = true;
                        break;
                    }

                // if category provided is not valid, return error
                if (!categoryValid)
                {
                    return (-1, -1, -1, -1);
                }

                // base of the request for count within category
                request += opentdbSettings["categoryCountURI"] + "?" +
                           opentdbSettings["category"];

                // replace category with desired 
                request = request.Replace("{_CATEGORY}", _category);
            } // end if
            
            try
            {
                // process the web service request for category count
                responseBody = await opentdbWS.GetStringAsync(request);
            }
            catch (HttpRequestException e)
            {
                Console.Error.WriteLine("Exception caught:");
                Console.Error.WriteLine("Message: {0}", e.Message);

                // return to the caller
                return (-1, -1, -1, -1);
            } // end catch

            // return correct tuple for specific category or gloabl count
            if (_category != "ALL")
            {
                // interpret result as a count for a specific category
                // and return tuple from category count
                CategoryCount respObj = JsonConvert.DeserializeObject<CategoryCount>(responseBody);
                return (respObj.category_question_count.total_question_count,
                        respObj.category_question_count.total_easy_question_count,
                        respObj.category_question_count.total_medium_question_count,
                        respObj.category_question_count.total_hard_question_count);
            }
            else
            {
                // interpret result as a global count for all questions
                // and return tuple from global count
                GlobalCount respObj = JsonConvert.DeserializeObject<GlobalCount>(responseBody);
                return (respObj.overall.total_num_of_questions,
                        respObj.overall.total_num_of_pending_questions,
                        respObj.overall.total_num_of_verified_questions,
                        respObj.overall.total_num_of_rejected_questions);
            } // end if
        } // end CountAsync

        /// <summary>
        /// retrieve a category list in the form id/name
        /// </summary>
        /// <returns>an object representing the category list</returns>
        public async Task<CategoryList> CategoriesAsync()
        {
            // response body for web service call
            string responseBody = null;

            // request to obtain category list from web service
            string request = opentdbSettings["baseURL"] +
                             opentdbSettings["categoryURI"];

            try
            {
                // process the web service reqeust for the category list
                responseBody = await opentdbWS.GetStringAsync(request);
            }
            catch (HttpRequestException e)
            {
                Console.Error.WriteLine("Exception caught:");
                Console.Error.WriteLine("Message: {0}", e.Message);
                return null;
            }

            // parse the Json into a CategoryList object
            CategoryList respObj = JsonConvert.DeserializeObject<CategoryList>(responseBody);

            // return the CategoryList object
            return respObj;
        } // end CategoriesAsync

        /// <summary>
        /// retrieve a collection of question with a specific category and number
        /// </summary>
        /// <param name="_clientState">the client state holding the token</param>
        /// <param name="_amount">the amount of questions to retrieve</param>
        /// <param name="_category">the category filter</param>
        /// <returns>a collection of questions retrieved</returns>
        public async Task<List<Result>> QuestionsAsync(ClientState _clientState, int _amount = 10, string _category = "18")
        {
            // response body for web service call
            string responseBody = null;

            // request to obtain trivia questions from web service
            string request = opentdbSettings["baseURL"] +
                             opentdbSettings["questionURI"] + "?" +
                             opentdbSettings["tokenToken"] + "&" +
                             opentdbSettings["amount"];

            // if the category is set, then add category filter to request
            if (_category != "ALL")
            {
                // determine if th category provided is valid
                bool categoryValid = false;
                var catList = await CategoriesAsync();
                foreach (var category in catList.trivia_categories)
                    if (category.id.ToString() == _category)
                    {
                        categoryValid = true;
                        break;
                    }

                // if the category provided is not valid, return error
                if (!categoryValid)
                {
                    return null;
                }

                // add the category filter to the request
                request += "&" + opentdbSettings["category"];
                request = request.Replace("{_CATEGORY}", _category);
            }

            // replace the amount with correct value
            request = request.Replace("{_AMOUNT}", _amount.ToString());

            // replace the token with the correct value
            request = request.Replace("{_TOKEN}", _clientState.Token);

            try
            {
                // process the web service request for the questions
                responseBody = await opentdbWS.GetStringAsync(request);
            }
            catch (HttpRequestException e)
            {
                Console.Error.WriteLine("Exception caught:");
                Console.Error.WriteLine("Message: {0}", e.Message);

                // return a null object if encounter an error
                return null;
            } // end catch

            // parse the Json into a QuestionList object
            QuestionList respObj = JsonConvert.DeserializeObject<QuestionList>(responseBody);

            // Check to make sure haven't exhausted question list for the
            // category under the token; if so, reissue a new token
            if ( respObj.response_code == 4 &&
                 (! await DestroyTokenAsync(_clientState) ||
                  ! await GetTokenAsync(_clientState)) )
            {
                // notify of an error and do not return any results
                Console.Error.WriteLine("Could not reset token");
                return null;
            } // end

            // return the results object List<Result> to the caller
            return respObj.results;
        } // end QuestionsAsync
    } // end OpentdbWebService
} // end namespace
