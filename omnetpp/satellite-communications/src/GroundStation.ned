
package satellite_communications;

simple GroundStation
{
    parameters:
        int M = default(10);
        int minS @unit(byte) = default(1 byte);
        int maxS @unit(byte) = default(3616 byte);
        volatile int S @unit(byte) = default(intuniform(minS, maxS));
        double meanT @unit(ms) = default(40 ms);
        volatile double T @unit(s) = default(exponential(meanT));
        @signal[throughput](type=long);
	    @statistic[throughputStat](
	    	source=sumPerDuration(throughput);
	    	record=last, vector;
	    	unit="bps";
	    ); // TODO: vector metric only collected for debugging purposes
        @display("i=device/receiverdish");

    gates:
        input in @directIn;
}
