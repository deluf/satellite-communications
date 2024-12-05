
package satellite_communications;

simple Terminal 
{
	parameters:
	   volatile int codingRate = default(intuniform(0,6));
	   @signal[delay](type=double);
	   @statistic[delayStat](
	   		source=delay;
	   		record=mean, vector;
	   		unit="ms";
	   ); // TODO: vector metric only collected for debugging purposes
	   @display("i=device/cellphone2");
	   
	gates:
	    input in @directIn;
}