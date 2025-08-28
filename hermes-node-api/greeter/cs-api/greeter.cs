using Microsoft.JavaScript.NodeApi;

public static class Greeter
{
    [JSExport]
    public static string SayHello(string name)
    {
        return $"C#: Hello, {name}!";
    }
}
