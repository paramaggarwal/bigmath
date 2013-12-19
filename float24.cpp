/*
Implementation of a 24 byte float

float24
  Bits		Purpose
	0		Sign of mantissa
	1 – 95		Mantissa
	96		Sign of exponent
	97-191		Exponent of 10
*/

#include <iostream>
#include <string>

using namespace std;

class float24 {
	private:
		unsigned char num[24];

		void setBit(int pos, bool val);
		bool checkBit(int pos);
		
		string add(string a, string b);
		string subtract(string a, string b);
		string multiply(string a, string b);
		
		void storeMantissa(string a);
		void storeExponent(string a);
		string getMantissa();
		string getExponent();
		
		bool strby2(string &a);
		void strinto2(string &a);
		string strtobin(string s);
		string bintodec(string a);
		int strtoint(string a);
		
		friend ostream &operator<<(ostream &stream, float24 &ob);
		friend istream &operator>>(istream &stream, float24 &ob);
		friend float24 operator+(float24 a, float24 b);
		friend float24 operator-(float24 a, float24 b);
		friend float24 operator*(float24 a, float24 b);
		
		
	public:
		float24();
		int operator=(string a);
		
		
};

void float24::setBit(int pos, bool val)
{
	int place = pos/8;
	pos %= 8;

	if(val)
		num[place] |= 128 >> pos;
	else
		num[place] &= ~(128 >> pos);
}

bool float24::checkBit(int pos)
{	
	int place = pos/8;
	pos %= 8;

	if((num[place] & (128 >> pos)) == 0)
		return 0;
	else
		return 1;
}

string float24::add(string a, string b)
{
	unsigned char tmp;
	int carry = 0;
	bool signA = false;
	bool signB = false;
	bool signC = false;
	
	if(a[0] == '-') {
		signA = true;
		a.erase(0,1);
	}
	
	if(b[0] == '-') {
		signB = true;
		b.erase(0,1);
	}
	
	if(a.size() < b.size()) {
		while(a.size() != b.size())
			a = '0' + a;
	} else {
		while(a.size() != b.size())
			b = '0' + b;
	}
	
	if(signA && signB) {
		signC = true;
	} else if(signA) {
		for(int i=0; i<a.size(); i++)
			a[i] = (9-(a[i]-48)) + 48;
		a = add(a, "1");
	} else if(signB) {
		for(int i=0; i<b.size(); i++)
			b[i] = (9-(b[i]-48)) + 48;
		b = add(b, "1");
	}
	
	for(int i=a.size()-1; i>=0; i--) {
		tmp = b[i];
		
		b[i] = ((a[i]-48) + (b[i]-48) + carry)%10 + 48;		
		carry = ((a[i]-48) + (tmp-48) + carry)/10;

		if(i==0 && carry != 0) {
			tmp = carry + 48;
			b = (char)tmp + b;
		}
	}

	if(signC)
		b = '-' + b;
	
	if(signA ^ signB) {
		if(b[0] == '1') {
			b.erase(0,1);
		} else {
			for(int i=0; i<b.size(); i++) {
				b[i] = (9-(b[i]-48)) + 48;
			}
			b = add(b, "1");
			b = '-' + b;
		}
	}
	
	while(b[0] == '0')
		b.erase(0,1);
				
	return b;
}

string float24::subtract(string a, string b)
{
	if(b[0] == '-')
		b.erase(0,1);
	else
		b = '-' + b;
	
	return add(a,b);
}

string float24::multiply(string a, string b)
{
	string ret;
	bool signA = false;
	bool signB = false;
	bool carryFlag = false;
	
	if(a[0] == '-') {
		signA = true;
		a.erase(0,1);
	}
	
	if(b[0] == '-') {
		signB = true;
		b.erase(0,1);
	}
	
	for(int i=a.size()-1; i>=0; i--) {
		int carry = 0;
		string tmp;
		for(int j=b.size()-1; j>=0; j--) {
			tmp = (char)((((b[j]-48)*(a[i]-48) + carry)%10) + 48) + tmp;
			carry = (((b[j]-48)*(a[i]-48) + carry)/10);
			if(j==0) {
				tmp = (char)(carry+48) + tmp;
				if(carry == 1)
					carryFlag = true;
			}
		}
		for(int j=1; j<a.size()-i; j++) {
			tmp = tmp + '0';
		}
		ret = add(tmp, ret);
	}
	
	if(signA ^ signB)
		ret = '-' + ret;
	
	if(carryFlag == true)
		ret = '*' + ret;
		
	return ret;
}

void float24::storeMantissa(string a)
{
	int c = 95;

	for(int i=0; i<a.size(); i++) {
		if(a[i] == '1')
			setBit(c--, 1);
		else
			setBit(c--, 0);
	}
}

