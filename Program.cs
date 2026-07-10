using System;

/* 
README before using project
Need STM32NUCLEO-L476RG, QAPASS LCM1602A, x4 Male-to-Female wires
To run this on your computer: Must be windows! Must have STM32CubeMX properly setup and running. CubeMX is recommended, although user can ask me for all the file (many folders)
Network upload and download is not your wifi, it is what you are currently using, go to youtube or other heavy website to get a good number
All hardware metrics are what you are currently using
Default is CPU, RAM, Latency. Other options are chosen by keypress
*/
class Program
{
    static void Main(string[] args)
    {
        if (!OperatingSystem.IsWindows()) //some of my things only work on windows
        {
            Console.WriteLine("This application only works on Windows!");
            return;
        }

        SerialInterface serialMonitor = new SerialInterface("COM4", 9600); //make new interface class. change "COM" to whatever port you have
        serialMonitor.Menu(); //boom
        // (serialMonitor handles a lot of it)
    }
}