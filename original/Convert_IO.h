//*******************************************************************
//Makes rounded bfloat16 from float
bfloat16 float_to_bfloat16(float TestFloat)
{
	bfloat16 New_bf16;
	New_bf16.Exp=0;
	New_bf16.SgnFrag=0;
	//Shortcut for 0 with Sgn Shift to +
	if(TestFloat==0){return New_bf16;}
	//64-Bit Adress of float => 32 Bit unsigned int
	unsigned __int64 *adresse = (unsigned __int64*) (&TestFloat);
	unsigned __int32 Zwischen=*adresse;
	//Takes the 8 Bit of the Exponent
	New_bf16.Exp =((Zwischen<<1)>>24);
	//Takes the left FragLen Bit of the Fragment
	New_bf16.SgnFrag =((Zwischen<<9)>>(32-FragLen));
	//Rounding of Fragment with 8th Bit of Fragment
	if((FragLen<23)&&((Zwischen<<(9+FragLen))>>31)==1)
	{
		//Increment Fragment
		New_bf16.SgnFrag++;
		//Test if Overflow of Fragment
		if (New_bf16.SgnFrag==Pow2Int32[FragLen])
		{
			//Reset Overflow
			New_bf16.SgnFrag=0;
			//Increment of Exponent, if not Inf/NaN (may cause Inf..)
			if (New_bf16.Exp<255){New_bf16.Exp++;}
		}
	}
	//Sgn as leeding Bit of Fragment (0: allways +)
	New_bf16.Sgn =(Zwischen>>31);
	New_bf16.SgnFrag = New_bf16.SgnFrag+(New_bf16.Sgn<<FragLen);
	return New_bf16;
};
//*******************************************************************
unsigned __int8 findPoolNr(unsigned __int8 PNr,unsigned __int32 TestNr)
{
	unsigned __int8 PoolNrTmp=0,TestPoolNrTmp; //Zwischenwerte für Poolzuordnung
	for(int u=Pool[PNr].BisecSteps-1;u>=0;u--)
	{
		TestPoolNrTmp=Pool[PNr].BisecStep[u]+PoolNrTmp;
		if(TestNr>Pool[PNr].Last[TestPoolNrTmp-1]){PoolNrTmp=TestPoolNrTmp;}
	}	
	return PoolNrTmp;
}
//*******************************************************************
//Setzt Suchschritte für reverse Bisektion ohne Überlauf
void setPoolSearch(unsigned __int8 PNr)
{
Pool[PNr].BisecSteps=0;
if((Pool[PNr].PoolsCount>1))
{
	while (Pool[PNr].PoolsCount>Pow2Int32[Pool[PNr].BisecSteps+1])
	{
		Pool[PNr].BisecStep[Pool[PNr].BisecSteps]=Pow2Int32[Pool[PNr].BisecSteps];
		Pool[PNr].BisecSteps++;
	}
	Pool[PNr].BisecStep[Pool[PNr].BisecSteps]=Pool[PNr].PoolsCount-Pow2Int32[Pool[PNr].BisecSteps];
	Pool[PNr].BisecSteps++;
}
}
//*******************************************************************
//Priorisiert zunächst die Null, dann kleine Zahlen
typeOutAdder SevenOfNine(unsigned __int32 NrSel,unsigned __int32 NrPoss)
{
	typeOutAdder AusgabeCode;
	AusgabeCode.BitsUsed=findPoolNr(4,NrPoss);
	AusgabeCode.BitsOut=NrSel;
	unsigned __int32 MaxMampfen=Pow2Int32[AusgabeCode.BitsUsed]-NrPoss+1;
	unsigned __int32 Pow2ForZero;
	//Berechne max. nötige Bits
	if(MaxMampfen>1)
	{
		//Wie viele (2-er Pot) Codes für die Null??
		Pow2ForZero=findPoolNr(5,MaxMampfen);
		if(NrSel>0)
		{
			if(NrSel<=MaxMampfen-Pow2Int32[Pow2ForZero])
			{
				AusgabeCode.BitsOut+=Pow2Int32[Pow2ForZero-1]-1;
				AusgabeCode.BitsUsed--;
			}
			else
			{
				AusgabeCode.BitsOut+=MaxMampfen-1;
			}
		}
		else
		{
			AusgabeCode.BitsUsed-=Pow2ForZero;
		}
	}
	//cout<<((NrSel<10)?" ":"")<<NrSel<<"/0-"<<((NrPoss-1<10)?" ":"")<<NrPoss-1<<",Code: "<<((AusgabeCode.BitsOut<10)?" ":"")<<AusgabeCode.BitsOut<<"("<<1*AusgabeCode.BitsUsed<<"/"<<1*findPoolNr(4,NrPoss)<<"Bit)";
	return AusgabeCode;
}
//*******************************************************************
//Fügt AusgabeCode an Bitstream an
unsigned __int32  FromBitstream(unsigned __int8 BitCount)
{
	unsigned __int32 ResultBits=0;
	unsigned __int8 RestBits=BitCount;
	//wenn Codelänge Null => nichts machen
	while(RestBits>0)
	{
		//Schaffe Platz (dann 1-8Bit frei)
		if(OutBitsFreeRead==0)
		{
			OutByteNr++;
			OutBitsFreeRead=8;
		}
		if(RestBits<=OutBitsFreeRead)
		{
			//Code kann komplett geholt werden
			ResultBits=(ResultBits<<RestBits)+(OutData[OutByteNr]>>(8-RestBits));
			OutData[OutByteNr]=OutData[OutByteNr]<<RestBits;
			OutBitsFreeRead-=RestBits;
			break;
		}
		else
		{
			//Codeschnipsel trennen & einbringen
			ResultBits=(ResultBits<<OutBitsFreeRead)+(OutData[OutByteNr]>>(8-OutBitsFreeRead));
			RestBits-=OutBitsFreeRead;
			OutBitsFreeRead=0;
		}
	}
	return ResultBits;
}
//*******************************************************************
//Fügt AusgabeCode an Bitstream an
void Bitstream(typeOutAdder AusgabeCode)
{
	//wenn Codelänge Null => nichts machen
	while(AusgabeCode.BitsUsed>0)
	{
		//Schaffe Platz (dann 1-8Bit frei)
		if(OutBitsFree==0)
		{
			OutData[OutBytes]=0; //hier löschen!!
			OutBytes++;
			OutBitsFree=8;
		}
		if(AusgabeCode.BitsUsed<=OutBitsFree)
		{
			//Code kann komplett eingebracht werden
			OutBitsFree-=AusgabeCode.BitsUsed;
			OutData[OutBytes-1]+=(AusgabeCode.BitsOut<<OutBitsFree);
			break;
		}
		else
		{
			//Codeschnipsel trennen & einbringen
			OutData[OutBytes-1]+=(AusgabeCode.BitsOut>>(AusgabeCode.BitsUsed-OutBitsFree));
			AusgabeCode.BitsUsed-=OutBitsFree;
			AusgabeCode.BitsOut=AusgabeCode.BitsOut%Pow2Int32[AusgabeCode.BitsUsed];
			OutBitsFree=0;
		}
	}
}
//*******************************************************************
//Priorisiert zunächst die Null, dann kleine Zahlen
unsigned __int32 NineOfSeven(unsigned __int32 NrPoss)
{
	unsigned __int32 NrSel;
	unsigned __int8 BitsMax=findPoolNr(4,NrPoss);
	unsigned __int32 MaxMampfen=Pow2Int32[BitsMax]-NrPoss+1;
	unsigned __int32 Pow2ForZero;
	//Berechne max. nötige Bits
	if(MaxMampfen==1)
	{
		NrSel=FromBitstream(BitsMax);
	}
	else
	{
		//Wie viele (2-er Pot) Codes für die Null??
		Pow2ForZero=findPoolNr(5,MaxMampfen);
		NrSel=FromBitstream(BitsMax-Pow2ForZero);
		if(NrSel>0)
		{
			NrSel=(NrSel<<(Pow2ForZero-1))+FromBitstream(Pow2ForZero-1);
			if(NrSel+1-Pow2Int32[Pow2ForZero-1]<=MaxMampfen-Pow2Int32[Pow2ForZero])
			{
				NrSel=NrSel+1-Pow2Int32[Pow2ForZero-1];
			}
			else
			{
				NrSel=(NrSel<<1)+FromBitstream(1)-MaxMampfen+1;
			}
		}
	}
	return NrSel;
}
//*******************************************************************




