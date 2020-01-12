/*Ho va ten: La Quoc Thang
MSSV: 1610207
Lop: CTK40
Ten bai tap: Xuat thong tin cua mot thu muc bao gom kich thuoc that va kich thuoc tren dia*/

#include <iostream.h>
#include <conio.h>
#include <iomanip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <alloc.h>
#include <dos.h>

#pragma region Structs

//BIOS Parameter Block (BPB)
typedef struct
{
	char jmp[3];		//The first byte to JMP SHORT XX NOP
	char Ver[8];		//OEM identifier
	unsigned SecSiz;	//Bytes/sector (in little-endian format)
	char ClustSiz;		//Sectors/cluster
	unsigned ResSec;	//Number of reserved sectors (included Boot Record Sectors)
	char FatCnt;		//Number of File Allocation Tables. Often 2
	unsigned RootSiz;	//Max # of dictionary entries
	unsigned TotSec;	//Total logical sectors
	char Media;			//Indicates the media descriptor type (FAT ID)
	unsigned FatSiz;	//Number of sectors per FAT
	unsigned TrkSec;	//Number of sectors per track
	unsigned HeadCnt;	//Number of heads or sides on the storage media
	unsigned HidSec;	//Number of hidden sectors (LBA)
} EntryBPB;

//Union between Sector array and BPB structure
typedef union {
	char Sec[512];
	EntryBPB Entry;
} UnionBPB;

//The file time
typedef struct
{
	unsigned S : 5;	//Seconds
	unsigned M : 6;	//Minutes
	unsigned H : 5;	//Hours
} Time;

//Union between Creation time and Time structure
typedef union {
	unsigned intTime;
	Time T;
} UnionTime;

//The file date
typedef struct
{
	unsigned D : 5;	//Days
	unsigned M : 4;	//Months
	unsigned Y : 7;	//Years
} Date;

//Union between Creation date and Date structure
typedef union {
	unsigned intDate;
	Date Dat;
} UnionDate;

//Attribute Byte (8 bits)
typedef struct
{
	char ReadOnly : 1;	//Indicates that the file is read only
	char Hidden : 1;	//Indicates a hidden file
	char System : 1;	//Indicates a system file
	char Volume : 1;	//Indicates a special entry containing the disk's volume label
	char SubDir : 1;	//Describes a subdirectory
	char Archive : 1;	//This is the archive flag
	char DR : 2;
} Attribute;

//Union between Attribute char and Attribute structure
typedef union {
	char charAtt;
	Attribute Attr;
} UnionAttribute;

//Regular Directory Entry
typedef struct
{
	char FileName[8];	//The file name, the first byte indicates it's status
	char Ext[3];		//The file name extension
	UnionAttribute Att; //The file attributes
	char DR[10];
	UnionTime Tg;		//The file time
	UnionDate Ng;		//The file date
	unsigned Clust;		//The starting cluster number
	long FileSize;		//The file size
} EntryDir;

//Union between Entry char array and Entry structure
typedef union {
	char Entry[32];
	EntryDir Entdir;
} UnionDir;

//Define a node in linked list
typedef struct Node
{
	void *Data;	//Content of node
	Node *Next;	//The pointer of next node
} NodeType;

//Define a linked list
typedef NodeType *PointerType;

#pragma endregion

#pragma region GlobalScope

char *path;	//Duong dan
char drive;	//O dia
EntryBPB BPB;	//Bang tham so dia
unsigned char *FAT;	//Noi dung bang FAT
long SecStart = 0;	//Sector bat dau

#pragma endregion

#pragma region Functions

/*Chen mot nut vao danh sach
- list: Danh sach lien ket
- lastItem: Phan tu cuoi cung cua danh sach
- item: Phan tu can chen
Tra ve:
- true: Thanh cong
- false: That bai
*/
int InsertLast(PointerType &list, PointerType &lastItem, void *item)
{
	PointerType temp = new NodeType;
	if (!temp)
	{
		cout << "\nLoi cap phat bo nho cho viec chen vao danh sach!";
		return 0;
	}
	temp->Data = item;
	temp->Next = NULL;
	if (list == NULL)
		list = temp;
	else
		lastItem->Next = temp;
	lastItem = temp;
	return 1;
}

/*Xoa toan bo danh sach lien ket
- list: Danh sach can xoa
*/
void DeleteList(PointerType &list)
{
	PointerType p = list->Next;
	while (p != NULL)
	{
		delete list;
		list = p;
		p = p->Next;
	}
	//delete list;
	list = NULL;
}

