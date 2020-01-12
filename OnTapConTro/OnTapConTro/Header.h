#pragma once
//Code vi du

void VD()
{
	int  bien1 = 15000;   // khai bao bien.
	int  *sv;        // bien con tro sv
	sv = &bien1;       // luu tru dia chi cua bien1 vao bien con tro sv
	cout << "VD:";
	cout << "\nGia tri cua bien1 la: " << bien1;
	// In dia chi duoc luu tru trong bien con tro sv
	cout << "\nDia chi duoc luu tru trong bien con tro sv la: " << sv;
	// Truy cap gia tri co san tai dia chi cua bien con tro
	cout << "\nGia tri cua *sv la: " << *sv;
}

void VD0()
{
	int  bien1;
	char bien2[10];
	cout << "\n\nVD0:";
	cout << "\nDia chi cua bien1 la: " << &bien1;
	cout << "\nDia chi cua bien2 la: " << &bien2;
}

//Code trong sach

void VD1()
{
	char a = 65;
	cout << "\n\nVD1:";
	cout << "\na = 65";
	cout << "\na = " << a;
	cout << "\n&a = " << &a;
}

void VD2()
{
	char *b;
	char t = 65;
	b = &t;
	cout << "\n\nVD2:";
	cout << "\nchar *b";
	//cout << "\nb = new char(65)";
	cout << "\nchar t = 65";
	cout << "\nb = &t";
	cout << "\n*b = " << *b;
	cout << "\nb = " << b;
}

void VD3()
{
	int a = 300;
	char *b;
	b = (char *)a;
	cout << "\n\nVD3:";
	cout << "\nint a = 300";
	cout << "\nchar *b";
	cout << "\nb = (char *)a";
	cout << "\n*b = " << *b;
}