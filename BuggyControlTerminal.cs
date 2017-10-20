using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;


namespace ConsoleApplication2
{
    class Program
    {

        public static List<string> theList = new List<string>(); //declaring a list. This will be the list that will hold the data like "g1b2"
        //static bool received = false;
        static public SerialPort serial1 = new SerialPort();
        //static bool first = false;
        static public string ready = "";
        static public string cut = "";
        static public int splitterCount = 0;
        static public bool adding = false;
        static void Main(string[] args)
        {

            serial1.BaudRate = 9600;
            serial1.PortName = "COM14";
            serial1.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);
            serial1.Open();
            serial1.Write("+++");
            System.Threading.Thread.Sleep(1100);
            serial1.WriteLine("ATID 6869, CH C, CN");
            for (int i = 10; i > 0; i--)
            {
                System.Threading.Thread.Sleep(1000);
                Console.WriteLine(i);
            }

            int initialStart = 0;
            Console.WriteLine("Type 1 to start");
            initialStart = Convert.ToInt32(Console.ReadLine());
            Console.WriteLine("Accepted input as " + initialStart);
            bool completedOutput = false;
            // bool gantry1Full = false;//For silver and gold only. if there is a buggy between g1 and g2 then this will be true, which means the second buggy will know to wait at g1 until the second buggy has moved past g2.
            // bool gantry2Full = false;
            //  bool gantry3Full = false;
            bool finalLap = false;//the track is different on the final lap (buggies need to fork left at the junction and park)
            string input = "";
            //int count = 0;
            // bool bronzeLap = false;//only for bronze, we need to do only one lap and then stop when the buggy returns to the first gantry.
            //int receivedCounter = 0;
            // serial1.WriteLine("b1bg");//telling buggy to begin the loop

            while (!completedOutput)
            // while (lapnumber < 1)
            {
                if (initialStart == 1)
                {

                    serial1.WriteLine("b1bg");//telling buggy to begin the loop
                    // received = false;
                    initialStart = 0;
                }
                //need to read from the bottom and then delete the bottom data
                if (theList.Count != 0)//if (theList has stuff in it do all this code)
                {
                    input = theList.ElementAt(0);//read from bottom of list
                    theList.RemoveAt(0);//remove command from bottom of list   

                    char gantry = '0';
                    char buggy = '0';

                    if (input[0] == 'b' && input[2] == 'g' && input[3] != 'o')
                    {

                        gantry = input[3];
                        buggy = input[1];
                        Console.WriteLine("\nBuggy " + buggy + " is at gantry " + gantry + "\n");
                        switch (gantry)//instead of "if" statement for when buggy is at gantry (it makes it more clear to look at)
                        {

                            case '1':


                                if (!finalLap)//this is only for silver or gold, but the idea is that the buggy will follow the code in the else section on the final lap, which means that it will need to turn left and park.
                                {
                                    serial1.WriteLine("b" + buggy + "go");
                                }
                                else
                                {
                                    serial1.WriteLine("b1pk");//telling buggy to use right overide
                                }
                                break; //There is a break at the end of each case so that the program won't bother going through all the other cases in the switch statement, making it more efficient

                            case '2':
                                serial1.WriteLine("b" + buggy + "go");

                                break;

                            case '3':
                                serial1.WriteLine("b" + buggy + "go");
                                finalLap = true;
                                
                                break;
                        }
                    }
                    if (input[2] == 's' && input[3] == 't')
                    {
                        Console.WriteLine("Buggy will remain at a stop");
                    }
                    if (input[2] == 'b' && input[3] == 'g') //if buggy tells us it is beggining
                    {
                        Console.WriteLine("Buggy has started its lap");
                    }
                    if (input[2] == 'm' && input[3] == 'o')//buggy is telling us it is now moving
                    {
                        Console.WriteLine("Buggy is now moving away from gantry");
                    }
                    if (input[2] == 'o' && input[3] == 'b')//if buggy is at an obstacle (for the ultrasonics, not to worry about yet!
                    {
                        buggy = input[1];
                        Console.WriteLine("BUGGY " + buggy + " IS AT AN OBSTACLE");//pretty self-explanatory...
                    }
                    if (input[2] == 'c' && input[3] == 'l')//obstacle has been cleared
                    {
                        buggy = input[1];
                        serial1.WriteLine("b" + buggy + "fg");//command buggy to go again
                        Console.WriteLine("Obstacle cleared, buggy " + buggy + " is moving again.");

                    }
                  
                    if (input[2] == 'p' && input[3] == 'k')//obstacle has been cleared
                    {
                        Console.WriteLine("buggy has parked. Task completed.");
                        completedOutput = true;
                    }
                }
               
            }

        }
        





        private static void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e) //The bulk of this event handler was found on the internet
        {
            
            SerialPort sp = (SerialPort)sender;
            string indata = sp.ReadExisting();
            Splitter(indata);
        }

        private static void Splitter(string s)
        {
            foreach (char c in s)
            {
                if (c == 'b')
                {
                    adding = true;
                }


                if (adding)
                {
                    cut += c.ToString();
                    //splitterCount++;
                    if (cut.Length == 4)
                    {
                        // splitterCount = 0;
                        theList.Add(cut);
                        //Console.WriteLine("debug: " + cut);
                        cut = "";
                        adding = false;
                    }
                }
            }

        }
    }
}