using System;

namespace Pit {
    public struct Vector3 {
        public float X, Y, Z;

        public Vector3(float x, float y, float z) {
            X = x;
            Y = y;
            Z = z;
        }
    }
       

    public class Entity {
        public static void Log(string msg) => InternalCalls.Log(msg);
        public static void Log(string msg, Vector3 vec3) => InternalCalls.Log(msg + $"({vec3.X}, {vec3.Y}, {vec3.Z})");


        public float FloatVar { get; set; }
        public Entity() {
            Console.WriteLine("Main Constructor!");
            Log("Hi from C#!");
            Vector3 vec3 = new Vector3(1.4f, 2.23f, 3.124f);
            Log("Hi from C#! With a vec3", vec3);
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

    public class TestEntity : Entity {

        public float Speed;
        public TestEntity() {

        }
    }
}