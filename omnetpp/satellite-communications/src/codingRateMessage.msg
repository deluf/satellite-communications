
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

message CodingRateMessage 
{
    int terminalId;
    CODING_RATE codingRate;
};