/*So sanh hai chuoi
- source: Chuoi ban dau
- dest: Chuoi thu hai
Tra ve:
- true: bang nhau
- false: khong bang nhau
*/
int Compare(char *source, char *dest)
{
	for (int i = 0; source[i] != '\0'; i++)
		if (source[i] != dest[i])
			return 0;
	if (dest[i] == ' ')
		return 1;
	else return 0;
}

/*Lay cac thu muc xuat hien trong duong dan, ngoai tru o dia ra
- path: Chuoi duong dan ma nguoi dung nhap vao
Tra ve:
- NULL: Duong dan khong hop le
- !NULL: Danh sach duong dan
*/
PointerType AnalysePath(char *path) {
	PointerType list = NULL, last = NULL;
	char *fileName;
	drive = path[0];
	if (drive >= 97 && drive <= 122)
		drive -= 97;	//Neu la chu thuong
	else if (drive >= 65 && drive <= 90)
		drive -= 65;	//Neu la chu hoa
	else
	{
		cout << "\nKhong ton tai o dia " + drive;
		return NULL;
	}
	if (drive == 2)
		drive = 0x80;
	if (drive == 0 || drive == 1 || drive == (char)0x80)	//Chi chap nhan o A, B, C
	{
		if (path[1] != ':' && path[2] != '\\')
		{
			cout << "\nDuong dan nhap sai";
			return NULL;
		}
		if (path[3] == '\0')
		{
			cout << "\nDuong dan thieu thu muc";
			return NULL;
		}
		int i = 3;
		while (path[i] != '\0')	//Doc den het duong dan
		{
			int j = 0;
			fileName = new char[12];
			if (!fileName)
			{
				cout << "\nLoi cap phat bo nho cho ten file";
				return NULL;
			}
			while (path[i] != '\\' && path[i] != '\0')	//Doc den het ten cua thu muc
			{
				fileName[j] = path[i];
				++i;
				++j;
			}
			fileName = strupr(fileName);	//Doi sang chu hoa
			fileName[j] = '\0';
			j = 0;
			while (fileName[j] != '.' && fileName[j] != '\0')	//Lay ten cua thu muc (khong tinh phan mo rong)
				++j;
			if (fileName[j] == '.')	//Neu no la tap tin
			{
				fileName[j] = ' ';
				++j;
				int k = 3;
				while (fileName[j] != '\0')
				{
					fileName[11 - k] = fileName[j];
					++j;
				}
				fileName[11] = '\0';
				k = 7;
				while (fileName[k] != ' ')
				{
					fileName[k] = ' ';
					k--;
				}
			}
			if (path[i] != '\0')
				i++;
			InsertLast(list, last, fileName);
		}
		return list;
	}
	else
	{
		cout << "\nDuong dan thieu o dia!";
		return NULL;
	}
}

/*Kiem tra mot thu muc con co xuat hien trong thu muc duoc chi dinh
- listEntry: Danh sach entry cua mot thu muc (goc hoac con)
- fileName: Ten cua thu muc con
- dir: Thuc muc tim duoc (ket qua)
Tra ve:
- true: Tim thay
- false: Khong tim thay
*/
int SearchDir(PointerType listEntry, char *fileName, EntryDir &dir)
{
	PointerType p = listEntry;
	while (p != NULL)	//Duyet toan bo entry cua thu muc
	{
		if (Compare(fileName, ((EntryDir *)p->Data)->FileName))
		{
			dir = *(EntryDir *)p->Data;
			return 1;
		}
		p = p->Next;
	}
	return 0;
}

/*Doc dia va cho vao buffer
- buff: Du lieu cua o dia (ket qua)
- side: So mat cua dia
- track: So track cua dia
- sector: Vi tri sector
- number: So luong sector can doc
Tra ve:
- true: Thanh cong
- false: Khong thanh cong
*/
int ReadDiskBIOS(char *buff, unsigned side, unsigned track, unsigned sector, unsigned number)
{
	union REGS u, v;	//Thanh ghi da dung
	struct SREGS s;		//Thanh ghi doan
	int i = 0, k;		//i: So lan doc toi da
	v.x.cflag = 1;		//Co carry
	while (i < 2 && v.x.cflag != 0)
	{
		u.h.ah = 0x2;				//Doc dia - Neu la 0x1: Ghi dia
		u.h.dl = drive;				//O dia - Neu la 80: Dia cung
		u.h.dh = side;
		u.h.cl = sector;			//SecTor DAU
		u.h.ch = track;				//track
		u.x.cx = track;				//Track va Sector bat dau
		u.h.al = number;			//So luong sector can doc
		s.es = FP_SEG(buff);		//Thanh ghi doan cua buff
		u.x.bx = FP_OFF(buff);		//Dia chi offset cua buff
		int86x(0x13, &u, &v, &s);	//Ham ngat 13h
		i++;			//Tang so lan doc
	}
	return !(v.x.cflag);	//Kiem tra loi
}

