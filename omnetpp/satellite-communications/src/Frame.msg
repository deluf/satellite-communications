
import CodingRatePacket;

class Block extends cObject
{
    int maxSize;
    int usedSize;
 	CODING_RATE codingRate;
    Packet *packets[];
};

packet Frame
{
    Block blocks[];
};
