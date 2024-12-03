
package satellite_communications;

simple Terminal 
{
	parameters:
	   volatile int codingRate = default(intuniform(0,6));
	   @display("i=device/cellphone2");
	   
	gates:
	    input in @directIn;
}