/*Doc bang tham so dia BPB va cho vao bien toan cuc BPB
*/
void ReadBPB()
{
	UnionBPB temp;
	if (drive == 0 || drive == 1)
		if (!ReadDiskBIOS(temp.Sec, 0, 1, 1, 1))
		{
			cout << "\nKhong doc duoc bang tham so dia";
			return;
		}
	BPB = temp.Entry;
}

/*Doi sector logic sang sector vat ly
- begin: Sector bat dau
- side: So mat cua dia
- track: So track cua dia
- sector: Vi tri sector
Luu y: Phai gan cho bien toan cuc BPB truoc
*/
void Change(long begin, unsigned &side, unsigned &track, unsigned &sector)
{
	unsigned x;
	sector = (unsigned)(1 + begin % BPB.TrkSec);
	side = (unsigned)((begin / BPB.TrkSec) % BPB.HeadCnt);
	track = (unsigned)(begin / (BPB.TrkSec * BPB.HeadCnt));
	x = track;
	x = x & 0xFF00;
	x = x >> 2;
	x = x & 0x00FF;
	x = x | sector;
	track = track << 8;
	track = track | x;
}

/*Doc dia theo tung sector logic
- buff: Du lieu cua o dia (ket qua)
- begin: Sector bat dau
- number: So sector can doc
Tra ve:
- true: Thanh cong
- false: That bai
*/
int ReadDisk(char *buff, long begin, int number)
{
	unsigned side, track, sector;
	Change(begin, side, track, sector);
	if (ReadDiskBIOS(buff, side, track, sector, number))
		return 1;
	else return 0;
}

/*Doc noi dung bang FAT vao bien toan cuc
Luu y: Phai gan gia tri cho bien toan cuc BPB truoc
*/
int ReadFAT()
{
	FAT = new char[BPB.FatSiz * BPB.SecSiz];
	if (!FAT)
	{
		cout << "\nLoi cap phat bo nho";
		return 0;
	}
	if (!ReadDisk(FAT, BPB.ResSec, BPB.FatSiz))
	{
		cout << "\nKhong doc duoc bang tham so dia!";
		return 0;
	}
	return 1;
}

/*Tim cluster ke tiep cua file trong FAT
- index: So hieu cluster
Tra ve: Vi tri cluster tiep theo cua index
Luu y: Phai gan ket qua cho bien toan cuc FAT truoc
*/
unsigned NextEntry(unsigned index)
{
	unsigned add, x, t;
	if (drive == 0 || drive == 1)
	{
		add = index * 3 / 2;
		x = FAT[add];
		t = FAT[add + 1];
		t = t << 8;
		x = x + t;
		if ((index % 2) == 0)
			x = x & 0x0FFF;	//Neu la chan
		else x = x >> 4;	//Neu la le
	}
	else
	{
		add = (index * 2) - 1;
		x = FAT[add];
		t = FAT[add + 1];
		t = t << 8;
		x = x + t;
	}
	return x;
}

/*Lay danh sach cluster cua thu muc goc
Tra ve: Danh sach cluster
Luu y: Phai gan ket qua cho bien toan cuc BPB truoc
*/
PointerType GetClusterRoot()
{
	PointerType listCluster = NULL, last = NULL;
	unsigned rootSec, *cluster, number;
	rootSec = BPB.ResSec + BPB.FatSiz * (int)BPB.FatCnt;
	number = BPB.RootSiz * 32 / 512;
	for (int i = 0; i < number; i++, rootSec++)
	{
		cluster = new unsigned;
		*cluster = rootSec;
		InsertLast(listCluster, last, (unsigned *)cluster);
	}
	return listCluster;
}

/*Lay danh sach cluster dua vao cluster bat dau
- begin: Vi tri cluster bat dau
Tra ve: Danh sach cluster
*/
PointerType GetCluster(unsigned begin)
{
	PointerType listCluster = NULL, last = NULL;
	unsigned *cluster, next = begin;
	for (; next >= 2 && next < 0xFEF; next = NextEntry(*cluster))
	{
		cluster = new unsigned;
		*cluster = next;
		InsertLast(listCluster, last, (unsigned *)cluster);
	}
	return listCluster;
}

