using System;
using System.Collections.Generic;

namespace MyFSM
{
    class Program
    {
        static void Main(string[] args)
        {
            new FiniteStateMachine();
        }
    }

    class GumballData
    {
        public int quarters { get; set; }
        public int dimes { get; set; }
        public int nickels { get; set; }
        public int running_quarters { get; set; }
        public int running_dimes { get; set; }
        public int running_nickels { get; set; }
        public int running_total { get; set; }
        public int cost { get; set; }
        public int[] gumballs { get; set; }
        public int totalGumballs;

        public GumballData()
        {
            gumballs = new int[4];
            Random random = new Random(Guid.NewGuid().GetHashCode());
            this.totalGumballs = 0;
            for (int i = 0; i < 4; i++)
            {
                int r = random.Next(0, 6);
                gumballs[i] = r;
                this.totalGumballs += r;
            }
            cost = random.Next(5, 101);
            cost -= cost % 5;
            running_total = 0;
            quarters = 0;
            dimes = 0;
            nickels = 0;
            running_quarters = 0;
            running_dimes = 0;
            running_nickels = 0;
        }

        // Sets all totals that are tracked to zero.
        public void zeroTotals()
        {
            this.running_quarters = 0;
            this.running_dimes =  0;
            this.running_nickels = 0;
            this.running_total = 0;
            this.quarters = 0;
            this.dimes = 0;
            this.nickels = 0;
        }

        // Maps a gumball color to an index, returning the color name or empty string.
        public string getGumballColor(int index)
        {
            switch (index) 
            {
                case 0: return "R";
                case 1: return "G";
                case 2: return "Y";
                case 3: return "B";
                default: return "";
            }
        }

        // Returns a gumball color from the machine or null if there are none.
        // Takes care of adjusting totalGumballs as gumballs are dispensed.
        public string getGumball()
        {
            for (int i = 0; i < this.gumballs.Length; i++)
            {
                int gumballCount = this.gumballs[i];
                if (gumballCount > 0) {
                    this.gumballs[i]--;
                    this.totalGumballs--;
                    return getGumballColor(i);
                }
            }
            return null;
        }
    }

    class FiniteStateMachine
    {
        private GumballData data;
        private GumballInputParser parser;
        private ErrorHandler errorHandler;
        private Dictionary<int, Dictionary<string, Func<int>>> transitions; // dict[state,dict[command,action<state>]]
        private int currentState;

        public FiniteStateMachine()
        {
            this.errorHandler = new ErrorHandler();
            this.parser = new GumballInputParser(this.errorHandler);
            this.transitions = new Dictionary<int, Dictionary<string, Func<int>>>();

            // STATE 0 TRANSITIONS: INIT
            this.transitions.Add(0, new Dictionary<string, Func<int>>());
            this.transitions[0].Add("TURN", () => {
                this.errorHandler.sendError(430, parser.input);
                return 0;
            });
            this.transitions[0].Add("BVAL", () => {
                this.doBval();
                return 0;
            });
            this.transitions[0].Add("ABRT", () => {
                Environment.Exit(0);
                return 0;
            });
            this.transitions[0].Add("COIN", () => {
                return this.doCoin() ? 1 : 0;
            });
            // END INIT

            // STATE 1 TRANSITIONS: COIN
            this.transitions.Add(1, new Dictionary<string, Func<int>>());
            this.transitions[1].Add("COIN", () => {
                this.doCoin();
                return 1;
            });
            this.transitions[1].Add("TURN", () => {
                int resultCode = doTurn();
                if (resultCode == 2)
                {
                    // We ran out of gumballs! Reset the machine!
                    this.doMachineReset();
                    return 0;
                }
                // We got a gumball, goto init or continue in coin state.
                return resultCode == 1 ? 0 : 1;
            });
            this.transitions[1].Add("ABRT", () => {
                this.doAbort();
                return 0;
            });
            this.transitions[1].Add("BVAL", () => {
                this.errorHandler.sendError(432, this.parser.input);
                return 1;
            });
            // END COIN
            this.execute();
        }

        private void execute() {
            string time = String.Format("{0:F}", new DateTime());
            Console.Write("210 Greeting from the csdept10.cs.mtech.edu Gumball Server (v.2.1) at " + time + "\r\n");
            this.doMachineReset();
            this.currentState = 0;
            while (true)
            {
                if (this.parser.parse(Console.ReadLine()))
                {
                    string command = this.parser.label;
                    int nextState = this.transitions[this.currentState][command]();
                    this.currentState = nextState;
                }
            }
        }

        private void doBval()
        {
            int V = parser.value;
            if (V < 5 || V > 100)
            {
                this.errorHandler.sendError(440, parser.input);
                return;
            }
            if (V % 5 != 0)
            {
                this.errorHandler.sendError(441, parser.input);
                return;
            }
            this.data.cost = V;
            Console.Write("200 BVAL " + this.data.cost + "\r\n");
        }

        // Returns false if an error is encountered.
        private bool doCoin()
        {
            if (this.parser.moneyTriple.Item1 >= 5 || this.parser.moneyTriple.Item2 > 10 ||
                this.parser.moneyTriple.Item3 > 20)
            {
                this.errorHandler.sendError(422, this.parser.input);
                return false;
            }
            if (this.parser.moneyTriple.Item1 < 0 || this.parser.moneyTriple.Item2 < 0 ||
                this.parser.moneyTriple.Item3 < 0)
            {
                this.errorHandler.sendError(420, this.parser.input);
                return false;
            }
            this.data.quarters = this.parser.moneyTriple.Item1;
            this.data.dimes = this.parser.moneyTriple.Item2;
            this.data.nickels = this.parser.moneyTriple.Item3;
            Console.Write("200 RCVD Q" + this.data.quarters + 
                        ":D" + this.data.dimes + ":N" + this.data.nickels + "\r\n");
            return true;
        }

