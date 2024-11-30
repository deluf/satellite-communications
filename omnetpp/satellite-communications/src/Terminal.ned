
package satellite_communications;

simple Terminal {
	parameters:
	   int CR = default(intuniform(0,6)); 
	   @display("i=device/cellphone2");
	   
	gates:
	    input in @directIn;
}