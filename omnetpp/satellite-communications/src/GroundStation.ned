
package satellite_communications;

simple GroundStation
{
    parameters:
        int M = default(10);
        int minS = default(1);
        int maxS = default(3616);
        double meanT @unit(ms) = default(10ms);
        volatile double T @unit(ms) = default(exponential(meanT));
        @display("i=device/receiverdish");

    gates:
        input in @directIn;
}
