using System;
using System.Diagnostics;
using System.Threading;
using System.Collections.Generic;

public class HardwareMetric : MetricSource
{
    int _maxLogicalCores = Environment.ProcessorCount;
    private PerformanceCounter _cpuCounter;
    private PerformanceCounter _ramCounter;
    private List<PerformanceCounter> _gpuCounters = new List<PerformanceCounter>();
    private float _percentage;
    public HardwareMetric(MetricOptions type, int refreshRate = 1) :base(type, refreshRate)
    {
        switch (type)
        {
            case MetricOptions.CPU: // % of CPU used
                _cpuCounter = new PerformanceCounter("Processor", "% Processor Time", "_Total");
                _cpuCounter.NextValue(); // init poller
                break;

            case MetricOptions.RAM: // % of RAM used
                _ramCounter = new PerformanceCounter("Memory", "% Committed Bytes In Use");
                _ramCounter.NextValue(); //init poller
                break;

            case MetricOptions.GPU:
                InitializeGpuCounters(); //gpu is its own entire beast. lets give it its own method. maybe couldve had its own class but im too stubborn to do that.
                break;
        }
    }
    private void InitializeGpuCounters()
    {
        try
        {
            var category = new PerformanceCounterCategory("GPU Engine"); //gpu is a beast, not just one thing. needs its own object man
            var instances = category.GetInstanceNames(); //get everything
            foreach (var instance in instances)
            {
                if (instance.Contains("engtype_3D"))
                {
                    var counter = new PerformanceCounter("GPU Engine", "Utilization Percentage", instance);
                    counter.NextValue(); //finally ready to init poller
                    _gpuCounters.Add(counter); //all values will be averaged
                }
            }
        }
        catch (Exception) //if any object aint working just ignore it
        {
        }
    }
    public float GetPercent()
    {
        return _percentage;
    }
    public override void UpdateValue()
    {
        switch (_metricName)
        {
            case MetricOptions.CPU:
                if (_cpuCounter != null) //make sure its not empty
                {
                    _percentage = _currentValue = (float)Math.Round(_cpuCounter.NextValue(), 1);
                }
                break;

            case MetricOptions.RAM:
                if (_ramCounter != null) //make sure its not empty
                {
                    _percentage = (float)Math.Round(_ramCounter.NextValue(), 1);
                    _currentValue = _percentage;
                }
                break;
            case MetricOptions.GPU:
                if (_gpuCounters.Count > 0) //make sure its not empty
                {
                    float totalGpuUsage = 0;
                    foreach (var counter in _gpuCounters)
                    {
                        try
                        {
                            totalGpuUsage += counter.NextValue();
                        }
                        catch //if any object aint working just ignore it
                        {
                        }
                    }
                    _percentage = (float)Math.Round(Math.Min(totalGpuUsage, 100), 1); //cap at 100 if is weird
                    _currentValue = _percentage; //value is just percentage
                }
                break;
            default:
                _currentValue = -1.0f; //send error
                break;
        }
    }
    public override void Dispose()
    {
        _cpuCounter?.Dispose();
        _ramCounter?.Dispose();
        if (_gpuCounters != null)
        {
            foreach (var counter in _gpuCounters)
            {
                counter.Dispose();
            }
            _gpuCounters.Clear();
        }
    }
}