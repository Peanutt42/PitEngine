using PitEngine;

namespace Sandbox {
    public class Main {
        public void PrintCustomMessage(string message) {
            Debug.Log($"C# says: {message}");
        }
    }
    
    public struct Component {
        public int Integer;
        public float AFloat;
    }

    public class TestSystem : PitEngine.ECS.System {
        public static void Update() {
            Debug.Log("System.Update()");
        }
    }
}