/*Doc tung entry ben trong thu muc va cho vao danh sach
- listCluster: Danh sach cluster
- flag:		0 : Thu muc goc
			1 : Thu muc con
Tra ve:
- NULL: Khong thanh cong
- !NULL: Danh sach entry cua thu muc
Luu y: Phai gan gia tri cho bien toan cuc BPB, drive truoc
*/
PointerType GetEntryDir(PointerType listCluster, char flag)
{
	PointerType p = listCluster, listEntry = NULL, last = NULL;
	unsigned size = BPB.ClustSiz * 512, currentCluster, currentSector;
	unsigned char *buff = new unsigned char[size];
	UnionDir *dir;
	while (p != NULL)
	{
		currentCluster = *(unsigned *)p->Data;
		if (flag)
			currentSector = BPB.ResSec + BPB.FatSiz * BPB.FatCnt + (BPB.RootSiz * 32) / 512 + (currentCluster - 2) * BPB.ClustSiz;
		else
			currentSector = currentCluster;
		if (!ReadDisk(buff, currentSector, BPB.ClustSiz))
		{
			cout << "\nKhong doc duoc dia voi danh sach cluster nay";
			return NULL;
		}
		for (int i = 0; i < size;)
		{
			if (buff[i] != (char)0x00)	//Entry khac trong
				if (buff[i] != (char)0xE5)	//Entry khong bi xoa
				{
					int j = 0;
					dir = new UnionDir;
					for (; j < 32; j++, i++)
						dir->Entry[j] = buff[i];
					InsertLast(listEntry, last, &dir->Entdir);
				}
				else i += 32;
			else break;
		}
		p = p->Next;
	}
	delete buff;
	return listEntry;
}

/*Lay kich thuoc tren dia dua vao kich thuoc that
- realSize: Kich thuoc cua file
Tra ve: 
- Kich thuoc tren dia
*/
long GetSizeOnDisk(long realSize)
{
	long size = (long)BPB.SecSiz * BPB.ClustSiz; //Bytes/cluster
	if ((realSize % size) != 0)
		return ((realSize / size) + 1) * size;
	else
		return realSize;
}

/*Hien thi noi dung cua thu muc len man hinh
- listEntry: Danh sach entry can hien thi thong tin
*/
void PrintDir(PointerType listEntry)
{
	PointerType p = listEntry;
	EntryDir dir;
	cout << '\n'
		<< setiosflags(ios::left)
		<< setw(8) << "Name"
		<< setw(8) << "Ext"
		<< setw(16) << "Size (bytes)"
		<< setw(16) << "On disk (bytes)"
		<< setw(8) << "Time"
		<< setw(12) << "Date";
	while (p != NULL)
	{
		dir = *(EntryDir *)p->Data;
		if (dir.Att.Attr.Volume == 0)
		{
			int i;
			cout << "\n";
			for (i = 0; i < 8; i++)
				cout << dir.FileName[i];
			for (i = 0; i < 3; i++)
				cout << dir.Ext[i];
			cout << '\t' << dir.FileSize
				<< "\t\t" << GetSizeOnDisk(dir.FileSize)
				<< "\t\t" << ((dir.Tg).T).H << ':' << ((dir.Tg).T).M
				<< '\t' << ((dir.Ng).Dat).D << '/' << ((dir.Ng).Dat).M << "/" << ((dir.Ng).Dat).Y + 1980;
		}
		p = p->Next;
	}
}
#pragma endregion

#pragma region Main

void ChayChuongTrinh()
{
	path = new char[256];
	cout << "\nNhap duong dan: ";
	scanf("%s", path);
	PointerType listPath = AnalysePath(path);
	if (listPath == NULL)
		return;
	ReadBPB();
	if (!ReadFAT())
		return;
	//Tai thu muc goc
	PointerType listCluster = GetClusterRoot();
	PointerType listEntry = GetEntryDir(listCluster, 0);
	//Tai thu muc con
	while (listPath)
	{
		EntryDir dir;
		if (SearchDir(listEntry, (char *)listPath->Data, dir))
		{
			listCluster = GetCluster(dir.Clust);
			listEntry = GetEntryDir(listCluster, 1);
		}
		else
		{
			cout << "\nKhong tim thay thu muc " << (char *)listPath->Data;
			return;
		}
		listPath = listPath->Next;
	}
	PrintDir(listEntry);
}

int main()
{
	clrscr();
	ChayChuongTrinh();
	getch();
	return 1;
}

#pragma endregion