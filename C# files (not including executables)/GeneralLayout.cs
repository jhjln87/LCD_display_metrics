public class GeneralLayout : DisplayLayout
{
    public override string[] FormatLayout(List<MetricSource> metrics)
    {
        string cpuOut = "0%";
        string ramOut = "0%";
        string latOut = "0ms";
        foreach(MetricSource met in metrics)
        {
            switch(met.GetName())
            {
                case MetricOptions.CPU:
                    cpuOut = ((int)met.GetValue()) + "%";
                    break;
                case MetricOptions.RAM:
                    ramOut = ((int)met.GetValue()) + "%";
                    break;
                case MetricOptions.Latency:
                    latOut = ((int)met.GetValue()) + "ms";
                    break;
            }
        }
        string[] final = new string[2];
        final[0] = EnsureFit($"CPU{cpuOut} RAM{ramOut}");
        final[1] = EnsureFit($"LATENCY {latOut}");
        return final;
    }
}