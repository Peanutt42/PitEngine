using System;

namespace Pit.Scripting {
    public class Main {
        public float FloatVar { get; set; }
        public Main() {
            Console.WriteLine("Main Constructor!");
        }
        public void PrintMessage() {
            Console.WriteLine("Hello World from C#!");
        }
        public void PrintIntMessage(int value) {
            Console.WriteLine($"C# says: {value}");
        }
        public void PrintIntsMessage(int value1, int value2) {
            Console.WriteLine($"C# says: {value1}, {value2}");
        }
        public void PrintCustomMessage(string message) {
            Console.WriteLine($"C# says: {message}");
        }
    }
}