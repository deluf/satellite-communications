
package satellite_communications;

simple PacketScheduler
{
    parameters:
        // TODO: Maybe find a better name for M (e.g. blocksPerFrame)
        int M = default(10);
        @signal[throughput](type=long);
        @statistic[throughputStat]
        (
        	source=sumPerDuration(throughput);
        	record=last,vector;
        	unit="bps"; 
        ); // TODO: vector metric only collected for debugging purposes

        @display("i=block/join");
        
    gates:
        input generatorsIn[];
        input in;
}
