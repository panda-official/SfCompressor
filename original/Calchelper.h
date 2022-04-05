//*******************************************************************
//Macht ungerundeten Superfloat aus double
//Exp=0				M=0			Null
//Exp=0				M>0			Denormalisierte Zahl
//0<Exp<2047	M=0			Normalisierte Zahl
//Exp=2047		M=0			Infinity
//Exp=2047		M>0			NaN
superfloat Dbl_to_SF(double TestDbl, unsigned __int16 Parts, bool rounded)
{
	superfloat New_SF=SF_NULL;
	unsigned __int64 *adresse = (unsigned __int64*) (&TestDbl);
	unsigned __int64 Zwischen=*adresse;
	unsigned __int64 Mantisse;
	New_SF.Parts =Parts;
	New_SF.Exp =((Zwischen<<1)>>53)-1023;
	New_SF.Vz=(Zwischen>=Pot2int64[63])?-1:1;
	//Holt die Mantisse auf die linken 52-Bit im 64-Bit Container(ohne die MSB 1)
	Mantisse=(Zwischen<<12);
	//Wenn New_SF.Exp=1024 => Infinity oder NaN
	if(New_SF.Exp!=1024)
	{
		//Wenn double gerundet,Fehler=0.5 Bit von dbl (auch Null rundbar...)
		if(rounded==true)New_SF.DeltaAbs=Pot2dbl[32*New_SF.Parts-53];
		//Wenn 0<Exp<2047: Normalisierte Zahl
		if(New_SF.Exp>-1023)
		{
			//Führende 1 und 31 Bit
			New_SF.Part[0]=Pot2int64[31]+(Mantisse>>33);
			//restliche 21 Bit
			New_SF.Part[1]=(Mantisse<<31)>>32;
			return New_SF;
		}
		//Pruefen auf denormalisierte Zahl
		if(Mantisse!=0)
		{
			//Wenn zu klein, Mantisse verdoppeln, Exponent dekrementieren
			while(Mantisse<Pot2int64[63]){Mantisse=(Mantisse<<1);New_SF.Exp--;New_SF.DeltaAbs*=2;}
			//Führende  32 Bit
			New_SF.Part[0]=(Mantisse>>32);
			//restliche 20 Bit
			New_SF.Part[1]=(Mantisse<<32)>>32;
			return New_SF;
		}
		//Meine Null hat den Exponenten 2^0
		New_SF.Exp=0;
	}
	else
	{
		New_SF.CalcError=(Mantisse==0)?INFINITY:NOTANUMBER;
	}
	return New_SF;
};
//*******************************************************************
//Zeigt Superfloat aus double
void Show_SF(superfloat MySF)
{
	cout<<MySF.Vz<<"*2^"<<MySF.Exp<<"*\n"; //Ausgabe Exponent
	for(__int16 u=0;u<MySF.Parts;u++)
	{
		if (u>0) cout<<" "; //Platzhalter für führendes Komma
		for(__int16 t=31;t>=0;t--)
		{
			cout<<(MySF.Part[u]>>t)%2; //gibt ein Bit aus
			if ((u==0)&(t==31)) cout<<"."; //setzt Komma hinter MSB-Bit
			if (t%8==0) cout<<" "; //setzt Space alle 8 Bit
		}
	cout<<"  "; //großer Space zwischen Containern (2 je Reihe)
	if (u%2==1) cout<<"\n";
	if (u==1) {for(__int16 t=0;t<63;t++)cout<<"-";cout<<"\n";} //Underline dbl-Raum
	}
	double testme;
	testme=log(MySF.DeltaAbs)/log(2.0);
	cout<<"+-"<<MySF.DeltaAbs<<"  (2^"<<testme<<" LSB) Rest:"<<MySF.Part[MySF.Parts]/Pot2dbl[64]<<"  Errors:"<<MySF.CalcError<<"\n"<<"\n";
};
//*******************************************************************
