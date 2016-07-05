// BigNumber.h: interface for the BigNumber class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BIGNUMBER_H__5F2FAC40_34F5_11D6_BF1C_00A0CCC0418B__INCLUDED_)
#define AFX_BIGNUMBER_H__5F2FAC40_34F5_11D6_BF1C_00A0CCC0418B__INCLUDED_

#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class BigNumber  
{
public:
	bool verifyChecksum();
	int computeChecksum(int type);
	BigNumber();
	BigNumber(int v);
	BigNumber(BigNumber *bn);
	BigNumber(int *digits,int nDigits);	//CHANGED BY I.K. fixed declaration
	BigNumber(time_t *t);				//CHANGED BY I.K. fixed declaration
	BigNumber(int y, int m, int d);		//CHANGED BY I.K. fixed declaration
	BigNumber(char *s);					//CHANGED BY I.K. fixed declaration
	void BigNumber::initFromString(char *s);	//CHANGED BY I.K. fixed declaration
	BigNumber(int v,int nDigits);		//CHANGED BY I.K. fixed declaration
	virtual ~BigNumber();
	int BigNumber::getDigit(int p);
	int BigNumber::operator[](int p);
	char *BigNumber::toString();
	char *BigNumber::toFormattedString();
	int BigNumber::toInt();
	int BigNumber::toInt(int s, int l);
	time_t BigNumber::toTime();
	char *BigNumber::toDateString();
	int BigNumber::compare(BigNumber *bn);
	void BigNumber::setDigit(int p, int d);
	void BigNumber::decodeDate();
	void BigNumber::encodeDate();
	void BigNumber::set(BigNumber *bn);
	int BigNumber::pow10(int n);



private:
	int n; //number of digits
	int *d; //digits

};

#endif // !defined(AFX_BIGNUMBER_H__5F2FAC40_34F5_11D6_BF1C_00A0CCC0418B__INCLUDED_)
