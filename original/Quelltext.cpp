//###################################################################
#include <iostream>
#include <math.h>
using namespace std;
//###################################################################
//Global Settings
const int MAXCODELEN=32;   //MaxLength of Codes (dont change)
//*******************************************************************
typedef struct
{
	unsigned __int8  BitsUsed;//gültige LSB-Bits
	unsigned __int32 BitsOut; //Ausgabewort
}typeOutAdder;
//*******************************************************************
typedef struct
{
	unsigned __int8  Exp;     //Exponent of the bfloat 16 (real Exp=Exp-127)
	unsigned __int8  Sgn;     //Sgn (1=Minus)
	unsigned __int32 SgnFrag; //Sgn & Fragment of the bfloat 16 (MSB=1:Minus)
}bfloat16;
//*******************************************************************
typedef struct
{
	//Poolcounter(Max possible/Used)
	unsigned __int8 PoolsCount,PoolsUsed;
	//Steps for optimal digital Search
	unsigned __int8 BisecSteps,BisecStep[6]; //2^6 systematic for 32-Bit(2x32)
	//Borders of Poolvalues
	unsigned __int32 Last[2*MAXCODELEN],CodeQty[2*MAXCODELEN];
	//Dataset Counter for each Pool
	unsigned __int32 DSQty[2*MAXCODELEN],DSQtyUp[2*MAXCODELEN];
	//CodeOffset for Codeout
	unsigned __int32 CodeOffset[2*MAXCODELEN];
	//Codelen for each Pool
	unsigned __int8 CodeLen[2*MAXCODELEN];
	//PoolAmount for Output
	unsigned __int8 LastPoolNr[2*MAXCODELEN];
}typePool;
//*******************************************************************
//ff. Eintraege entfallen mit Vektoren (Eingang)
unsigned __int32 M_NonZeroSize,M_Row,M_Col,M_Index[16777216];
unsigned __int8 M_RowBased;
float M_Value[16777216];
//ff. Eintraege entfallen mit Vektoren (Timeline)
unsigned __int32 TimelineLen,ZeroCount[16777216];
unsigned __int8  ExpJump[16777216];
unsigned __int32 Fragment[16777216];
unsigned __int8 FragLen;


unsigned __int8	OutBitsFree=0,OutData[16777216];
unsigned __int32 OutBytes=0;

unsigned __int8	OutBitsFreeRead=8;
unsigned __int32 OutByteNr=0;



//Pow2 as Shortcuts
unsigned __int32 Pow2Int32[MAXCODELEN];
double Pow2Dbl129[129];					

