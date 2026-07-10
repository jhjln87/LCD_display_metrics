public class SpecificLayout : DisplayLayout
{
    
    protected string _unit;
    protected MetricOptions _type;
    private string _dispUnit;
    private float _maxVal;
    public SpecificLayout(MetricSource metric)
    {
        _type = metric.GetName();
        _unit = metric.GetUnit();
        switch (_type)
        {
            case MetricOptions.NetDown:
                _dispUnit = "DNLD";
                _maxVal = 100;
                break;
            case MetricOptions.NetUp:
                _dispUnit = "UPLD";
                _maxVal = 30;
                break;
            case MetricOptions.Latency:
                _dispUnit = "LAT";
                _maxVal = 60;
                break;
            default:
                _dispUnit = _type.ToString();
                _maxVal = 100;
                break;
        }
    }
    public float GetPercentage(float num)
    {
        if (num > _maxVal)
            {
                _maxVal = num;
            }
        return num/_maxVal;
    }
    public override string[] FormatLayout(List<MetricSource> metrics)
    {
        float progressVal = 0f;
        foreach (MetricSource metric in metrics)
        {
            if (metric.GetName() == _type)
            {
                progressVal = metric.GetValue();
            }
        }
        int bars;
        if (progressVal < 0)
        {
            bars = (int)(GetPercentage(0) * (_screenWidth - 2));
        } else
        {
            bars = (int)(GetPercentage(progressVal) * (_screenWidth-2));
        }
        string[] final = new string[2];
        final[0] = "[";
        for (int i = 0; i < (_screenWidth-2); i++)
        {
            if (i < bars)
            {
                final[0] += "#";
            }
            else
            {
                final[0] += " ";
            }
        }
        final[0] += "]";
        final[0] = EnsureFit(final[0]);
        final[1] = EnsureFit($"{_dispUnit}: {progressVal}{_unit}");
        return final;
    }
}