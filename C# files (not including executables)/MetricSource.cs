public abstract class MetricSource : IDisposable //built in class so i can get rid of unused MetricSource classes
{
    protected MetricOptions _metricName; //should match the options in enum
    protected float _currentValue;
    private int _refreshRate; //in hertz
    private string _unit;
    protected MetricSource(MetricOptions type, int refreshRate = 1)
    {
        _metricName = type;
        _refreshRate = refreshRate;
        switch (type)
        {
            case MetricOptions.CPU:
            case MetricOptions.GPU:
            case MetricOptions.RAM:
                _unit = "%";
                break;
            case MetricOptions.NetDown:
            case MetricOptions.NetUp:
                _unit = "Mbps";
                break;
            case MetricOptions.Latency:
                _unit = "ms";
                break;
        }
    }
    public float GetValue()
    {
        return _currentValue;
    }
    public abstract void UpdateValue();
    public abstract void Dispose(); //this is a garbage collector, has overrides for hardware/network
    public void SetRefreshRate(int rate)
    {
        _refreshRate = rate;
    }
    public int GetRefreshRate()
    {
        return _refreshRate;
    }
    public void SetUnit(string unit)
    {
        _unit = unit;
    }
    public string GetUnit()
    {
        return _unit;
    }
    public MetricOptions GetName()
    {
        return _metricName;
    }
}