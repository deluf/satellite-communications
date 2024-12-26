
package satellite_communications;

simple PacketScheduler
{
    parameters:
        int blocksPerFrame = default(4);
        @signal[throughput](type=double);
        @statistic[throughputStat]
        (
        	source = throughput;
        	record = last, vector;
        	title = "Throughput";
        	unit = "bps"; 
        );
        @signal[instantaneousThroughput](type=double);
        @statistic[instantaneousThroughputStat]
        (
        	source = instantaneousThroughput;
        	record = mean, vector;
        	title = "Instantaneous throughput";
        	unit = "bps"; 
        );
        @display("i=block/join");
        
    gates:
        input generatorsIn[];
        input groundStationIn;
}
