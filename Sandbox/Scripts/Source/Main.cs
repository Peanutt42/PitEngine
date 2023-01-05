using PitEngine;

namespace Sandbox {
    public class Main {
        public void PrintCustomMessage(string message) {
            Debug.Log($"C# says: {message}");
        }
    }
    
    public struct Component {

    }

    public class TestSystem : PitEngine.ECS.System {
        public void Update() {
            Debug.Log("System.Update()");
        }
    }
}