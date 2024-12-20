
import CodingRatePacket;

class Block extends cObject
{
    int maxBytes;
    int usedBytes;
 	CODING_RATE codingRate;
    Packet *packets[];
};

packet Frame
{
    Block blocks[];
};
