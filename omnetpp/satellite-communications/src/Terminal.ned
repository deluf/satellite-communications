
package satellite_communications;

simple Terminal 
{
	parameters:
	   volatile int codingRateIndex = default(intuniform(0,6)); 
	   @display("i=device/cellphone2");
	   
	gates:
	    input in @directIn;
}