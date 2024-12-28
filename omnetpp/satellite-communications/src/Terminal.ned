
package satellite_communications;

simple Terminal 
{
	parameters:
	    string codingRateDistribution = default("uniform");
		@signal[delay](type = double);
		@statistic[delayStat](
	   		source = delay;
	   		record = mean, vector;
	   		title = "Delay";
	   		unit = "ms";
		);
		@signal[debugCodingRateDistribution](type = int);
		@statistic[debugCodingRateDistributionStat](
	   		source = debugCodingRateDistribution;
	   		record = histogram;
	   		title = "Coding rate distribution";
		);
		@display("i=device/cellphone2");
	   
	gates:
	    input in @directIn;
}