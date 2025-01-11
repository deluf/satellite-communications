
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
        @signal[frameUtilization](type=double);
        @statistic[frameUtilizationStat]
        (
        	source = frameUtilization;
        	record = mean, vector;
        );
        @signal[debugInstantaneousThroughput](type=double);
        @statistic[debugInstantaneousThroughputStat]
        (
        	source = debugInstantaneousThroughput;
        	record = vector?;
        );
        @signal[debugFrameSize](type=long);
        @statistic[debugFrameSizeStat]
        (
        	source = debugFrameSize;
        	record = mean?, vector?;
        );
        @display("i=block/join");
        
    gates:
        input generatorsIn[];
        input groundStationIn;
}
