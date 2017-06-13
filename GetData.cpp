#include "structure.h"

int iChromsomeNum;
map<int64_t, int> ChrLocMap;
int64_t GenomeSize, TwoGenomeSize;
vector<Chromosome_t> ChromosomeVec;

int IdentifyHeaderBoundary(char* str, int len)
{
	int i;

	for (i = 1; i < len; i++)
	{
		if (str[i] == ' ' || str[i] == '\t') return i;
	}
	return i - 1;
}

ReadItem_t GetNextEntry(FILE *file)
{
	ssize_t len;
	size_t size = 0;
	ReadItem_t read;
	char *buffer = NULL;

	read.header = read.seq = NULL; read.rlen = 0;

	if ((len = getline(&buffer, &size, file)) != -1)
	{
		len = IdentifyHeaderBoundary(buffer, len) - 1; read.header = new char[len + 1];
		strncpy(read.header, (buffer + 1), len); read.header[len] = '\0';

		if (FastQFormat)
		{
			if ((read.rlen = getline(&buffer, &size, file)) != -1)
			{
				read.seq = new char[read.rlen];
				strncpy(read.seq, buffer, read.rlen);
				read.rlen -= 1; read.seq[read.rlen] = '\0';
				getline(&buffer, &size, file); getline(&buffer, &size, file);
			}
			else read.rlen = 0;
		}
		else
		{
			string seq;
			while (true)
			{
				if ((len = getline(&buffer, &size, file)) == -1) break;
				if (buffer[0] == '>')
				{
					fseek(file, 0 - len, SEEK_CUR);
					break;
				}
				else
				{
					buffer[len - 1] = '\0'; seq += buffer;
				}
			}
			if ((read.rlen = (int)seq.length()) > 0)
			{
				read.seq = new char[read.rlen + 1];
				strcpy(read.seq, (char*)seq.c_str());
				read.seq[read.rlen] = '\0';
			}
		}
	}
	free(buffer);

	return read;
}

int GetNextChunk(bool bSepLibrary, FILE *file, FILE *file2, ReadItem_t* ReadArr)
{
	char* rseq;
	int i, iCount = 0;

	while (true)
	{
		if ((ReadArr[iCount] = GetNextEntry(file)).rlen == 0) break;
		ReadArr[iCount].EncodeSeq = new uint8_t[ReadArr[iCount].rlen];
		for (i = 0; i != ReadArr[iCount].rlen; i++) ReadArr[iCount].EncodeSeq[i] = nst_nt4_table[(int)ReadArr[iCount].seq[i]];
		iCount++;

		if (bSepLibrary) ReadArr[iCount] = GetNextEntry(file2);
		else if ((ReadArr[iCount] = GetNextEntry(file)).rlen == 0) break;

		if (bPairEnd)
		{
			rseq = new char[ReadArr[iCount].rlen];
			GetComplementarySeq(ReadArr[iCount].rlen, ReadArr[iCount].seq, rseq);
			copy(rseq, rseq + ReadArr[iCount].rlen, ReadArr[iCount].seq);
			delete[] rseq;
		}
		ReadArr[iCount].EncodeSeq = new uint8_t[ReadArr[iCount].rlen];
		for (i = 0; i != ReadArr[iCount].rlen; i++) ReadArr[iCount].EncodeSeq[i] = nst_nt4_table[(int)ReadArr[iCount].seq[i]];

		iCount++;
		if (iCount == ReadChunkSize || (bPacBioData && iCount == 10)) break;
	}
	return iCount;
}

ReadItem_t gzGetNextEntry(gzFile file)
{
	int len;
	ReadItem_t read;
	char buffer[1024];

	read.header = read.seq = NULL; read.rlen = 0;

	if (gzgets(file, buffer, 1024) != NULL)
	{
		len = IdentifyHeaderBoundary(buffer, strlen(buffer)) - 1; read.header = new char[len + 1]; 
		strncpy(read.header, (buffer + 1), len); read.header[len] = '\0';
		gzgets(file, buffer, 1024); read.rlen = strlen(buffer) - 1; read.seq = new char[read.rlen + 1]; read.seq[read.rlen] = '\0';
		strncpy(read.seq, buffer, read.rlen);

		gzgets(file, buffer, 1024); gzgets(file, buffer, 1024);
	}
	return read;
}

int gzGetNextChunk(bool bSepLibrary, gzFile file, gzFile file2, ReadItem_t* ReadArr)
{
	char* rseq;
	int i, iCount = 0;

	while (true)
	{
		if ((ReadArr[iCount] = gzGetNextEntry(file)).rlen == 0) break;
		ReadArr[iCount].EncodeSeq = new uint8_t[ReadArr[iCount].rlen];
		for (i = 0; i != ReadArr[iCount].rlen; i++) ReadArr[iCount].EncodeSeq[i] = nst_nt4_table[(int)ReadArr[iCount].seq[i]];
		iCount++;

		if (bSepLibrary) ReadArr[iCount] = gzGetNextEntry(file2);
		else if ((ReadArr[iCount] = gzGetNextEntry(file)).rlen == 0) break;

		if (bPairEnd)
		{
			rseq = new char[ReadArr[iCount].rlen];
			GetComplementarySeq(ReadArr[iCount].rlen, ReadArr[iCount].seq, rseq);
			copy(rseq, rseq + ReadArr[iCount].rlen, ReadArr[iCount].seq);
			delete[] rseq;
		}
		ReadArr[iCount].EncodeSeq = new uint8_t[ReadArr[iCount].rlen];
		for (i = 0; i != ReadArr[iCount].rlen; i++) ReadArr[iCount].EncodeSeq[i] = nst_nt4_table[(int)ReadArr[iCount].seq[i]];

		iCount++;
		if (iCount == ReadChunkSize) break;
	}
	return iCount;
}


bool CheckBWAIndexFiles(string IndexPrefix)
{
	fstream file;
	string filename;
	bool bChecked=true;

	filename = IndexPrefix + ".ann"; file.open(filename.c_str(), ios_base::in);
	if(!file.is_open()) return false; else file.close();
	
	filename = IndexPrefix + ".amb"; file.open(filename.c_str(), ios_base::in);
	if(!file.is_open()) return false; else file.close();

	filename = IndexPrefix + ".pac"; file.open(filename.c_str(), ios_base::in);
	if(!file.is_open()) return false; else file.close();

	return bChecked;
}
