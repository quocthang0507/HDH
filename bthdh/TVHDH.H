#include <iostream.h>
#include <string.h>
#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include <alloc.h>

//Partition Entry
typedef struct {
	char Boot;		//0: no bootable, 0x80: bootable/ active
	char HdStart;	//Starting head
	unsigned SecCylStart;	//Starting sector + cylinder
	char Sys;		//Partition Type
	char HdEnd;		//Ending Head
	unsigned SecCylEnd;		//Ending sector + cylinder
	long SecStart;	//Relative Sector
	long TotSec;	//Total Sectors in partition
} EntryPartition;

typedef union {
	unsigned char Ent[16];
	EntryPartition Par;
} UnionEntryPartition;

//BIOS Parameter Block (BPB)
typedef struct {
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

typedef union {
	char Sec[512];
	EntryBPB Entry;
} UnionBPB;

//The file time
typedef struct {
	unsigned S : 5;
	unsigned M : 6;
	unsigned H : 5;
} Time;

typedef union {
	unsigned intTime;
	Time T;
} UnionTime;

//The file date
typedef struct {
	unsigned D : 5;
	unsigned M : 4;
	unsigned Y : 7;
} Date;

typedef union {
	unsigned intDate;
	Date Dat;
} UnionDate;

//Attribute Byte (8 bits)
typedef struct {
	char ReadOnly : 1;	//Indicates that the file is read only
	char Hidden : 1;	//Indicates a hidden file
	char System : 1;	//Indicates a system file
	char Volume : 1;	//Indicates a special entry containing the disk's volume label
	char SubDir : 1;	//Describes a subdirectory
	char Archive : 1;	//This is the archive flag
	char DR : 2;
} Attribute;

typedef union {
	char charAtt;
	Attribute Attr;
} UnionAttribute;

typedef struct {
	char FileName[8];	//The file name, the first byte indicates it's status
	char Ext[3];		//The file name extension
	UnionAttribute Att;	//The file attributes
	char DR[10];
	UnionTime Tg;		//The file time
	UnionDate Ng;		//The file date
	unsigned Clust;		//The starting cluster number
	long FileSize;		//The file size
} EntryDir;

typedef union {
	char Entry[32];
	EntryDir Entdir;
} UnionDir;

typedef struct Node {
	void *Data;
	Node *Next;
} NodeType;

typedef NodeType *PointerType;

void AnalysePath(char *Path, PointerType &ListPath);
EntryBPB ReadBPB();
void ReadFat();
int InsertLast(PointerType &List, PointerType &Last, void *Item);

void Change(long SectorNumber, unsigned int &Side, \
	unsigned int &Track, unsigned int &Sector);
int ReadDisk(char *Buff, char Odia, long SectorNumber, int SoSector);
int ReadDiskBIOS(char *Buff, unsigned Side, unsigned Track, unsigned Sector, int SoSector);
void PrintTo(PointerType &ListEntry);
void DeleteList(PointerType &ListClust);
int SearchDir(PointerType ListEntry, char *FileName, EntryDir &EntDir);
int Compare(char *s, char *d);
PointerType GetEntryDir(PointerType ListClust, char flag);
unsigned NextEntry(unsigned Index);
PointerType GetClusterRoot();
PointerType GetCluster(unsigned StartClust);
void PrintFile(PointerType ListClust, long FileSize);

char *Path;		//="A:\\BT\\BT1"
char Odia;
EntryBPB Bpb;
unsigned char *FAT;
long SecStart = 0;

//***********************************************************************//
//Phan tich duong dan dua vao danh sach cac thu muc ListPathcout
//***********************************************************************//
//void AnalysePath(char *Path, PointerType &ListPath)
//{
//	PointerType Last;
//	ListPath = NULL;
//	Last = ListPath;
//	int i, j;
//	char *FileName;
//	int len = strlen(Path);
//	i = 0;
//	while (Path[i] != '\0')
//	{
//		if ((Path[i] >= 97) && (Path[i] <= 122))
//			Path[i] = Path[i] - 32;
//		i++;
//	}
//
//
//	Odia = Path[0];
//
//	if (Odia >= 97)
//		Odia = Odia - 97;
//	else
//		Odia = Odia - 65;
//	if (Odia == 2)
//		Odia = 0x80;
//
//	if ((Odia == 0) || (Odia == 1) || (Odia = 0x80))
//	{
//		if ((Path[1] != ':') && (Path[2] != '\\'))
//		{
//			cout << "\nDuong dan sai !";
//			return;
//		}
//		if (Path[3] == '\0')
//			return;//danh sach rong dir thu muc goc
//		i = 3;
//		while (Path[i] != '\0')
//		{
//			j = 0;
//			FileName = new char[12];
//			if (!FileName)
//				return;
//			while ((Path[i] != '\\') && (Path[i] != '\0'))
//			{
//				FileName[j] = Path[i];
//				i++;
//				j++;
//			}
//
//			FileName[j] = '\0';
//			//Dinh dang lai file
//			j = 0;
//			while (FileName[j] != '.' && FileName[j] != '\0')
//				j++;
//			if (FileName[j] == '.')
//			{
//				FileName[j] = ' ';
//				j++;
//				k = 3;
//				while (FileName[j] = '\0')
//				{
//					FileName[11 - k] = FileName[j];
//					j++;
//				}
//				FileName[11] = '\0';
//				k = 7
//					while (FileName[k] != ' ')
//					{
//						FileName[k] = ' ';
//						k--;
//					}
//			}
//
//			//
//			if (Path[i] != '\0')
//				i++;
//			cout << "thu" << FileName; getch();
//			InsertLast(ListPath, Last, FileName);
//
//		}
//	}
//	else
//	{
//		cout << "\nDuong dan thieu Odia !";
//		return;
//	}
//}

//***********************************************************************
//Chen mot nut vao danh sach
//List: Danh sach
//Last: Phan tu cuoi
//Item: item can chen
//***********************************************************************
int InsertLast(PointerType &List, PointerType &Last, void *Item)
{
	PointerType Temp;
	Temp = new NodeType;
	if (!Temp)
		return 0;
	Temp->Data = Item;
	Temp->Next = NULL;
	if (List == NULL)
		List = Temp;
	else
		Last->Next = Temp;
	Last = Temp;
	return 1;
}

//***********************************************************************
//Doc noi dung bang tham so dia
//***********************************************************************
EntryBPB ReadBPB()
{
	UnionBPB bpb;
	EntryBPB temp;
	char Buff[512];
	if ((Odia == 0) || (Odia == 1))
		if (!ReadDiskBIOS(Buff, 0, 1, 1, 1))
		{
			cout << "\nKhong doc duoc bang tham so dia";
			exit(1);
		}
	int i = 0;
	while (i < 512)
	{
		bpb.Sec[i] = Buff[i];
		i++;
	}
	temp = bpb.Entry;
	return temp;
}

//***********************************************************************
//Doc noi dung bang fat vao FAT (bien toan cuc)
//***********************************************************************
void ReadFat()
{
	FAT = new char[Bpb.FatSiz * 512];
	if (!FAT)
		exit(1);
	if (!ReadDisk(FAT, Odia, Bpb.ResSec, Bpb.FatSiz))
	{
		cout << "\nKhong doc bang Fat";
		exit(1);
	}
}

//***********************************************************************
//Doi seclog sang sec vat ly
//***********************************************************************
void Change(long SectorNumber, unsigned int &Side, \
	unsigned int &Track, unsigned int &Sector)
{
	unsigned int X;
	Sector = (unsigned)(1 + (SectorNumber) % Bpb.TrkSec);
	Side = (unsigned)(((SectorNumber) / Bpb.TrkSec) % Bpb.HeadCnt);
	Track = (unsigned)((SectorNumber) / (Bpb.TrkSec*Bpb.HeadCnt));
	X = Track;
	X = X & 0xFF00;
	X = X >> 2;
	X = X & 0x00FF;
	X = X | Sector;
	Track = Track << 8;
	Track = Track | X;  // track(10) sector(6)
}

//***********************************************************************
//Doc dia theo tung seclog
//***********************************************************************
int ReadDisk(char *Buff, char Odia, long SectorNumber, int SoSector)
{
	unsigned int Side, Track, Sector;
	Change(SecStart + SectorNumber, Side, Track, Sector);
	if (ReadDiskBIOS(Buff, Side, Track, Sector, SoSector))
		return 1;
	else
		return 0;
}

//***********************************************************************
//Doc dia va cho vao Buff
//***********************************************************************
int ReadDiskBIOS(char *Buff, unsigned Side, unsigned Track, unsigned Sector, int SoSector)
{
	union REGS u, v;
	struct SREGS s;
	int k, i = 0;
	v.x.cflag = 1;
	while ((i < 2) && (v.x.cflag != 0))
	{
		u.h.ah = 0x2;
		u.h.dl = Odia;			//SO DISK
		u.h.dh = Side;			//MAT DISK
		//u.h.cl = Sector;		//SecTor DAU
		//u.h.ch = Track;		//track
		u.x.cx = Track;
		u.h.al = SoSector;		//So sector can doc
		s.es = FP_SEG(Buff);
		u.x.bx = FP_OFF(Buff);
		int86x(0x13, &u, &v, &s);
		i++;
	}
	k = v.h.ah;
	return(!v.x.cflag);
}

//***********************************************************************
//Hien thi noi dung thu muc len man hinh
//***********************************************************************
void PrintTo(PointerType &ListEntry)
{
	PointerType Temp;
	Temp = ListEntry;
	int i;
	EntryDir Dir;
	while (Temp != NULL)
	{
		Dir = *(EntryDir*)(Temp->Data);
		if (Dir.Att.Attr.Volume == 0)
		{
			cout << '\n';
			for (i = 0; i < 8; i++)
				cout << Dir.FileName[i];
			cout << '\t';
			for (i = 0; i < 3; i++)
				cout << Dir.Ext[i];
			cout << '\t';
			cout << Dir.FileSize;

			cout << '\t';
			cout << ((Dir.Tg).T).H << ':' << ((Dir.Tg).T).M;	//<<':'<<((Dir.Tg).T).S

			cout << "\t";
			cout << ((Dir.Ng).Dat).D << '-' << ((Dir.Ng).Dat).M << '-' << 1980 + ((Dir.Ng).Dat).Y;
		}
		Temp = Temp->Next;
	}
	DeleteList(ListEntry);
}

//***********************************************************************
//Xoa danh sach lien ket
//***********************************************************************
void DeleteList(PointerType &ListClust)
{
	PointerType Temp;
	Temp = ListClust->Next;
	while (Temp != NULL)
	{
		delete ListClust;
		ListClust = Temp;
		Temp = Temp->Next;
	}
	delete ListClust;
}

//***********************************************************************
//Tim mot thu muc con co trong thu muc khong
//***********************************************************************
int SearchDir(PointerType ListEntry, char *FileName, EntryDir &EntDir)
{
	unsigned Clust = 0;
	PointerType TempListEntry;
	TempListEntry = ListEntry;
	while (TempListEntry != NULL)
	{
		if (Compare(FileName, ((EntryDir*)(TempListEntry->Data))->FileName))
		{
			EntDir = *(EntryDir*)(TempListEntry->Data);
			return 1;
		}
		TempListEntry = TempListEntry->Next;
	}
	return 0;
}

//***********************************************************************
//So sanh 2 chuoi
//***********************************************************************
int Compare(char *source, char *destination)
{
	int i = 0;
	while (source[i] != '\0')
	{
		if (source[i] != destination[i])
			return 0;
		i++;
	}
	if (destination[i] == ' ')
		return 1;
	else
		return 0;
}

//***********************************************************************
//Tim clust ke tiep cua file trong FAT
//***********************************************************************
unsigned  NextEntry(unsigned Index)
{
	unsigned Addr, X, X1;
	if ((Odia == 0) || (Odia == 1))
	{
		Addr = (Index * 3) / 2;
		X = FAT[Addr];
		X1 = FAT[Addr + 1];
		X1 = X1 << 8;
		X = X1 + X;
		if ((Index % 2) == 0)
			X = X & 0x0FFF;		//Neu la chan
		else
			X = X >> 4;			//Neu la le
	}
	else
	{
		Addr = (Index * 2) - 1;
		X = FAT[Addr];
		X1 = FAT[Addr + 1];
		X1 = X1 << 8;
		X = X1 + X;
	}
	return X;
}

//***********************************************************************
//Lay danh sach cluster thu muc goc
//***********************************************************************
PointerType GetClusterRoot()
{
	PointerType ListClust = NULL, Temp = NULL;
	unsigned SecRoot, *Clust;
	SecRoot = Bpb.ResSec + Bpb.FatSiz*Bpb.FatCnt;
	unsigned NumSec = Bpb.RootSiz * 32 / 512;
	int i = 0;
	while (i < NumSec)
	{
		Clust = new unsigned;
		*Clust = SecRoot;
		InsertLast(ListClust, Temp, (unsigned *)Clust);
		SecRoot++;
		i++;
	}
	return ListClust;
}

//***********************************************************************
//Lay danh sach cluster thu muc con hay tap tin
//***********************************************************************
PointerType GetCluster(unsigned StartClust)
{
	PointerType ListClust = NULL, Temp = NULL;
	unsigned *Clust, NextClust;
	int i = 0;
	NextClust = StartClust;
	while ((NextClust >= 2) && (NextClust < 0xFEF))
	{
		Clust = new unsigned;
		*Clust = NextClust;
		InsertLast(ListClust, Temp, (unsigned*)Clust);
		NextClust = NextEntry(*Clust);
	}
	return ListClust;
}

//***********************************************************************
//Dua tung entry thu muc vao danh sach
//***********************************************************************
PointerType GetEntryDir(PointerType ListClust, char flag)
{
	//flag=0: thu muc goc
	//flag=1: thu muc con
	UnionDir *Dir;
	unsigned char *BuffDir;
	int i, j;
	PointerType ListEntry = NULL, Last = NULL;
	unsigned CurrClust, CurrSec, Size;
	Size = Bpb.ClustSiz * 512;
	BuffDir = new unsigned char[Size];
	while (ListClust != NULL)
	{
		CurrClust = *(unsigned*)ListClust->Data;
		if (flag == 1)
			CurrSec = Bpb.ResSec + Bpb.FatSiz*Bpb.FatCnt + (Bpb.RootSiz * 32) / 512 + (CurrClust - 2)*Bpb.ClustSiz;
		else
			CurrSec = CurrClust;
		if (!ReadDisk(BuffDir, Odia, CurrSec, Bpb.ClustSiz))
		{
			cout << "\nKhong doc dia";
			exit(1);
		}
		i = 0, j = 0;
		while (i < Size)
		{
			if (BuffDir[i] != 0)
			{
				if (BuffDir[i] != 0xE5)
				{
					j = 0;
					Dir = new UnionDir;
					while (j < 32)
					{
						Dir->Entry[j] = BuffDir[i];
						j++;
						i++;
					}
					InsertLast(ListEntry, Last, &Dir->Entdir);
				}
				else
					i = i + 32;
			}
			else
				break;
		}
		ListClust = ListClust->Next;
	}
	delete BuffDir;
	return ListEntry;
}

//***********************************************************************
//Xuat noi dung file
//***********************************************************************
void PrintFile(PointerType ListClust, long FileSize)
{
	PointerType Temp;
	int i, j;
	char *Buff;
	unsigned CurrSec, CurrClust;
	Buff = new char[Bpb.SecSiz * Bpb.ClustSiz];
	Temp = ListClust;
	j = 0;
	while (Temp)
	{
		CurrClust = *(unsigned*)ListClust->Data;
		CurrSec = Bpb.ResSec + Bpb.FatSiz*Bpb.FatCnt + (Bpb.RootSiz * 32) / 512 + (CurrClust - 2)*Bpb.ClustSiz;
		if (!ReadDisk(Buff, Odia, CurrSec, Bpb.ClustSiz))
		{
			cout << "\nKhong doc bang Fat";
			exit(1);
		}
		i = 0;
		while (i < Bpb.SecSiz * Bpb.ClustSiz && j < FileSize)
		{
			cout << Buff[i];
			i++;
			j++;
		}
	}
	return;
}

//
void AnalysePath(char *Path, PointerType &ListPath)
{
	PointerType Last;
	ListPath = NULL;
	Last = ListPath;
	int i, j, k;
	char *FileName;
	int len = strlen(Path);
	i = 0;
	while (Path[i] != '\0')
	{
		if ((Path[i] >= 97) && (Path[i] <= 122))
			Path[i] = Path[i] - 32;
		i++;
	}
	Odia = Path[0];
	if (Odia >= 97)
		Odia = Odia - 97;
	else
		Odia = Odia - 65;
	if (Odia == 2)
		Odia = 0x80;
	if ((Odia == 0) || (Odia == 1) || (Odia = 0x80))
	{
		if ((Path[1] != ':') && (Path[2] != '\\'))
		{
			cout << "\nDuong dan sai !";
			return;
		}
		if (Path[3] == '\0')
			return;		//danh sach rong dir thu muc goc
		i = 3;
		while (Path[i] != '\0')
		{
			j = 0;
			FileName = new char[12];
			if (!FileName)
				return;
			while ((Path[i] != '\\') && (Path[i] != '\0'))
			{
				FileName[j] = Path[i];
				i++;
				j++;
			}
			FileName[j] = '\0';
			//Dinh dang lai file
			j = 0;
			while (FileName[j] != '.' && FileName[j] != '\0')
				j++;
			if (FileName[j] == '.')
			{
				FileName[j] = ' ';
				j++;
				k = 3;
				while (FileName[j] = '\0')
				{
					FileName[11 - k] = FileName[j];
					j++;
				}
				FileName[11] = '\0';
				k = 7;
				while (FileName[k] != ' ')
				{
					FileName[k] = ' ';
					k--;
				}
			}
			//
			if (Path[i] != '\0')
				i++;
			//	cout<<"thu"<<FileName;getch();
			InsertLast(ListPath, Last, FileName);
		}
	}
	else
	{
		cout << "\nDuong dan thieu O dia !";
		return;
	}
}
