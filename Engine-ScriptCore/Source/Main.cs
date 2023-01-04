using Pit.Engine;

namespace Pit {
    public class Main {
        public void PrintCustomMessage(string message) {
            Debug.Log($"C# says: {message}");
        }
    }

    public class Component {
        public Component() { }
        ~Component() { }
    }

    public class System {
        public void Update() {
            Debug.Log("System.Update()");
        }
    }
}