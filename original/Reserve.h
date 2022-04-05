//Bestimmt höchst besetztes Bit in Container (Shortcut für log)
__int16 MSB_In_Long64(unsigned __int64 Test64)
{
	__int16 MSB=0,DeltaMSB=32,MSBTest;
	//65 Möglichkeiten...
	if (Test64==0){MSB=-1;return MSB;}
	do
	{
		MSBTest=MSB+DeltaMSB;
		if (Test64>=Pot2int64[MSBTest])MSB=MSBTest;
		DeltaMSB=DeltaMSB>>1;
	}while(DeltaMSB>0);
	return MSB;
};
//*******************************************************************
