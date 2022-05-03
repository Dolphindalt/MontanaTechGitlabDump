using System;
using System.Collections.Generic;
using System.Linq;

namespace FECClient
{

    struct AggregateCorrectionResult
    {
        public ushort decodedCorrectedMessage;
        public long correctedMessage;
        public List<byte> retransmittedByteNumbers;
    }

    struct CorrectionResult
    {
        public bool retransmit;
        public byte correctedByte;
    }

    class ErrorCorrector
    {
        private Dictionary<byte, byte> codeWordDict; // binary pairs -> code
        private Dictionary<byte, byte> inverseCodeWordDict; // code -> binary pairs

        public ErrorCorrector(string[] codeWordTable)
        {
            // Will be 4 all the time but we should assert it.
            int tableLength = codeWordTable.Length;
            if (tableLength != 4)
            {
                throw new System.Exception("Error correcting table should be length of 4 (total binary pairs)");
            }
            this.ConstructCodeWordDictionary(codeWordTable);
        }

        public AggregateCorrectionResult forwardErrorCorrection(long recWord)
        {
            AggregateCorrectionResult ret = new AggregateCorrectionResult();
            ret.retransmittedByteNumbers = new List<byte>();
            byte[] bytesValues = this.ExtractBytesFromLong(recWord);
            for (byte i = 0; i < 8; i++) // 8 bytes per long.
            {
                byte recByte = bytesValues[i];
                CorrectionResult result = this.correctByteError(recByte);
                if (result.retransmit)
                    ret.retransmittedByteNumbers.Add((byte)(7-i)); // We are backwards.
                bytesValues[i] = result.correctedByte;
            }
            ret.correctedMessage = BitConverter.ToInt64(bytesValues.Reverse().ToArray());
            ret.decodedCorrectedMessage = decodeMessage(ret.correctedMessage);
            return ret;
        }

        private ushort decodeMessage(long code)
        {
            ushort result = 0;
            byte[] bytesValues = this.ExtractBytesFromLong(code);
            for (byte i = 0; i < 8; i++)
            {
                result <<= 2;
                try
                {
                    result |= inverseCodeWordDict[bytesValues[i]];
                }
                catch (Exception)
                {
                    // If we arrive here, error correcting failed due to duplicated 
                    // minimum Hamming distances, so we would retransmit in real life.
                    // The show must go on. I will just keep zero values for these.
                }
            }
            return result;
        }

        private CorrectionResult correctByteError(byte recByte)
        {
            CorrectionResult result = new CorrectionResult();
            result.correctedByte = recByte;
            result.retransmit = false;
            byte min = 255;
            byte min_byte_n = 0;
            bool duplicate_min_dist = false;
            for (byte i = 0; i < 4; i++)
            {
                byte dist = this.HammingDistance(this.codeWordDict[i], recByte);

                // Check for a new minimum distance to select.
                if (dist < min)
                {
                    min = dist;
                    min_byte_n = i;
                    duplicate_min_dist = false;
                }
                // Check if the minimum distance is duplicated.
				else if (dist == min)
                {
                    duplicate_min_dist = true;
                }
            }

            if (min == 0) // Case 1: code word is directly decoded into two-bit sequence.
            {
                result.correctedByte = recByte;
            }
            else if (!duplicate_min_dist) // Case 2: We can select a single codeword based on min hamming distance.
            {
                result.correctedByte = this.codeWordDict[min_byte_n];
            }
            else if (duplicate_min_dist) // Case 3: We need to retransmit in the case of a tie.
            {
                result.retransmit = true;
            }
            // We can't really detect Case 4.
            return result;
        }

        private byte HammingDistance(byte lh, byte rh)
        {
            byte distance = 0;
            for (byte i = 0; i < 8; i++)
                if ((lh & (1<<i)) != (rh & (1<<i)))
                    distance++;
            return distance;
        }

        private byte[] ExtractBytesFromLong(long lval)
        {
            byte[] byteValues = new byte[8];
            for (int i = 7, j = 0; i >= 0; i--, j++)
            {
                byteValues[j] = ((byte) ((lval >> (i*8)) & 0xFF));
            }
            return byteValues;
        }

        private void ConstructCodeWordDictionary(string[] codeWordTable)
        {
            this.codeWordDict = new Dictionary<byte, byte>();
            this.inverseCodeWordDict = new Dictionary<byte, byte>();
            for (byte i = 0; i < 4; i++)
            {
                byte codeWord = System.Byte.Parse(codeWordTable[i].Split(' ')[1]);
                codeWordDict.Add(i, codeWord);
                inverseCodeWordDict.Add(codeWord, i);
            }
        }

    }

}
