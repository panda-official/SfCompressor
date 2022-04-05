void SetTestvalues()
{
	//Testwerte
	M_RowBased=true;
	M_Row=1000;
	M_Col=1000;
	FragLen=7;//7-21 oder 23
	M_NonZeroSize=260;

	M_Index[0]=1;//23;
	M_Value[0]=2.5;
	M_Index[1]=5;//25;
	M_Value[1]=-1;
	M_Index[2]=8;//136;
	M_Value[2]=-1.5;

	for(int i=1;i<260;i++)
	{
		M_Value[i]=1+i;
	}

M_Index[0]=13;
M_Index[1]=26;
M_Index[2]=27;
M_Index[3]=34;
M_Index[4]=35;
M_Index[5]=39;
M_Index[6]=42;
M_Index[7]=49;
M_Index[8]=50;
M_Index[9]=53;
M_Index[10]=54;
M_Index[11]=63;
M_Index[12]=71;
M_Index[13]=90;
M_Index[14]=95;
M_Index[15]=99;
M_Index[16]=102;
M_Index[17]=107;
M_Index[18]=110;
M_Index[19]=144;
M_Index[20]=166;
M_Index[21]=182;
M_Index[22]=224;
M_Index[23]=225;
M_Index[24]=240;
M_Index[25]=247;
M_Index[26]=260;
M_Index[27]=262;
M_Index[28]=265;
M_Index[29]=286;
M_Index[30]=291;
M_Index[31]=298;
M_Index[32]=303;
M_Index[33]=306;
M_Index[34]=324;
M_Index[35]=326;
M_Index[36]=328;
M_Index[37]=337;
M_Index[38]=364;
M_Index[39]=375;
M_Index[40]=378;
M_Index[41]=389;
M_Index[42]=412;
M_Index[43]=421;
M_Index[44]=424;
M_Index[45]=426;
M_Index[46]=428;
M_Index[47]=443;
M_Index[48]=445;
M_Index[49]=452;
M_Index[50]=466;
M_Index[51]=467;
M_Index[52]=473;
M_Index[53]=486;
M_Index[54]=493;
M_Index[55]=498;
M_Index[56]=501;
M_Index[57]=518;
M_Index[58]=540;
M_Index[59]=560;
M_Index[60]=574;
M_Index[61]=585;
M_Index[62]=598;
M_Index[63]=637;
M_Index[64]=639;
M_Index[65]=647;
M_Index[66]=652;
M_Index[67]=664;
M_Index[68]=673;
M_Index[69]=678;
M_Index[70]=686;
M_Index[71]=690;
M_Index[72]=694;
M_Index[73]=702;
M_Index[74]=707;
M_Index[75]=728;
M_Index[76]=737;
M_Index[77]=739;
M_Index[78]=757;
M_Index[79]=764;
M_Index[80]=772;
M_Index[81]=779;
M_Index[82]=800;
M_Index[83]=808;
M_Index[84]=814;
M_Index[85]=818;
M_Index[86]=821;
M_Index[87]=825;
M_Index[88]=834;
M_Index[89]=836;
M_Index[90]=852;
M_Index[91]=858;
M_Index[92]=863;
M_Index[93]=865;
M_Index[94]=867;
M_Index[95]=889;
M_Index[96]=898;
M_Index[97]=910;
M_Index[98]=917;
M_Index[99]=919;
M_Index[100]=925;
M_Index[101]=932;
M_Index[102]=936;
M_Index[103]=960;
M_Index[104]=970;
M_Index[105]=983;
M_Index[106]=995;
M_Index[107]=1009;
M_Index[108]=1012;
M_Index[109]=1014;
M_Index[110]=1027;
M_Index[111]=1045;
M_Index[112]=1052;
M_Index[113]=1070;
M_Index[114]=1073;
M_Index[115]=1076;
M_Index[116]=1077;
M_Index[117]=1079;
M_Index[118]=1086;
M_Index[119]=1090;
M_Index[120]=1095;
M_Index[121]=1100;
M_Index[122]=1111;
M_Index[123]=1129;
M_Index[124]=1137;
M_Index[125]=1145;
M_Index[126]=1147;
M_Index[127]=1160;
M_Index[128]=1163;
M_Index[129]=1165;
M_Index[130]=1174;
M_Index[131]=1210;
M_Index[132]=1226;
M_Index[133]=1228;
M_Index[134]=1239;
M_Index[135]=1251;
M_Index[136]=1263;
M_Index[137]=1291;
M_Index[138]=1327;
M_Index[139]=1342;
M_Index[140]=1352;
M_Index[141]=1372;
M_Index[142]=1381;
M_Index[143]=1414;
M_Index[144]=1416;
M_Index[145]=1434;
M_Index[146]=1442;
M_Index[147]=1445;
M_Index[148]=1463;
M_Index[149]=1464;
M_Index[150]=1482;
M_Index[151]=1492;
M_Index[152]=1514;
M_Index[153]=1519;
M_Index[154]=1547;
M_Index[155]=1551;
M_Index[156]=1555;
M_Index[157]=1562;
M_Index[158]=1582;
M_Index[159]=1617;
M_Index[160]=1626;
M_Index[161]=1629;
M_Index[162]=1640;
M_Index[163]=1661;
M_Index[164]=1667;
M_Index[165]=1677;
M_Index[166]=1678;
M_Index[167]=1680;
M_Index[168]=1701;
M_Index[169]=1715;
M_Index[170]=1719;
M_Index[171]=1722;
M_Index[172]=1735;
M_Index[173]=1738;
M_Index[174]=1748;
M_Index[175]=1753;
M_Index[176]=1754;
M_Index[177]=1762;
M_Index[178]=1763;
M_Index[179]=1770;
M_Index[180]=1772;
M_Index[181]=1777;
M_Index[182]=1782;
M_Index[183]=1796;
M_Index[184]=1811;
M_Index[185]=1838;
M_Index[186]=1845;
M_Index[187]=1848;
M_Index[188]=1855;
M_Index[189]=1857;
M_Index[190]=1869;
M_Index[191]=1873;
M_Index[192]=1887;
M_Index[193]=1894;
M_Index[194]=1908;
M_Index[195]=1912;
M_Index[196]=1923;
M_Index[197]=1924;
M_Index[198]=1933;
M_Index[199]=1940;
M_Index[200]=1942;
M_Index[201]=1955;
M_Index[202]=1958;
M_Index[203]=1977;
M_Index[204]=1997;
M_Index[205]=2008;
M_Index[206]=2009;
M_Index[207]=2011;
M_Index[208]=2039;
M_Index[209]=2051;
M_Index[210]=2052;
M_Index[211]=2054;
M_Index[212]=2075;
M_Index[213]=2083;
M_Index[214]=2085;
M_Index[215]=2089;
M_Index[216]=2091;
M_Index[217]=2092;
M_Index[218]=2141;
M_Index[219]=2145;
M_Index[220]=2169;
M_Index[221]=2170;
M_Index[222]=2177;
M_Index[223]=2183;
M_Index[224]=2184;
M_Index[225]=2189;
M_Index[226]=2194;
M_Index[227]=2220;
M_Index[228]=2239;
M_Index[229]=2251;
M_Index[230]=2283;
M_Index[231]=2293;
M_Index[232]=2297;
M_Index[233]=2302;
M_Index[234]=2318;
M_Index[235]=2336;
M_Index[236]=2350;
M_Index[237]=2368;
M_Index[238]=2369;
M_Index[239]=2375;
M_Index[240]=2376;
M_Index[241]=2382;
M_Index[242]=2390;
M_Index[243]=2402;
M_Index[244]=2446;
M_Index[245]=2469;
M_Index[246]=2472;
M_Index[247]=2480;
M_Index[248]=2494;
M_Index[249]=2506;
M_Index[250]=2513;
M_Index[251]=2518;
M_Index[252]=2536;
M_Index[253]=2542;
M_Index[254]=2549;
M_Index[255]=2550;
M_Index[256]=2552;
M_Index[257]=2576;
M_Index[258]=2580;
M_Index[259]=2595;


}
//*******************************************************************
//Shows bfloat16
void Show_bfloat16(bfloat16 Mybfloat16)
{
	unsigned __int32 Zwischen=Mybfloat16.SgnFrag;
	if (Mybfloat16.Exp<255)
	{
		cout<<(Mybfloat16.SgnFrag>=Pow2Int32[FragLen]?"-":"+")<<(Mybfloat16.Exp>0?"1.":"0.");
		Zwischen=(Zwischen<<(32-FragLen));
		for(int t=0;t<FragLen;t++){cout<<(Zwischen>>31);Zwischen=(Zwischen<<1);}
		cout<<"*2^"<<Mybfloat16.Exp-127<<"\n";
	}
	else
	{
		if ((Mybfloat16.SgnFrag%Pow2Int32[FragLen])==0)
		{
			cout<<(Mybfloat16.SgnFrag>=Pow2Int32[FragLen]?"-Inf\n":"+Inf\n");
		}
		else
		{
			cout<<"NaN\n";
		}
	}
}
//*******************************************************************
//Setzt Suchschritte für reverse Bisektion ohne Überlauf
void ShowPoolSearch(unsigned __int8 PNr, bool ShowUnused)
{
cout<<"Bisection("<<1*PNr<<"): ";for(int t=Pool[PNr].BisecSteps-1;t>=0;t--){cout<<1*Pool[PNr].BisecStep[t]<<" ";}cout<<"\n";
for(unsigned __int32 t=0;t<((ShowUnused==true)?Pool[PNr].PoolsCount:Pool[PNr].PoolsUsed);t++)
{
	cout<<"P "<<1*PNr<<"."<<((t<10)?"0":"")<<t<<" Last: ";
	for(unsigned __int32 u=1000000000;u>9;u/=10){if(Pool[PNr].Last[t]<u)cout<<" ";}
	cout<<Pool[PNr].Last[t]<<" CQty: ";
	for(unsigned __int32 u=100000000;u>9;u/=10){if(Pool[PNr].CodeQty[t]<u)cout<<" ";}
	cout<<Pool[PNr].CodeQty[t]<<" LPNr: ";
	//for(unsigned __int32 u=100000000;u>9;u/=10){if(Pool[PNr].DSQty[t]<u)cout<<" ";}
	if(Pool[PNr].LastPoolNr[t]<10)cout<<" ";
	cout<<1*Pool[PNr].LastPoolNr[t]<<" Len: ";
	//for(unsigned __int32 u=100000000;u>9;u/=10){if(Pool[PNr].DSQty[t]<u)cout<<" ";}
	if(Pool[PNr].CodeLen[t]<10)cout<<" ";
	cout<<1*Pool[PNr].CodeLen[t]<<"\n";

//	cout<<"\n";
//	for(int t=Pool[PNr].BisecSteps-1;t>=0;t--)
//	{
//	cout<<" Step: ";
//	cout<<1*Pool[PNr].BisecStep[t];
//	}
//	cout<<"\n";
	//cout<<"\n";
}
cout<<"\n";
}
//*******************************************************************
int ShowBitstream()
{
	unsigned __int8 MyOutput;
	return 0;
	cout<<"OUT("<<((OutBytes>7)?OutBytes-7:0)<<"): ";
	for(int t=((OutBytes>7)?OutBytes-7:0);t<OutBytes;t++)
	{
		MyOutput=OutData[t];
		if(t+1<OutBytes)
		{
			for(int u=0;u<8;u++)
			{
				cout<<(MyOutput>127)?"1":"0";
				MyOutput=(MyOutput<<1);
			}
		}
		else
		{
			for(int u=0;u<8-OutBitsFree;u++)
			{
				cout<<(MyOutput>127)?"1":"0";
				MyOutput=(MyOutput<<1);
			}
		}
		cout<<" ";
	}
	cout<<"\n";
}