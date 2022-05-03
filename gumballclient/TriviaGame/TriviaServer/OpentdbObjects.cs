/* Open Trivia Database Web Service
 * Communications with the opentdb.com web service
 *
 * F19 CSCI 466 Networks
 *
 * Trivial Server
 * Classes for Objects from JSON responses
 * from open trivia db 
 *
 * Phillip J.Curtiss, Associate Professor
 * pcurtiss@mtech.edu, 406-496-4807
 * Department of Computer Science, Montana Tech
 */
using System;
using System.Collections.Generic;

namespace TriviaServer
{
    #region token

    /// <summary>
    /// object from tokenRequest
    /// </summary>
    public class Token
    {
        public int response_code { get; set; }
        public string response_message { get; set; }
        public string token { get; set; }
    } // end token

    #endregion

    #region count command

    /// <summary>
    /// object from GlobalCount.Overall
    /// holding total counts for all questions
    /// </summary>
    public class Overall
    {
        public int total_num_of_questions { get; set; }
        public int total_num_of_pending_questions { get; set; }
        public int total_num_of_verified_questions { get; set; }
        public int total_num_of_rejected_questions { get; set; }
    } // end Overall

    /// <summary>
    /// object from count/all
    /// </summary>
    public class GlobalCount
    {
        public Overall overall { get; set; }
        public Object categories { get; set; }
    } // end GlobalCount

    /// <summary>
    /// object from CategoryCount.CategoryQuestionCount
    /// </summary>
    public class CategoryQuestionCount
    {
        public int total_question_count { get; set; }
        public int total_easy_question_count { get; set; }
        public int total_medium_question_count { get; set; }
        public int total_hard_question_count { get; set; }
    } // end CategoryQuestonCount

    /// <summary>
    /// object from count/id - category id
    /// </summary>
    public class CategoryCount
    {
        public int category_id { get; set; }
        public CategoryQuestionCount category_question_count { get; set; }
    } // end CategoryCount

    #endregion

    #region categories command

    /// <summary>
    /// object from CategoryList.List<TriviaCategory>
    /// </summary>
    public class TriviaCategory
    {
        public int id { get; set; }
        public string name { get; set; }
    } // end TriviaCategory

    /// <summary>
    /// object from categories
    /// </summary>
    public class CategoryList
    {
        public List<TriviaCategory> trivia_categories { get; set; }
    } // end CategoryList

    #endregion

    #region q command

    /// <summary>
    /// object from QuestionList.List<Result>
    /// </summary>
    public class Result
    {
        public string category { get; set; }
        public string type { get; set; }
        public string difficulty { get; set; }
        public string question { get; set; }
        public string correct_answer { get; set; }
        public List<string> incorrect_answers { get; set; }
    } // end Result


    /// <summary>
    /// object from q/id/n
    /// </summary>
    public class QuestionList
    {
        public int response_code { get; set; }
        public List<Result> results { get; set; }
    } // end QuestionList

    #endregion

} // end namespace
