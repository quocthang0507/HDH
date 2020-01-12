#include <iostream.h>
#include <string.h>
#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include <alloc.h>

int ReadDiskBIOS(char *Buff, unsigned Side, unsigned Track, unsigned Sector, int SoSector)
{
	union REGS u, v;
	struct SREGS s;
	int k, i = 0;
	v.x.cflag = 1;
	while ((i < 2) && (v.x.cflag != 0))
	{
		u.h.ah = 0x2;
		u.h.dl = 0; //O dia A		//SO DISK;
		u.h.dh = Side;				//MAT DISK;
		//u.h.cl=Sector;			//SecTor DAU
		//u.h.ch=Track;				//track;
		u.x.cx = Track;
		u.h.al = SoSector;			//So sector can doc;
		s.es = FP_SEG(Buff);
		u.x.bx = FP_OFF(Buff);
		int86x(0x13, &u, &v, &s);
		i++;
	}
	k = v.h.ah;
	return(!v.x.cflag);
}

int main(int argc, char *argv[])
{
	clrscr();
	unsigned char Buff[512];

	if (!ReadDiskBIOS(Buff, 0, 1, 1, 1))
	{
		cout << "\nKhong doc duoc bang tham so dia";
		exit(1);
	}
	cout << "\n Phien ban HDH: ";
	for (int j = 0; j < 8; j++)
		cout << Buff[0x03 + j];
	cout << "\n Kich thuoc Sector :";
	cout << *(unsigned*)(Buff + 0x0b);
	cout << "\n Kich thuoc Cluster :";
	cout << (int)Buff[0x0d];
	cout << "\n So Sector truoc FAT :";
	cout << *(unsigned*)(Buff + 0x0e);
	cout << "\n So FAT :";
	cout << (int)Buff[0x10];
	cout << "\n So Entry cua Root :";
	cout << *(unsigned*)(Buff + 0x11);
	cout << "\nTong so sector truoc dia <32MB: ";
	cout << *(unsigned*)(Buff + 0x13);
	cout << "\nByte chi danh dia: ";
	cout << *(int*)(Buff + 0x15);
	cout << "\nSo sector trong bang FAT: ";
	cout << *(unsigned*)(Buff + 0x16);
	cout << "\nSo sector tren moi track: ";
	cout << *(unsigned*)(Buff + 0x18);
	cout << "\nSo dau doc ghi: ";
	cout << *(unsigned*)(Buff + 0x1A);
	cout << "\nTong so sector dia >32MB: ";
	cout << *(long*)(Buff + 0x20);
	getch();
	return 1;
}


