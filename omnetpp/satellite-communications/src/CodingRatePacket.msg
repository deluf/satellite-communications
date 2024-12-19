
import Packet;

cplusplus
{{
static const char *codingRateToString[7] = 
	{ "L3", "L2", "L1", "R", "H1", "H2", "H3" };
}}

enum CODING_RATE 
{
    L3 = 0;
    L2 = 1;
    L1 = 2;
    R  = 3;
    H1 = 4;
    H2 = 5;
    H3 = 6;
};

packet CodingRatePacket extends Packet  
{
    CODING_RATE codingRate;
};
