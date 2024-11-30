
package satellite_communications;

simple GroundStation
{
    parameters:
        int M = default(10);
        int minS = default(1);
        int maxS = default(3616);
        double meanT = default(10); // cosa vuol dire unit? meanT sarebbe 1/lambda
        volatile double T @unit(ms) = default(exponential(1/meanT));
        @display("i=device/receiverdish");

    gates:
        input in @directIn;
}
