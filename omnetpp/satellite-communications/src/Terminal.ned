
package satellite_communications;

simple Terminal 
{
	parameters:
	    bool codingRateUniformDistributed = default(true);
		volatile int codingRate = default(intuniform(0,6));
		@signal[delay](type = double);
		@statistic[delayStat](
	   		source = delay;
	   		record = mean, vector;
	   		title = "Delay";
	   		unit = "ms";
		);
		@display("i=device/cellphone2");
	   
	gates:
	    input in @directIn;
}