
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
        );
        @signal[instantaneousThroughput](type=double);
        @statistic[instantaneousThroughputStat]
        (
        	source = instantaneousThroughput;
        	record = vector;
        );
        @display("i=block/join");
        @signal[frameUtilization](type=double);
        @statistic[frameUtilizationStat]
        (
        	source = frameUtilization;
        	record = mean, vector;
        );
        @display("i=block/join");
        
    gates:
        input generatorsIn[];
        input groundStationIn;
}
