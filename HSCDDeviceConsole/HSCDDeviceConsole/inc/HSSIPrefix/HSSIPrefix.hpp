#pragma once

//SIê⁄ì™é´
enum struct HSSIPrefix {
	None = 0,	//ñ≥Çµ
	Kiro,		//ÉLÉç
	Mega,		//ÉÅÉK
	Giga,		//ÉMÉK
	Tera,		//ÉeÉâ
	terminal	//èIí[
};

const char HSSIPrefixCharArray [ ( int ) HSSIPrefix::terminal ] = { ' ' , 'K' , 'M' , 'G' , 'T' };
const wchar_t HSSIPrefixCharArrayUnicode [ ( int ) HSSIPrefix::terminal ] = { L' ' , L'K' , L'M' , L'G' , L'T' };

typedef unsigned int HSPrefixUINT;
typedef unsigned long long HSPrefixUINT64;


struct HSPrefixNumber{
	HSSIPrefix Prefix;
	HSPrefixUINT Number;
	HSPrefixUINT PrevPrefixNumber;
};

struct HSPrefixDoubleNumber {
	HSSIPrefix Prefix;
	double Number;
};

HSPrefixNumber HSGetPrefixNumber ( HSPrefixUINT Number );
HSPrefixNumber HSGetPrefixNumber64 ( HSPrefixUINT64 Number );

HSPrefixNumber HSConvToPrefixNumber ( HSPrefixDoubleNumber prefdNum );

HSPrefixDoubleNumber HSGetPrefixDoubleNumber ( HSPrefixUINT Number );
HSPrefixDoubleNumber HSGetPrefixDoubleNumber64 ( HSPrefixUINT64 Number );

HSPrefixDoubleNumber HSConvToPrefixDoubleNumber ( HSPrefixNumber prefnum );

char HSGetPrefixCharA ( HSSIPrefix prefix );
char HSGetPrefixCharA ( HSPrefixNumber prefixnum );
char HSGetPrefixCharA ( HSPrefixDoubleNumber prefixnum );

wchar_t HSGetPrefixCharW ( HSSIPrefix prefix );
wchar_t HSGetPrefixCharW ( HSPrefixNumber prefixnum );
wchar_t HSGetPrefixCharW ( HSPrefixDoubleNumber prefixnum );


#ifdef _UNICODE
#define HSGetPrefixChar HSGetPrefixCharW
#else 
#define HSGetPrefixChar HSGetPrefixCharA
#endif

