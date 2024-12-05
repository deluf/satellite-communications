
package satellite_communications;

simple PacketGenerator
{
    parameters:
        // TODO: Maybe find a better name for minS, maxS, meanT (e.g. minPacketSize, meanPacketArrivalTime)
        int minS @unit(byte) = default(1 byte);
        int maxS @unit(byte) = default(3616 byte);
        double meanT @unit(s) = default(40 ms);
        @display("i=misc/globe");
        
    gates:
        output out;
}
