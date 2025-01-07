
package satellite_communications;

simple PacketGenerator
{
    parameters:
        int minPacketSize @unit(byte) = default(1 byte);
        int maxPacketSize @unit(byte) = default(500 byte);
        double meanPacketInterarrivalTime @unit(s) = default(40 ms);
       	@signal[debugArrivalTime](type = double);
        @statistic[debugArrivalTimeStat](
	   		source = debugArrivalTime;
	   		record = mean, vector;
	   		title = "Within-slot arrival time distribution";
	   		unit = "ms";
		);
        @display("i=misc/globe");
        
    gates:
        output out;
}