//Container für die Codezuordnungen (2 In, 2 Out, nächst höhere 2er-Potenz, nächst niedrigere)
typePool Pool[6];
//###################################################################
#include <D:\Softwareprojekte\CalcSF\CalcSF\Convert_IO.h>
#include <D:\Softwareprojekte\CalcSF\CalcSF\Helper.h>
//###################################################################
int KompMe ()
{
	static bool InitialBasics=false;			 //Setzt beim 1. Start Konstanten
	unsigned __int32 t;
	unsigned __int8 SgnUsed;               //0:kein Eintrag,1:+,2:-,3:+-
	unsigned __int8 PNr,PZiel;
	int JumpSize,MaxJumpSize,LastExpTmp;   //Ermittelung des ExpJump bei Randstellung
	double Sollanteil,Istanteil;           //Berechnung Soll-/Istcodelängen
	unsigned __int8 PoolsReady, NextPoolsReady;       //Abgearbeitete Pools => Codelängen
	unsigned __int32 CodesReady;           //Letzter abgearbeiteter Eintrag
	unsigned __int32 CountUpsReady;        //Abgearbeitete Vorkommensumme
	double FreeCodes;											 //Variable Auffüllung Codebereich
	bfloat16 M_Value16;



	//Lese Spielwerte
	SetTestvalues();
	//###################################################################
	//Set Values (1x) at Program start
	if(InitialBasics==false)
	{
		//Calculate Pow(2) as Shortcuts
		Pow2Int32[0]=1;for(t=1;t<MAXCODELEN;t++){Pow2Int32[t]=2*Pow2Int32[t-1];}
		Pow2Dbl129[0]=1.0;for(t=1;t<=MAXCODELEN;t++){Pow2Dbl129[t]=2.0*Pow2Dbl129[t-1];}
		//Setze Codeborders for Exp-Jumps und Zeros
		for(PNr=0;PNr<2;PNr++)
		{
			Pool[PNr].PoolsCount=(PNr==0?16:63); //16=(2*8+1)-{1 wg. Speed},63=(2*31+1)
			for(t=0;t<5;t++){Pool[PNr].Last[t]=((PNr==0&&t>2)?2*(t-1):t);}
			for(t=5;t<Pool[PNr].PoolsCount;t++){Pool[PNr].Last[t]=2*Pool[PNr].Last[t-2];}
			Pool[PNr].CodeQty[0]=1;
			for(t=1;t<Pool[PNr].PoolsCount;t++){Pool[PNr].CodeQty[t]=Pool[PNr].Last[t]-Pool[PNr].Last[t-1];}
			//Erzeuge Suchmaske für Eingabepools
			setPoolSearch(PNr);
		}
		//Suchmaske für Shortcut Log2
		for(t=0;t<32;t++)Pool[4].Last[t]=Pow2Int32[t];
		Pool[4].PoolsCount=32;
		setPoolSearch(4);
		for(t=0;t<31;t++)Pool[5].Last[t]=Pool[4].Last[t+1]-1;
		Pool[5].Last[31]=0xFFFFFFFF;
		Pool[5].PoolsCount=32;
		setPoolSearch(5);
		InitialBasics=true;
	}


	//for(PNr=0;PNr<2;PNr++)ShowPoolSearch(PNr,true);



	//###################################################################
	//Bild Timeline and Counts for Coding
	TimelineLen=0;LastExpTmp=127;SgnUsed=0;
	for(PNr=0;PNr<2;PNr++){for(t=0;t<Pool[PNr].PoolsCount;t++){Pool[PNr].DSQty[t]=0;Pool[PNr].DSQtyUp[t]=0;}}
	for(t=0;t<M_NonZeroSize;t++)
	{
		ZeroCount[TimelineLen]=M_Index[t]-((t>0)?(1+M_Index[t-1]):0);
		//Convert Floatvalue to b_float
		M_Value16=float_to_bfloat16(M_Value[t]);
		//Show_bfloat16(M_Value16);
		SgnUsed=(M_Value16.Sgn+1)|SgnUsed;
		if(M_Value16.Exp>=LastExpTmp)
		{
			JumpSize=M_Value16.Exp-LastExpTmp;
			//Bidir Jump possible up to LastExp
			ExpJump[TimelineLen]=JumpSize+((LastExpTmp<JumpSize)?LastExpTmp:JumpSize);
		}
		else
		{
			JumpSize=LastExpTmp-M_Value16.Exp;
			MaxJumpSize=255-LastExpTmp;
			//Bidir Jump possible up to (255-LastExp)
			ExpJump[TimelineLen]=JumpSize+((MaxJumpSize<JumpSize)?MaxJumpSize:JumpSize-1);
		}
		//Data to Poolcounter
		Pool[0].DSQty[findPoolNr(0,ExpJump[TimelineLen])]++;
		Pool[1].DSQty[findPoolNr(1,ZeroCount[TimelineLen])]++;
		Fragment[TimelineLen]=M_Value16.SgnFrag;
		//Reset by Iterator
		TimelineLen++;
		//Copy for next Jump-Calculations
		LastExpTmp=M_Value16.Exp;
	}
	//Ergänze Nullen am Ende (auch wenn n.v., damit Dateiende definiert...)
//	JumpSize=255-LastExpTmp;
//	ExpJump[TimelineLen]=JumpSize+((LastExpTmp<JumpSize)?LastExpTmp:JumpSize);
//	ZeroCount[TimelineLen]=(M_NonZeroSize>0)?M_Row*M_Col-(M_Index[M_NonZeroSize-1]+1):M_Row*M_Col;
//	Pool[0].DSQty[findPoolNr(0,ExpJump[TimelineLen])]++;
//	Pool[1].DSQty[findPoolNr(1,ZeroCount[TimelineLen])]++;
//	TimelineLen++;
	//###################################################################
	//Look for last used Container & calculate Summs of Datasets for both Codes
	for(PNr=0;PNr<2;PNr++)
	{
		Pool[PNr].PoolsUsed=0;
		for(t=Pool[PNr].PoolsCount;t>0;t--)if(Pool[PNr].DSQty[t-1]>0){Pool[PNr].PoolsUsed=t;break;}
		Pool[PNr].DSQtyUp[0]=Pool[PNr].DSQty[0];
		for(t=1;t<Pool[PNr].PoolsUsed;t++){Pool[PNr].DSQtyUp[t]=Pool[PNr].DSQtyUp[t-1]+Pool[PNr].DSQty[t];}
		//for(t=0;t<Pool[PNr].PoolsUsed;t++)cout<<t<<" "<<Pool[PNr].DSQty[t]<<" "<<Pool[PNr].DSQtyUp[t]<<" "<<Pool[PNr].Last[t]<<"\n";
	}
	//###################################################################
	//Erstelle Codezuordnungen mit abgerundeter Menge
	for(PNr=0;PNr<2;PNr++)
	{
		FreeCodes=Pow2Dbl129[32]-(Pool[PNr].Last[Pool[PNr].PoolsUsed-1]+1);
		PoolsReady=0;CodesReady=0;CountUpsReady=0;
		for(unsigned __int32 TestCodeLen=0;TestCodeLen<=MAXCODELEN;TestCodeLen++)
		{
			NextPoolsReady=PoolsReady;
			for(t=PoolsReady;t<Pool[PNr].PoolsUsed;t++)
			{
				Sollanteil=double(Pool[PNr].Last[t]+1-CodesReady)*(Pow2Dbl129[32-TestCodeLen]-1.0)/FreeCodes;
				if(Sollanteil>1){break;}//Mit dieser CodeLen nicht mehr möglich
				Istanteil=double(Pool[PNr].DSQtyUp[t]-CountUpsReady)/double(TimelineLen);
				if(Istanteil>=Sollanteil){NextPoolsReady=t+1;}
				//cout<<"t: "<<t<<" TestCodeLen: "<<TestCodeLen<<" Sollanteil "<<Sollanteil<<" Istanteil "<<Istanteil<<"\n";
			}
			if(NextPoolsReady>PoolsReady)
				{
					for(t=PoolsReady;t<NextPoolsReady;t++){Pool[PNr].CodeLen[t]=TestCodeLen;}
					CodesReady=Pool[PNr].Last[NextPoolsReady-1]+1;
					FreeCodes=Pow2Dbl129[32]-(Pool[PNr].Last[Pool[PNr].PoolsUsed-1]+1-CodesReady);
					CountUpsReady=Pool[PNr].DSQtyUp[NextPoolsReady-1];
					PoolsReady=NextPoolsReady;
				}
			if(PoolsReady==Pool[PNr].PoolsUsed){break;}
		}
		//###################################################################
		//Verteile den Rest "von oben nach unten"
		FreeCodes=1; //Setze Bereich auf 100% frei
		for(t=0;t<Pool[PNr].PoolsUsed;t++){FreeCodes-=double(Pool[PNr].CodeQty[t])/Pow2Dbl129[Pool[PNr].CodeLen[t]];}
		while(FreeCodes>0&&Pool[PNr].PoolsUsed>0)
		{		
			for(t=0;t<Pool[PNr].PoolsUsed;t++)
			{
				//cout<<t<<": "<<1*Pool[PNr].CodeLen[t]<<"=>";
				//Pruefen, ob Codeanteil vergroessert (2x) werden kann
				if(FreeCodes>=double(Pool[PNr].CodeQty[t])/Pow2Dbl129[Pool[PNr].CodeLen[t]])
				{
					FreeCodes-=double(Pool[PNr].CodeQty[t])/Pow2Dbl129[Pool[PNr].CodeLen[t]];
					Pool[PNr].CodeLen[t]--;
					//cout<<1*Pool[PNr].CodeLen[t];
					if(FreeCodes==0){break;}
				}
				//cout<<"\n";
			}
		}
		//cout<<"\n"<<"Pools Used: "<<1*Pool[PNr].PoolsUsed<<"\n";
	}
	//###################################################################
	//Erzeuge Ausgabepools
	for(PNr=0;PNr<2;PNr++)
	{
		PZiel=PNr+2;
		Pool[PZiel].PoolsCount=1;
		Pool[PZiel].LastPoolNr[0]=0;
		Pool[PZiel].CodeLen[0]=Pool[PNr].CodeLen[0];
		Pool[PZiel].CodeOffset[0]=0;
		for(t=1;t<Pool[PNr].PoolsUsed;t++)
		{
			if(Pool[PNr].CodeLen[t]>Pool[PNr].CodeLen[t-1])
			{
				Pool[PZiel].Last[Pool[PZiel].PoolsCount-1]=Pool[PNr].Last[t-1];
				Pool[PZiel].CodeLen[Pool[PZiel].PoolsCount]=Pool[PNr].CodeLen[t];
				if(Pool[PZiel].PoolsCount>1)
				{
					//Nächsten Code letzte CodeLen
					Pool[PZiel].CodeOffset[Pool[PZiel].PoolsCount-1]=(Pool[PZiel].CodeOffset[Pool[PZiel].PoolsCount-2]+Pool[PZiel].Last[Pool[PZiel].PoolsCount-2]+1);
					//Schieben auf neue CodeLen
					Pool[PZiel].CodeOffset[Pool[PZiel].PoolsCount-1]=(Pool[PZiel].CodeOffset[Pool[PZiel].PoolsCount-1]<<(Pool[PZiel].CodeLen[Pool[PZiel].PoolsCount-1]-Pool[PZiel].CodeLen[Pool[PZiel].PoolsCount-2]));
					//Neuen Startwert abziehen
					Pool[PZiel].CodeOffset[Pool[PZiel].PoolsCount-1]-=(Pool[PZiel].Last[Pool[PZiel].PoolsCount-2]+1);
				}
				Pool[PZiel].PoolsCount++;
			}
			Pool[PZiel].LastPoolNr[Pool[PZiel].PoolsCount-1]=t;
		}
		Pool[PZiel].Last[Pool[PZiel].PoolsCount-1]=Pool[PNr].Last[Pool[PNr].PoolsUsed-1];
		if(Pool[PZiel].PoolsCount>1)
		{
			//Nächsten Code letzte CodeLen
			Pool[PZiel].CodeOffset[Pool[PZiel].PoolsCount-1]=(Pool[PZiel].CodeOffset[Pool[PZiel].PoolsCount-2]+Pool[PZiel].Last[Pool[PZiel].PoolsCount-2]+1);
			//Schieben auf neue CodeLen
			Pool[PZiel].CodeOffset[Pool[PZiel].PoolsCount-1]=(Pool[PZiel].CodeOffset[Pool[PZiel].PoolsCount-1]<<(Pool[PZiel].CodeLen[Pool[PZiel].PoolsCount-1]-Pool[PZiel].CodeLen[Pool[PZiel].PoolsCount-2]));
			//Neuen Startwert abziehen
			Pool[PZiel].CodeOffset[Pool[PZiel].PoolsCount-1]-=(Pool[PZiel].Last[Pool[PZiel].PoolsCount-2]+1);
		}
		Pool[PZiel].PoolsUsed=Pool[PZiel].PoolsCount;
		//Erzeuge Suchmaske für Ausgabepools
		setPoolSearch(PZiel);
	}
	for(PNr=0;PNr<6;PNr++)ShowPoolSearch(PNr,true);
	//###################################################################
	if(double(M_Row)*double(M_Col)>Pow2Dbl129[31]){cout<<"Matrix zu gross, keine Ausgabe";return 0;}
	//###################################################################
	int LastDefinedCode;
	int LastDefinedPool;
	int LastDefinedCodeLen;
	unsigned __int32 MaxPossCode;
	unsigned __int8 MaxPossPool;
	unsigned __int8 RowColCodeLen;
	unsigned __int32 FragCodes;
	bool ZerosAppear;
//	typeOutAdder AusgabeCode;

	OutBitsFree=0;
	OutBytes=0;

	
	cout<<"Header\n\n";
	//VersionsNr 0-2: 2Bit, 3: weitere Bits
	Bitstream(SevenOfNine(0,4));cout<<",VersionsNr=0\n";
	ShowBitstream();
	//1 Bit für Row-Based
	Bitstream(SevenOfNine(M_RowBased,2));cout<<",Row-Based="<<1*M_RowBased<<"\n";
	ShowBitstream();
  //Kann Werte annehmen: 0..2^31-1
	RowColCodeLen=findPoolNr(4,((M_Row>M_Col)?M_Row:M_Col)+1);
	Bitstream(SevenOfNine(RowColCodeLen,32));cout<<",RowColCodeLen="<<1*RowColCodeLen<<"\n";
	ShowBitstream();
	//RowColCodeLen=0, wenn M_Row=0 & M_Col=0
	if (RowColCodeLen>0)
	{
		Bitstream(SevenOfNine(M_Row,Pow2Int32[RowColCodeLen]));cout<<",Rows="<<1*M_Row<<"\n";
		ShowBitstream();
		Bitstream(SevenOfNine(M_Col,Pow2Int32[RowColCodeLen]));cout<<",Cols="<<1*M_Col<<"\n";
		ShowBitstream();
		//3:+-,2:-,1:+,0:no Values
		Bitstream(SevenOfNine(SgnUsed,4));cout<<",SgnUsed="<<1*SgnUsed<<":"<<((SgnUsed%2==1)?"+":"")<<((SgnUsed>1)?"-":"")<<"\n";
		ShowBitstream();
		if (SgnUsed>0)
		{
			//Kann Werte annehmen: 0..2^31-1, Option Base 0
			Bitstream(SevenOfNine(M_NonZeroSize,M_Row*M_Col+1));cout<<",NonZeroSize="<<M_NonZeroSize<<"\n";
			ShowBitstream();
			//4 Bit für Fragment 0..14: Len = 7+(0..14), 15: Len=23
			Bitstream(SevenOfNine(((FragLen<22)?(FragLen-7):(FragLen-8)),16));cout<<",FragLen="<<1*FragLen<<"\n";
			ShowBitstream();
			//Kann Werte annehmen: 0..7, Option Base 0
			Bitstream(SevenOfNine(Pool[2].CodeLen[0],8));cout<<",ExpJump1stCodeLen="<<1*Pool[2].CodeLen[0]<<"\n";
			ShowBitstream();
			//Keine Nullen, wenn Matrix voll besetzt
			ZerosAppear=(M_NonZeroSize<M_Row*M_Col+1);
			if (ZerosAppear==true)
			{
				//Kann Werte annehmen: 0..31, Option Base 0
				Bitstream(SevenOfNine(Pool[3].CodeLen[0],32));cout<<",Zeros1stCodeLen="<<1*Pool[3].CodeLen[0]<<"\n";
				ShowBitstream();
			}
		}
		for(PNr=2;PNr<4;PNr++)
		{
			//Kein CodeLen-Skipping (keine Matrizen habe kurze Codes..)
			FreeCodes=1; //Setze Bereich auf 100% frei
			LastDefinedCode=-1;
			LastDefinedPool=-1;
			LastDefinedCodeLen=Pool[PNr].CodeLen[0]-1;
			if(LastDefinedCodeLen>=0)
			{
				cout<<"--------------------------------------------------------------\n";
				cout<<"Pooldaten("<<PNr-2<<")\n\n";
				for(t=0;t<Pool[PNr].PoolsCount;t++)
				{
					//Ausgabe, wenn eine Codelänge keine Einträge hat
					while(LastDefinedCodeLen<Pool[PNr].CodeLen[t]-1)
					{
						LastDefinedCodeLen++;
						MaxPossCode=LastDefinedCode+FreeCodes*Pow2Dbl129[LastDefinedCodeLen];
						MaxPossPool=findPoolNr(1,MaxPossCode);
						Bitstream(SevenOfNine(0,MaxPossPool-LastDefinedPool+1));cout<<",Min/MaxPossPool: "<<LastDefinedPool<<"/"<<1*MaxPossPool<<"\n";
						ShowBitstream();
					}
					MaxPossCode=LastDefinedCode+FreeCodes*Pow2Dbl129[Pool[PNr].CodeLen[t]];
					MaxPossPool=findPoolNr(1,MaxPossCode);
					Bitstream(SevenOfNine(Pool[PNr].LastPoolNr[t]-LastDefinedPool,MaxPossPool-LastDefinedPool+1));cout<<",Min/MaxPossPool: "<<LastDefinedPool<<"/"<<1*MaxPossPool<<"\n";
					ShowBitstream();
					FreeCodes-=(Pool[PNr].Last[t]-LastDefinedCode)/Pow2Dbl129[Pool[PNr].CodeLen[t]];
					//cout<<"FreeCodes: "<<100*FreeCodes<<"%\n";
					LastDefinedPool=Pool[PNr].LastPoolNr[t];
					LastDefinedCode=Pool[PNr-2].Last[LastDefinedPool];
					LastDefinedCodeLen=Pool[PNr].CodeLen[t];
				}
			}
		}
		cout<<"--------------------------------------------------------------\n";
		cout<<"Timeline\n\n";
		FragCodes=Pow2Int32[(SgnUsed==3)?FragLen+1:FragLen];
		for(unsigned __int32 i=0;i<TimelineLen;i++)
		{
			if (ZerosAppear==true)
			{
				Bitstream(SevenOfNine(ZeroCount[i]+Pool[3].CodeOffset[findPoolNr(3,ZeroCount[i])],Pow2Int32[Pool[3].CodeLen[findPoolNr(3,ZeroCount[i])]]));
				//cout<<ZeroCount[i]<<" Nullen: "<<ZeroCount[i]+Pool[3].CodeOffset[findPoolNr(3,ZeroCount[i])]<<", Len:"<<1*Pool[3].CodeLen[findPoolNr(3,ZeroCount[i])]<<"\n";
				//ShowBitstream();
			}
			//Liegen alle Zahlen in einer 2-er-Potenz(z.B. Bilddaten) => keine Ausgabe
			if(Pool[2].CodeLen[0]>0)
			{
				Bitstream(SevenOfNine(ExpJump[i]+Pool[2].CodeOffset[findPoolNr(2,ExpJump[i])],Pow2Int32[Pool[2].CodeLen[findPoolNr(2,ExpJump[i])]]));
				//cout<<1*ExpJump[i]<<" ExpJump: "<<ExpJump[i]+Pool[2].CodeOffset[findPoolNr(2,ExpJump[i])]<<", Len:"<<1*Pool[2].CodeLen[findPoolNr(2,ExpJump[i])]<<"\n";
				//ShowBitstream();
			}
			Bitstream(SevenOfNine(Fragment[i]%FragCodes,FragCodes));//cout<<",Fragment["<<i<<"]="<<Fragment[i]%FragCodes<<"\n";
			//ShowBitstream();
			//cout<<"\n";
		}
	}

	//CodeOffset(CodeLen)=Pow2Int[CodeLen+1]*VerbrauchterAnteilVorPool-First(CodeLen)
	//AusgabeCode=CodeOffset(CodeLen)+Value

	//Code Prio 0
	//Pool[1].Count => Row Col
	//int u=10;
	//for (int t=0;t<u;t++){cout<<t<<": "<<1*SevenOfNine(t,u).BitsOut<<" "<<1*SevenOfNine(t,u).BitsUsed<<"\n";};
//for (int t=0;t<18;t++){cout<<t<<" "<<1*findPoolNr(5,t)<<"\n";};


	return 0;
};
//*******************************************************************
int DeKompMe ()
{
	unsigned __int8 RowColCodeLen,SgnUsed,ExpJump1stCodeLen,Zeros1stCodeLen,MaxPossPool,FragCodes,LastExpTmp=127;
	double FreeCodes;											 //Variable Auffüllung Codebereich
	bool ZerosAppear;
	int LastDefinedCode,LastDefinedPool,LastDefinedCodeLen;
	unsigned __int32 MaxPossCode,Gelesen;
	if(FromBitstream(2)==0)
	{
		M_RowBased=FromBitstream(1);cout<<"Row-Based: "<<1*M_RowBased<<"\n";
		RowColCodeLen=NineOfSeven(32);cout<<"Row-ColLen: "<<1*RowColCodeLen<<"\n";
		M_Row=NineOfSeven(Pow2Int32[RowColCodeLen]);cout<<"Rows: "<<M_Row<<"\n";
		M_Col=NineOfSeven(Pow2Int32[RowColCodeLen]);cout<<"Cols: "<<M_Col<<"\n";
		SgnUsed=NineOfSeven(4);cout<<"SgnUsed: "<<1*SgnUsed<<"\n";
		M_NonZeroSize=NineOfSeven(M_Row*M_Col+1);cout<<"NonZeroSize: "<<M_NonZeroSize<<"\n";
		FragLen=NineOfSeven(16);FragLen+=((FragLen>14)?8:7);cout<<"FragLen: "<<1*FragLen<<"\n";
		ExpJump1stCodeLen=NineOfSeven(8);cout<<"ExpJump1stCodeLen: "<<1*ExpJump1stCodeLen<<"\n";
		//Keine Nullen, wenn Matrix voll besetzt
		ZerosAppear=(M_NonZeroSize<M_Row*M_Col+1);
		if (ZerosAppear==true)
		{
			Zeros1stCodeLen=NineOfSeven(32);cout<<"Zeros1stCodeLen: "<<1*Zeros1stCodeLen<<"\n";
		}
		for(unsigned __int8 PNr=2;PNr<4;PNr++)
		{
			//Kein CodeLen-Skipping (keine Matrizen habe kurze Codes..)
			FreeCodes=1; //Setze Bereich auf 100% frei
			LastDefinedCode=-1;
			LastDefinedPool=-1;
			LastDefinedCodeLen=(PNr==2)?ExpJump1stCodeLen-1:Zeros1stCodeLen-1;
			Pool[PNr].PoolsCount=0;
			if(LastDefinedCodeLen>=0)
			{
				cout<<"--------------------------------------------------------------\n";
				cout<<"Pooldaten("<<PNr-2<<")\n\n";
				for(int t=0;t<Pool[PNr-2].PoolsCount;t++)
				{
					do
					{				
						LastDefinedCodeLen++;
						MaxPossCode=LastDefinedCode+FreeCodes*Pow2Dbl129[LastDefinedCodeLen];
						MaxPossPool=findPoolNr(1,MaxPossCode);
						Gelesen=NineOfSeven(MaxPossPool-LastDefinedPool+1);
						cout<<Gelesen<<" "<<(MaxPossPool-LastDefinedPool+1)<<" "<<1*LastDefinedPool<<" "<<1*MaxPossPool<<" "<<"\n";
					}while(Gelesen==0);
					Pool[PNr].CodeLen[t]=LastDefinedCodeLen;
					Pool[PNr].LastPoolNr[t]=LastDefinedPool+Gelesen;
					Pool[PNr].Last[t]=Pool[PNr-2].Last[Pool[PNr].LastPoolNr[t]];
					Pool[PNr].CodeOffset[t]=(1-FreeCodes)*Pow2Int32[LastDefinedCodeLen]-LastDefinedCode-1;
					FreeCodes-=(Pool[PNr].Last[t]-LastDefinedCode)/Pow2Dbl129[Pool[PNr].CodeLen[t]];
					LastDefinedCode=Pool[PNr-2].Last[Pool[PNr].LastPoolNr[t]];
					LastDefinedPool=LastDefinedPool+Gelesen;
					Pool[PNr].PoolsCount++;
					//cout<<"FreeCodes: "<<100*FreeCodes<<"%\n";
					if(FreeCodes==0)break;
				}
			}
		}
		cout<<"--------------------------------------------------------------\n";
		cout<<"Timeline\n\n";
		unsigned __int8 PushLen;
		int ExpNow=127,IndexNow=-1;
		double MySign=((SgnUsed==1)?1.0:-1.0),MyFragValue;
		FragCodes=Pow2Int32[(SgnUsed==3)?FragLen+1:FragLen];
		for(unsigned __int32 i=0;i<M_NonZeroSize;i++)
		{
			if (ZerosAppear==true)
			{
				ZeroCount[i]=FromBitstream(Pool[3].CodeLen[0]);
				if(ZeroCount[i]>Pool[3].Last[0])
				{
					for(unsigned __int8 t=1;t<Pool[3].PoolsCount;t++)
					{
						PushLen=Pool[3].CodeLen[t]-Pool[3].CodeLen[t-1];
						ZeroCount[i]=(ZeroCount[i]<<PushLen)+FromBitstream(PushLen);
						if(ZeroCount[i]<=Pool[3].Last[t]+Pool[3].CodeOffset[t])
						{
							ZeroCount[i]-=Pool[3].CodeOffset[t];
							break;
						}
					}
				}
				IndexNow+=1+ZeroCount[i];
				cout<<IndexNow<<" ";
			}
			ExpJump[i]=FromBitstream(Pool[2].CodeLen[0]);
			if(ExpJump[i]>Pool[2].Last[0])
			{
				for(unsigned __int8 t=1;t<Pool[2].PoolsCount;t++)
				{
					PushLen=Pool[2].CodeLen[t]-Pool[2].CodeLen[t-1];
					ExpJump[i]=(ExpJump[i]<<PushLen)+FromBitstream(PushLen);
					if(ExpJump[i]<=Pool[2].Last[t]+Pool[2].CodeOffset[t])
					{
						ExpJump[i]-=Pool[2].CodeOffset[t];
						break;
					}
				}
			}
			//cout<<1*ExpJump[i]<<" ";
			if(LastExpTmp<128)
			{
				if(ExpJump[i]<=2*LastExpTmp)
				{
					if(ExpJump[i]%2==0)
					{
						ExpJump[i]=ExpJump[i]/2+LastExpTmp;
					}
					else
					{
						ExpJump[i]=LastExpTmp-(ExpJump[i]+1)/2;
					}
				}
				//Sonst bleibt ExpJump[i]...
			}
			else
			{
				if(ExpJump[i]<=2*(255-LastExpTmp))
				{
					if(ExpJump[i]%2==0)
					{
						ExpJump[i]=ExpJump[i]/2+LastExpTmp;
					}
					else
					{
						ExpJump[i]=LastExpTmp-(ExpJump[i]+1)/2;
					}
				}
				else
				{
					ExpJump[i]=255-ExpJump[i];
				}
				//Sonst bleibt ExpJump[i]...
			}
			LastExpTmp=ExpJump[i];
			//cout<<1*ExpJump[i]<<" ";
			//Vorzeichen holen, wenn geschrieben
			if(SgnUsed==3){MySign=((FromBitstream(1)==1)?-1.0:1.0);}
			MyFragValue=MySign*(1.0+FromBitstream(FragLen)/Pow2Dbl129[FragLen]);
			if(LastExpTmp>127)
			{
				MyFragValue*=Pow2Dbl129[LastExpTmp-127];
			}
			else
			{
				MyFragValue/=Pow2Dbl129[127-LastExpTmp];
			}

			cout<<MyFragValue<<" ";
			cout<<"\n";
		}
	}
	else
	{
		cout<<"Falsche Version\n";
	}
	cout<<OutBytes;
	//for(int PNr=2;PNr<4;PNr++)ShowPoolSearch(PNr,false);
	return 0;
};
//*******************************************************************
int main ()
{
	KompMe();
	DeKompMe();
	return 0;
};
