using System;
using System.Runtime.CompilerServices;

namespace PitEngine {
    internal static class InternalCalls {

        #region Logging

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Log(string msg);

        #endregion
    }

    public static class Debug {
        public static void Log(string msg) => InternalCalls.Log(msg);
        public static void Log(string msg, Vector3 vec3) => InternalCalls.Log(msg + $"({vec3.X}, {vec3.Y}, {vec3.Z})");
    }
}
