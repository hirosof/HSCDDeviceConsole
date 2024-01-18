#include "HSSIPrefix.hpp"

HSPrefixNumber HSGetPrefixNumber ( HSPrefixUINT Number ){
	HSPrefixDoubleNumber dnum;
	dnum = HSGetPrefixDoubleNumber ( Number );
	return HSConvToPrefixNumber(dnum);
}

HSPrefixNumber HSGetPrefixNumber64 ( HSPrefixUINT64 Number ){
	HSPrefixDoubleNumber dnum;
	dnum = HSGetPrefixDoubleNumber64 ( Number );
	return HSConvToPrefixNumber(dnum);
}

HSPrefixNumber HSConvToPrefixNumber ( HSPrefixDoubleNumber prefdNum ){
	HSPrefixNumber num;
	HSPrefixDoubleNumber dnum = prefdNum;
	num.Prefix = dnum.Prefix;
	num.Number = ( HSPrefixUINT ) ( dnum.Number * 1000.0 );
	num.PrevPrefixNumber = num.Number % 1000;
	num.Number /= 1000;
	return num;
}

HSPrefixDoubleNumber HSGetPrefixDoubleNumber ( HSPrefixUINT Number ){
	HSPrefixDoubleNumber num;
	int prefixnums = ( int ) HSSIPrefix::terminal;
	num.Number = Number;
	num.Prefix = HSSIPrefix::None;
	for ( int i = 0; ( i < ( prefixnums - 1 ) ) && ( num.Number >= 1000 ); i++ ) {
		num.Number = num.Number / 1000;
		num.Prefix = ( HSSIPrefix ) ( ( ( int ) num.Prefix ) + 1 );
	}
	return num;
}

HSPrefixDoubleNumber HSGetPrefixDoubleNumber64 ( HSPrefixUINT64 Number )
{
	HSPrefixDoubleNumber num;
	int prefixnums = ( int ) HSSIPrefix::terminal;
	num.Number =(double) Number;
	num.Prefix = HSSIPrefix::None;
	for ( int i = 0; ( i < ( prefixnums - 1 ) ) && ( num.Number >= 1000 ); i++ ) {
		num.Number = num.Number / 1000;
		num.Prefix = ( HSSIPrefix ) ( ( ( int ) num.Prefix ) + 1 );
	}
	return num;
}

HSPrefixDoubleNumber HSConvToPrefixDoubleNumber ( HSPrefixNumber prefnum ){
	HSPrefixDoubleNumber dnum;
	dnum.Prefix = prefnum.Prefix;
	dnum.Number = prefnum.Number;
	dnum.Number += prefnum.PrevPrefixNumber / 1000.0;
	return dnum;
}

char HSGetPrefixCharA ( HSSIPrefix prefix ){
	if ( prefix == HSSIPrefix::terminal ) return ' ';
	return HSSIPrefixCharArray[(int)prefix];
}

char HSGetPrefixCharA ( HSPrefixNumber prefixnum ){
	return HSGetPrefixCharA(prefixnum.Prefix);
}

char HSGetPrefixCharA ( HSPrefixDoubleNumber prefixnum ){
	return HSGetPrefixCharA ( prefixnum.Prefix );
}

wchar_t HSGetPrefixCharW ( HSSIPrefix prefix ){
	if ( prefix == HSSIPrefix::terminal ) return L' ';
	return HSSIPrefixCharArrayUnicode [ ( int ) prefix ];
}

wchar_t HSGetPrefixCharW ( HSPrefixNumber prefixnum ){
	return HSGetPrefixCharW ( prefixnum.Prefix );
}

wchar_t HSGetPrefixCharW ( HSPrefixDoubleNumber prefixnum ){
	return HSGetPrefixCharW ( prefixnum.Prefix );
}
