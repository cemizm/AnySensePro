using System;

namespace MavLink
{
    public abstract class BaseMavlinkLayer : Mavlink, IDisposable
    {
        public abstract void SendMessage(MavlinkMessage msg);

        public abstract void Dispose();
    }
}
