
package satellite_communications;

module GroundStation
{
    parameters:
        @display("i=device/receiverdish");

    gates:
        input in @directIn;
        
    submodules:
        packetGenerator[parent.N]: PacketGenerator;
        packetScheduler: PacketScheduler;
        
    connections:
        in --> packetScheduler.in;
        for i = 0 .. parent.N-1 
        {
	        packetGenerator[i].out --> packetScheduler.generatorsIn++;
        }
}