        // Reuturn codes: 0 turn no gumball, 1 turn gumball, 2 turn gumball no more left
        private int doTurn()
        {
            this.data.running_quarters += this.data.quarters;
            this.data.running_dimes += this.data.dimes;
            this.data.running_nickels += this.data.nickels;
            this.data.running_total += this.data.quarters * 25 + this.data.dimes * 10 + this.data.nickels * 5;
            this.data.quarters = 0;
            this.data.dimes = 0;
            this.data.nickels = 0;
            int D = data.running_total - data.cost;
            int cost_tracker = data.cost;
            Console.Write("200 DLTA " + D);
            
            if (D > 0) 
            {
                this.data.running_total -= this.data.cost;
                if (D >= 0)
                {
                    while (cost_tracker >= 25 && data.running_quarters > 0)
                    {
                        cost_tracker -= 25;
                        data.running_quarters--;
                    }
                    while (cost_tracker >= 10 && data.running_dimes > 0)
                    {
                        cost_tracker -= 10;
                        data.running_dimes--;
                    }
                    while (cost_tracker >= 5 && data.running_nickels > 0)
                    {
                        cost_tracker -= 5;
                        data.running_nickels--;
                    }
                }
                Console.Write(" [RTRN Q" + data.running_quarters + ":D" + data.running_dimes + ":N" + data.running_nickels + "]");
            }
            Console.Write("\r\n");

            if (D >= 0)
            {
                string gumballGotten = this.data.getGumball();
                Console.Write("200 BALL (" + gumballGotten + ")\r\n");
                this.data.zeroTotals();
                if (this.data.totalGumballs == 0)
                {
                    Console.Write("200 ALL BALLS HAVE GONE FROM THIS MACHINE\r\n");
                    return 2;
                }
                return 1;
            }
            return 0;
        }

        private void doAbort()
        {
            // This is suppose to be like returning the change I think.
            Console.Write("200 RTRN Q" + this.data.running_quarters + 
                ":D" + this.data.running_dimes +
                ":N" + this.data.running_nickels + "\r\n");
            this.data.zeroTotals();
        }

        private void doMachineReset()
        {
            this.data = new GumballData();
            Console.Write("200 INVT R" + this.data.gumballs[0] + ":G" + this.data.gumballs[1] +
                ":Y" + this.data.gumballs[2] + ":B" + this.data.gumballs[3] + "\r\n");
            Console.Write("210 Gumball Server Ready to dispense gumballs of value " + this.data.cost + " upon receipt of coin\r\n");

        }

    }

    class ErrorHandler
    {
        private Dictionary<int, string> errorTable;
        public ErrorHandler()
        {
            errorTable = new Dictionary<int, string>();
            errorTable.Add(400, "Syntax Error");
            errorTable.Add(420, "Negative Denomination");
            errorTable.Add(421, "Unregonized Denomination");
            errorTable.Add(422, "Demonination too large");
            errorTable.Add(430, "TURN not allowed");
            errorTable.Add(431, "ABRT not allowed");
            errorTable.Add(432, "BVAL not allowed");
            errorTable.Add(440, "BVAL out of range");
            errorTable.Add(441, "BVAL not multiple of 5");
        }
        public void sendError(int status, string input)
        {
            Console.Write(status.ToString() + " " + errorTable[status] + "\r\n");
            Console.Write("410 " + input + "\r\n");
        }
    }

    class GumballInputParser
    {
        public bool success { get; set; }
        public string input { get; set; }
        public string label { get; set; }
        public (int, int, int) moneyTriple { get; set; }
        public int moneyTotal { get; set; }
        public int value { get; set; }
        private ErrorHandler errorHandler;

        public GumballInputParser(ErrorHandler errorHandler)
        {
            this.errorHandler = errorHandler;
        }

        // Parse parses a command and fills the fields associated with the command 
        // based upon the label given. Returns false if parsing fails due to bad syntax.
        public bool parse(string input)
        {
            this.input = input;
            success = true;
            try
            {
                string[] splitput = input.TrimEnd().Split(' ');
                this.label = splitput[0];
                switch (this.label)
                {
                    case "TURN":
                        break;
                    case "COIN":
                        try
                        {
                            moneyTriple = parseCoins(splitput[1]);
                        }
                        catch (Exception)
                        {
                            this.errorHandler.sendError(421, input);
                            success = false;
                            return false;
                        }
                        moneyTotal = moneyTriple.Item1 + moneyTriple.Item2 + moneyTriple.Item3;
                        break;
                    case "ABRT":
                        break;
                    case "BVAL":
                        value = Int32.Parse(splitput[1]);
                        break;
                    default:
                        throw new Exception();
                }
            }
            catch (Exception)
            {
                this.errorHandler.sendError(400, input);
                success = false;
            }
            return success;
        }

        private (int, int, int) parseCoins(string word)
        {
            string[] numbers = word.Split(':');
            char[] denominations = { 'Q', 'D', 'N' };
            for (int i = 0; i < numbers.Length; i++)
            {
                if (numbers.Length != 0 && numbers[i][0] != denominations[i])
                    throw new Exception();
                numbers[i] = numbers[i].Substring(1, numbers[i].Length-1);
            }
            return (Int32.Parse(numbers[0]), Int32.Parse(numbers[1]), Int32.Parse(numbers[2]));
        }

    }
}