void float24::storeExponent(string a)
{
	int c = 191;

	for(int i=0; i<a.size(); i++) {
		if(a[i] == '1')
			setBit(c--, 1);
		else
			setBit(c--, 0);
	}
}

string float24::getMantissa()
{
	string tmp;
	
	for(int i=1; i<=95; i++)
		tmp.push_back(checkBit(i) + 48);

	tmp = bintodec(tmp);

	if(checkBit(0) == 1)
		tmp = '-' + tmp;
	
	return tmp;
}

string float24::getExponent()
{
	string tmp;
	
	for(int i=96; i<=191; i++)
		tmp.push_back(checkBit(i) + 48);

	tmp = bintodec(tmp);

	if(checkBit(96) == 1)
		tmp = '-' + tmp;
	
	return tmp;
}

bool float24::strby2(string &a)
{
	bool rem;
	bool carry=0;
	char tmp;

	for(int i=0; i<a.size(); i++) {
		tmp = a[i];
		
		a[i] = ((carry*10 + a[i] - 48)/2) + 48;
		
		if(tmp%2)
			carry = 1;
		else
			carry = 0;
	}

	if(a[0] == '0')
		a.erase(0,1);
	
	return carry;
}

void float24::strinto2(string &a)
{
	unsigned char tmp;
	int carry = 0;
	
	for(int i=a.size()-1; i>=0; i--) {
		tmp = a[i];
		
		a[i] = ( ( ( a[i]-48 )*2 + carry )%10) + 48;
		carry = ((tmp-48)*2 + carry)/10;
		
		if(i == 0 && carry != 0)
			a = (char)(carry + 48) + a;
	}
}

string float24::strtobin(string s)
{
	string ret = "";
	
	while(!s.empty())
		ret.push_back(strby2(s) + 48);

	while(ret.size() != 91)
		ret += '0';
	
	return ret;
}

string float24::bintodec(string a)
{
	string tmp = "1";
	string ret;
	
	for(int i=a.size()-1; i>=0; i--) {	
		if(a[i] == '1') {
			ret = add(ret, tmp);
		}
		strinto2(tmp);
	}
	
	return ret;
}

int float24::strtoint(string a)
{
	int ret = 0;
	bool flag = false;
	
	if(a[0] == '-') {
		a.erase(0,1);
		flag = true;
	}
	
	for(int i=0; i<a.size(); i++)
		ret = 10*ret + a[i] - 48;
	
	if(flag)
		ret = -ret;

	return ret;
}

ostream &operator<<(ostream &stream, float24 &ob)
{
	string tmp = "";
	string mant = ob.getMantissa();
	string exp = ob.getExponent();
	
	if(mant == "")
		mant = "0";
	
	if(mant[0] == '-') {
		tmp = tmp + '-' + mant[1] + '.';
		mant.erase(0, 2);
	} else {
		tmp = tmp + mant[0] + '.';
		mant.erase(0,1);
	}
	
	tmp = tmp + mant;
	
	if(exp != "")
		tmp = tmp + 'E' + exp;
	
	stream << tmp;
	
	return stream;
}

istream &operator>>(istream &stream, float24 &ob)
{
	string tmp;
	stream >> tmp;
	
	ob = tmp;
	
	return stream;
}


float24 operator+(float24 a, float24 b)
{
	float24 ret;
	
	string mant1 = a.getMantissa();
	string mant2 = b.getMantissa();
	string mant;
	
	string exp1 = a.getExponent();
	string exp2 = b.getExponent();
	string exp;
	
	int diff = ret.strtoint(ret.subtract(exp1, exp2));
	
	if(diff > 15) {
		mant = mant1;
		exp = exp1;
	} else if(diff < -15) {
		mant = mant2;
		exp = exp2;
	} else {
		if(diff > 0) {
			exp = exp1;
			for(int i=0; i<diff; i++)
				mant2 = '0' + mant2;
		} else {
			exp = exp2;
			for(int i=0; i>diff; i--)
				mant1 = '0' + mant1;
		}
		
		if(mant1.size() < mant2.size()) {
			while(mant1.size() != mant2.size())
				mant1 += '0';
		} else {
			while(mant1.size() != mant2.size())
				mant2 += '0';
		}

		mant = ret.add(mant1, mant2);
	}
	
	if (mant[0] == '-')
		ret.setBit(0, 1);
	else
		ret.setBit(0, 0);
	
	if(exp[0] == '-')
		ret.setBit(96, 1);
	else
		ret.setBit(96, 0);
		
	if(mant[0] == '-')
		mant.erase(0,1);
	if(exp[0] == '-')
		exp.erase(0,1);
	
	mant = ret.strtobin(mant);
	exp = ret.strtobin(exp);
	
	ret.storeMantissa(mant);
	ret.storeExponent(exp);
	
	return ret;
}

