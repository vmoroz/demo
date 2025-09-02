using Microsoft.JavaScript.NodeApi;

public static partial class Greeter
{
    [JSExport]
    public static string SayHello(string name)
    {
        return $"C# API: Hello, {name}!";
    }
}
