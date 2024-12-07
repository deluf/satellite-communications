import TerminalPacket;
import CodingRateMessage;

message Block
{
    CODING_RATE codingRate;
    int maxSize;
    int usedSize;
    TerminalPacket packets[]; 
}
