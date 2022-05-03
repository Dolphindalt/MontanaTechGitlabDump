using System;
using System.Linq;
/**
* Forward Error Correction 
* Client Program - 
* - connect to endpoint (csdept10.cs.mtech.edu, 30120)
* - obtain the data from the server and correct the errored data
*
* CSCI 466 Networks
*
* Phillip J. Curtiss, Associate Professor
* pcurtiss@mtech.edu, 406-496-4807
* Department of Computer Science, Montana Tech
*/

namespace FECClient
{
    class Program
    {
        static void Main(string[] args)
        {
            string[] readContent = new InfoGrabber("csdept10.cs.mtech.edu", 30120).readContent;
            string[] codeWordTable = readContent.Take(4).ToArray();
            string[] messages = readContent.Skip(5).ToArray();

            ErrorCorrector ec = new ErrorCorrector(codeWordTable);
            foreach (string pair in messages)
            {
                string[] split = pair.Split(' ');
                short expectedByte = Convert.ToInt16(split[0], 16);
                long actualReceived = Convert.ToInt64(split[1], 16);
                Console.Write("Original 2-byte number: {0}\r\nResult: ", split[0]);
                AggregateCorrectionResult result = ec.forwardErrorCorrection(actualReceived);
                // Case 2: We need to retransmit.
                if (result.retransmittedByteNumbers.Capacity != 0)
                {
                    Console.Write("Bytes to retransmit: ");
                    foreach (byte index in result.retransmittedByteNumbers)
                    {
                        Console.Write("{0} ", index);
                    }
                    Console.WriteLine();
                }
                // Case 3: Something went wrong. Must be detected in upper layers.
                else if (result.decodedCorrectedMessage != expectedByte)
                {
                    Console.WriteLine("Error will be propagated to the higher network layers");
                }
                else // We did it I suppose
                {
                    Console.WriteLine("The original 2-byte number: 0x{0:X}", result.decodedCorrectedMessage);
                }
                Console.WriteLine();
            }
        }
    }

}
