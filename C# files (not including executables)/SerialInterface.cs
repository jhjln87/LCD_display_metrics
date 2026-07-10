using System;
using System.IO.Ports;
using System.Threading;
using System.Collections.Generic;

public class SerialInterface
{
    private SerialPort _serialPort;
    private bool _running = true;
    public SerialInterface(string portName = "COM3", int baudRate = 9600)
    {
        _serialPort = new SerialPort(portName, baudRate, Parity.None, 8, StopBits.One); //windows built-in class for serial uart comms
        _serialPort.Handshake = Handshake.None; // its asynchronous so no need a handshake
        
        _serialPort.ReadTimeout = 500; //if nothing for half second then timeout
        _serialPort.WriteTimeout = 500;
    }
    public void SendData(string[] thing)
    {
        if (!_serialPort.IsOpen)
        {
            return;
        }
        try
        {
            _serialPort.Write(thing[0] + thing[1]); //serial write one thing, \n is the splitter
        }
        catch (Exception ex)
        {
            Console.WriteLine($"\nError: Failed to transmit data. {ex.Message}");
        }
    }
    public void Menu()
    {
        try
        {
            _serialPort.Open();
            Console.WriteLine($"Successfully connected to {_serialPort.PortName}!");
            Thread.Sleep(1500);
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error opening {_serialPort.PortName}: {ex.Message}\nPress any key to exit and check your device manager/CubeIDE settings.");
            Console.ReadKey();
            return;
        }
        StartMainLoop();
    }
    public void StartMainLoop() {
        while (_running)
        {
            Console.Clear();
            Console.Write("--- Hardware & Network Monitor Menu ---\n1. Poll CPU Usage\n2. Poll RAM Usage\n3. Poll GPU Usage\n4. Poll Network Download Speed\n5. Poll Network Upload Speed\n6. Poll Network Latency (Ping)\n7. Exit\nSelect an option (1-7): ");
            
            List<MetricSource> defaultMetrics = new List<MetricSource>
            {
                new HardwareMetric(MetricOptions.CPU, 1),
                new HardwareMetric(MetricOptions.RAM, 1),
                new NetworkMetric(MetricOptions.Latency, 1)
            };
            DisplayLayout defaultLayout = new GeneralLayout();
            
            int select;
            while (!Console.KeyAvailable)
            {
                foreach (var metric in defaultMetrics)
                {
                    metric.UpdateValue();
                }
                SendData(defaultLayout.FormatLayout(defaultMetrics));
                Thread.Sleep(1000); 
            }
            foreach (var metric in defaultMetrics) 
            { 
                metric.Dispose(); 
            }
            ConsoleKeyInfo keyInfo = Console.ReadKey(true);
            select = (int)char.GetNumericValue(keyInfo.KeyChar);
            
            switch (select)
            {
                case 1:
                    StartPoll(MetricOptions.CPU, 1, false);
                    break;
                case 2:
                    StartPoll(MetricOptions.RAM, 1, false);
                    break;
                case 3:
                    StartPoll(MetricOptions.GPU, 1, false);
                    break;
                case 4:
                    StartPoll(MetricOptions.NetDown, 1, false);
                    break;
                case 5:
                    StartPoll(MetricOptions.NetUp, 1, false);
                    break;
                case 6:
                    StartPoll(MetricOptions.Latency, 1, false);
                    break;
                case 7:
                    _running = false;
                    break;
                default:
                    Console.WriteLine("\nInvalid option. Please try again.\nPress any key to continue.");
                    Console.ReadKey();
                    break;
            }
        }
        if (_serialPort.IsOpen)
        {
            _serialPort.Close();
        }
        Console.WriteLine("Exiting program...");
    }
    private void StartPoll(MetricOptions option, int refreshRate, bool useGeneralLayout)
    {
        Console.Clear();
        Console.WriteLine($"--- Transmission Active (Press 'ESC' to return to menu) ---\nStreaming data over to {_serialPort.PortName}...");
        
        MetricSource metric;
        if (option == MetricOptions.CPU || option == MetricOptions.RAM || option == MetricOptions.GPU)
        {
            metric = new HardwareMetric(option, refreshRate);
        } else
        {
            metric = new NetworkMetric(option, refreshRate);
        }
        DisplayLayout layout = new SpecificLayout(metric);

        List<MetricSource> metricsToPoll = new List<MetricSource> {metric};
        
        while (!(Console.KeyAvailable && Console.ReadKey(true).Key == ConsoleKey.Escape))
        {
            metric.UpdateValue();
            SendData(layout.FormatLayout(metricsToPoll));
            Thread.Sleep(1000/refreshRate);
        }
        metric.Dispose();
    }
}