float24 operator-(float24 a, float24 b)
{
	float24 ret;
	
	string mant1 = a.getMantissa();
	string mant2 = b.getMantissa();
	string mant;
	string exp1 = a.getExponent();
	string exp2 = b.getExponent();
	string exp;
	int diff = ret.strtoint(ret.subtract(exp1, exp2));
	
	if(diff > 15) {
		mant = mant1;
		exp = exp1;
	} else if(diff < -15) {
		mant = mant2;
		exp = exp2;
	} else {
		if(diff > 0) {
			exp = exp1;
			for(int i=0; i<diff; i++)
				mant2 = '0' + mant2;
		} else {
			exp = exp2;
			for(int i=0; i>diff; i--)
				mant1 = '0' + mant1;
		}
		
		if(mant1.size() < mant2.size()) {
			while(mant1.size() != mant2.size())
				mant1 += '0';
		} else {
			while(mant1.size() != mant2.size())
				mant2 += '0';
		}

		mant = ret.subtract(mant1, mant2);
	}
	
	if (mant[0] == '-')
		ret.setBit(0, 1);
	else
		ret.setBit(0, 0);
	
	if(exp[0] == '-')
		ret.setBit(96, 1);
	else
		ret.setBit(96, 0);
		
	if(mant[0] == '-')
		mant.erase(0,1);
	if(exp[0] == '-')
		exp.erase(0,1);
	
	mant = ret.strtobin(mant);
	exp = ret.strtobin(exp);
	
	ret.storeMantissa(mant);
	ret.storeExponent(exp);
		
	return ret;
}

float24 operator*(float24 a, float24 b)
{
	float24 ret;
	
	string mant1 = a.getMantissa();
	string mant2 = b.getMantissa();
	string mant = ret.multiply(mant1, mant2);
	
	string exp1 = a.getExponent();
	string exp2 = b.getExponent();
	string exp = ret.add(exp1, exp2);
	
	if(mant[0] == '*') {
		exp = ret.add(exp, "1");
		mant.erase(0,1);
	}
	
	if(mant[0] == '-')
		ret.setBit(0, 1);
	else
		ret.setBit(0, 0);
	
	if(exp[0] == '-')
		ret.setBit(96, 1);
	else
		ret.setBit(96, 0);
		
	if(mant[0] == '-')
		mant.erase(0,1);
	if(exp[0] == '-')
		exp.erase(0,1);
	
	mant = ret.strtobin(mant);
	exp = ret.strtobin(exp);
	
	ret.storeMantissa(mant);
	ret.storeExponent(exp);
		
	return ret;
}

float24::float24()
{
	for(int i=0; i<24; i++)
		num[i] = 0;
}

int float24::operator=(string a)
{
	if(a.find('E') == -1) {
		if((a.find('.') == -1)) {
			string exp = "0";
			int size = a.size() - 1;
			if(a[0] == '-')
				size--;
			string tmp = "";
			tmp += (char)(size+48);
			exp = add(exp,tmp);
			a = a + 'E' + exp;
		} else {
			a += "E0";
			a.erase(a.find('.'),1);
		}
	} else {
		if((a.find('.') != -1))
			a.erase(a.find('.'),1);
	}
	
	string mantissa = a.substr(0, a.find('E'));
	string exponent = a.substr(a.find('E'), a.size() - a.find('E'));
	
	if(mantissa[0] == '-')
		mantissa.erase(0,1);
	
	if(exponent[ exponent.find('E') + 1 ] == '-')
		exponent.erase(exponent.find('E'), 2);
	else
		exponent.erase(exponent.find('E'), 1);

	mantissa = strtobin(mantissa);
	exponent = strtobin(exponent);
	
	storeMantissa(mantissa);
	storeExponent(exponent);
	
	if (a[0] == '-')
		setBit(0, 1);
	else
		setBit(0, 0);
	
	if(a[ a.find('E')+1 ] == '-')
		setBit(96, 1);
	else
		setBit(96, 0);
		
	return 0;
}

int main()
{
	float24 a;
	cin >> a;
	
	float24 b;
	cin >> b;
	
	float24 mult;
	float24 sum;
	float24 diff;
	
	sum = a + b;
	diff = a - b;
	mult = a * b;
	
	cout << "Sum: " << sum << endl;
	cout << "Diff: " << diff << endl;
	cout << "Mult: " << mult << endl;
		
	return 0;
}
