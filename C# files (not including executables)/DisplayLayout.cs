public abstract class DisplayLayout
{
    protected const int _screenWidth = 16;
    public abstract string[] FormatLayout(List<MetricSource> metrics);
    protected string EnsureFit(string text)
    {
        string formattedResult = "";
        for (int i = 0; i < _screenWidth; i++)
        {
            if (i < text.Length)
            {
                formattedResult += text[i];
            } else
            {
                formattedResult += " ";
            }
        }
        return formattedResult;
    }
}