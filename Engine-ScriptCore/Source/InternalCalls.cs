using System;
using System.Runtime.CompilerServices;

namespace Pit {
    internal static class InternalCalls {

        #region Logging

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Log(string msg);

        #endregion
    }
}
