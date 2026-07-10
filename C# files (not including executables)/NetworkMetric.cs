using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Net.NetworkInformation;

public class NetworkMetric : MetricSource
{
    private List<PerformanceCounter> _networkCounters = new List<PerformanceCounter>();
    private double _bps;
    private Task<PingReply> _pingTask;
    public NetworkMetric(MetricOptions type, int refreshRate = 1) : base(type, refreshRate)
    {
        InitNet(type);
    }
    private void InitNet(MetricOptions type)
    {
        try
        {
            string pollMethod;
            switch (type)
            {
                case MetricOptions.NetDown:
                    pollMethod = "Bytes Received/sec";
                    break;
                case MetricOptions.NetUp:
                    pollMethod = "Bytes Sent/sec";
                    break;
                default:
                    return;
            }

            var category = new PerformanceCounterCategory("Network Interface");
            var instances = category.GetInstanceNames();
            foreach (var instance in instances)
            {
                if (!instance.Contains("Loopback") && !instance.Contains("Tunneling"))
                { //filter for just network stuff
                    var counter = new PerformanceCounter("Network Interface", pollMethod, instance);
                    counter.NextValue(); // init poller
                    _networkCounters.Add(counter);
                }
            }
        }
        catch (Exception)
        { //if any object aint working just ignore it because is list
        }
    }
    public override void UpdateValue()
    {
        switch (_metricName)
        {
            case MetricOptions.NetDown:
            case MetricOptions.NetUp:
                if (_networkCounters != null && _networkCounters.Count > 0)
                {
                    float bps = 0;
                    foreach (var counter in _networkCounters)
                    {
                        try
                        {
                            bps += counter.NextValue();
                        }
                        catch
                        { // if fail, ignore
                        }
                    }
                    _currentValue = (float)Math.Round(bps / 125000.0, 2);
                }
                break;
            case MetricOptions.Latency: //this one was a bit more difficult. some of this is copy pasted from internet
                if (_pingTask == null || _pingTask.IsCompleted) //ping is async so user can exit whenever. needs to check if ping task is already running
                {
                    if (_pingTask != null && _pingTask.IsCompletedSuccessfully && _pingTask.Result.Status == IPStatus.Success) // this is just a failsafe for if the next ping fails
                    {
                        _currentValue = (float)_pingTask.Result.RoundtripTime;
                    } else if (_pingTask != null && _pingTask.IsCompleted)
                    {
                        _currentValue = -1.0f;
                    }

                    try
                    {
                        Ping pingSender = new Ping(); 
                        _pingTask = pingSender.SendPingAsync("8.8.8.8", 1000).ContinueWith(t => {
                            pingSender.Dispose();
                            return t.Result;
                        });
                    } catch
                    {
                        _currentValue = -1.0f;
                    }
                }
                break;
        }
    }
    public override void Dispose()
    {
        if (_networkCounters != null)
        {
            foreach (var counter in _networkCounters)
            {
                counter.Dispose();
            }
            _networkCounters.Clear();
        }
    }
}