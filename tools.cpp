#include "structure.h" 

static const char ReverseMap[255] =
{
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /*   0 -   9 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /*  10 -  19 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /*  20 -  29 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /*  30 -  39 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /*  40 -  49 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /*  50 -  59 */
	'\0', '\0', '\0', '\0', '\0',  'T', '\0',  'G', '\0', '\0', /*  60 -  69 */
	'\0',  'C', '\0', '\0', '\0', '\0', '\0', '\0',  'N', '\0', /*  70 -  79 */
	'\0', '\0', '\0', '\0',  'A',  'A', '\0', '\0', '\0', '\0', /*  80 -  89 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0',  'T', '\0',  'G', /*  90 -  99 */
	'\0', '\0', '\0',  'C', '\0', '\0', '\0', '\0', '\0', '\0', /* 100 - 109 */
	'\0', '\0', '\0', '\0', '\0', '\0',  'A',  'A', '\0', '\0', /* 110 - 119 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /* 120 - 129 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /* 130 - 139 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /* 140 - 149 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /* 150 - 159 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /* 160 - 169 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /* 170 - 179 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /* 180 - 189 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /* 190 - 199 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /* 200 - 209 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /* 210 - 219 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /* 220 - 229 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /* 230 - 239 */
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', /* 240 - 249 */
	'\0', '\0', '\0', '\0', '\0'                                /* 250 - 254 */
};

void GetComplementarySeq(int len, char* seq, char* rseq)
{
	int i, j;

	for (j = len - 1, i = 0; i<j; i++, j--)
	{
		rseq[i] = ReverseMap[(int)seq[j]];
		rseq[j] = ReverseMap[(int)seq[i]];
	}
	if (i == j) rseq[i] = ReverseMap[(int)seq[i]];

	rseq[len] = '\0';
}

void SelfComplementarySeq(int len, char* seq)
{
	int i, j;
	char aa1, aa2;

	for (j = len - 1, i = 0; i<j; i++, j--)
	{
		aa1 = seq[i]; aa2 = seq[j];
		seq[i] = ReverseMap[(int)aa2];
		seq[j] = ReverseMap[(int)aa1];
	}
	if (i == j) seq[i] = ReverseMap[(int)seq[i]];
}

int CalFragPairNonIdenticalBases(int len, char* frag1, char* frag2)
{
	int i, c;

	for (c = 0, i = 0; i < len; i++) if (frag1[i] != frag2[i]) c++;

	return c;
}

int CalFragPairIdenticalBases(int len, char* frag1, char* frag2)
{
	int i, c;

	for (c = 0, i = 0; i < len; i++) if (frag1[i] != frag2[i]) c++;

	return (len - c);
}

int AddNewCigarElements(string& str1, string& str2, vector<pair<int,char> >& cigar_vec)
{
	char state = '*';
	int i, c, len, score = 0;

	for (len = (int)str1.length(), c = 0, i = 0; i < len; i++)
	{
		if (str1[i] == '-')
		{
			if (state == 'D') c++;
			else
			{
				if (c > 0)
				{
					cigar_vec.push_back(make_pair(c, state));
					//if (bDebugMode) printf("%d%c", c, state);
				}
				c = 1; state = 'D';
			}
		}
		else if (str2[i] == '-')
		{
			if (state == 'I') c++;
			else
			{
				if (c > 0)
				{
					cigar_vec.push_back(make_pair(c, state));
					//if (bDebugMode) printf("%d%c", c, state);
				}
				c = 1; state = 'I';
			}
		}
		else
		{
			if (str1[i] == str2[i]) score++;

			if (state == 'M') c++;
			else
			{
				if (c > 0)
				{
					cigar_vec.push_back(make_pair(c, state));
					//if (bDebugMode) printf("%d%c", c, state);
				}
				c = 1; state = 'M';
			}
		}
	}
	if (c > 0)
	{
		cigar_vec.push_back(make_pair(c, state));
		//if (bDebugMode) printf("%d%c", c, state);
	}
	return score;
}

void ShowSeedInfo(vector<SeedPair_t>& SeedPairVec)
{
	for (vector<SeedPair_t>::const_iterator iter = SeedPairVec.begin(); iter != SeedPairVec.end(); iter++)
		if (iter->rLen > 0 || iter->gLen > 0) printf("\t\tseed#%d: R[%d-%d]=%d G[%ld-%ld]=%d Diff=%ld %s\n", (int)(iter - SeedPairVec.begin() + 1), iter->rPos, iter->rPos + iter->rLen - 1, iter->rLen, iter->gPos, iter->gPos + iter->gLen - 1, iter->gLen, iter->PosDiff, (iter->bSimple ? "Simple" : "Normal"));
	printf("\n\n"); fflush(stdout);
}

void ShowSeedLocationInfo(int64_t MyPos)
{
	int64_t gPos;

	map<int64_t, int>::const_iterator iter = ChrLocMap.lower_bound(MyPos);
	if (MyPos < GenomeSize) gPos = MyPos - ChromosomeVec[iter->second].FowardLocation;
	else gPos = iter->first - MyPos;
	printf("\t\tChr [%s, %ld]\n", ChromosomeVec[iter->second].name, gPos);
}

void ShowAlignmentCandidateInfo(bool bFirst, char* header, vector<AlignmentCandidate_t>& AlignmentVec)
{
	vector<AlignmentCandidate_t>::iterator iter;

	printf("\n%s\n", string().assign(100, '-').c_str());

	printf("Alignment Candidate for read_%d: %s\n", bFirst? 1:2, header);
	for (iter = AlignmentVec.begin(); iter != AlignmentVec.end(); iter++)
	{
		if (iter->Score == 0) continue;
		printf("\tcandidate#%d: Score=%d\n", (int)(iter - AlignmentVec.begin() + 1), iter->Score);
		ShowSeedLocationInfo(iter->PosDiff);
		ShowSeedInfo(iter->SeedVec);
	}
	printf("%s\n\n", string().assign(100, '-').c_str());

	fflush(stdout);
}

void GenerateNormalPairAlignment(int rLen, string& frag1, int gLen, string& frag2)
{
	bool bRunNW = true;

	if (rLen > 30 && gLen > 30)
	{
		int i, num, MaxShift;
		string aln1, aln2, str1, str2;
		vector<SeedPair_t> PartitionVec;

		if (bPacBioData)
		{
			if((MaxShift = (rLen > gLen ? (int)(rLen*0.2) : (int)(gLen*0.2))) > 50) MaxShift = 50;
		}
		else MaxShift = MaxGaps;

		PartitionVec = GenerateSimplePairsFromFragmentPair(MaxShift, rLen, (char*)frag1.c_str(), gLen, (char*)frag2.c_str());
		if (PartitionVec.size() > 0) IdentifyNormalPairs(rLen, gLen, PartitionVec);

		if ((num = (int)PartitionVec.size()) > 0)
		{
			bRunNW = false;
			if (bDebugMode) printf("NormalPair Partition1: len1=%d len2=%d\n", rLen, gLen), ShowSeedInfo(PartitionVec);
			
			for (i = 0; i < num; i++)
			{
				// add the alignment of fragment pair in PartitionVec[i];
				//printf("align %d-th pairs: R[%d-%d]=%d, G[%ld-%ld]=%d\n", i + 1, PartitionVec[i].rPos, PartitionVec[i].rPos+ PartitionVec[i].rLen - 1, PartitionVec[i].rLen, PartitionVec[i].gPos, PartitionVec[i].gPos+ PartitionVec[i].gLen -1, PartitionVec[i].gLen), fflush(stdout);
				if (PartitionVec[i].rLen > 0 || PartitionVec[i].gLen > 0)
				{
					if (PartitionVec[i].gLen == 0)
					{
						//iNPindels += PartitionVec[i].rLen;
						aln1 += frag1.substr(PartitionVec[i].rPos, PartitionVec[i].rLen);
						aln2 += string().assign(PartitionVec[i].rLen, '-');
					}
					else if (PartitionVec[i].rLen == 0)
					{
						//iNPindels += PartitionVec[i].gLen;
						aln1 += string().assign(PartitionVec[i].gLen, '-');
						aln2 += frag2.substr(PartitionVec[i].gPos, PartitionVec[i].gLen);
					}
					else
					{
						//printf("substr: r[%d-%d]=%d, g[%d-%d]=%d\n", PartitionVec[i].rPos, PartitionVec[i].rPos + PartitionVec[i].rLen - 1, PartitionVec[i].rLen, PartitionVec[i].gPos, PartitionVec[i].gPos + PartitionVec[i].gLen - 1, PartitionVec[i].gLen), fflush(stdout);
						str1 = frag1.substr(PartitionVec[i].rPos, PartitionVec[i].rLen);
						str2 = frag2.substr(PartitionVec[i].gPos, PartitionVec[i].gLen);
						if (!PartitionVec[i].bSimple)
						{
							if (bPacBioData && (PartitionVec[i].rLen > 300 || PartitionVec[i].gLen > 300)) GenerateNormalPairAlignment(PartitionVec[i].rLen, str1, PartitionVec[i].gLen, str2);
							else
							{
								PairwiseSequenceAlignment(PartitionVec[i].rLen, str1, PartitionVec[i].gLen, str2);
							}
						}

						aln1 += str1; aln2 += str2;
					}
				}
			}
			frag1 = aln1; frag2 = aln2;
		}
	}
	if (bRunNW)
	{
		PairwiseSequenceAlignment(rLen, frag1, gLen, frag2);
	}
}

int ProcessNormalSequencePair(char* seq, SeedPair_t& sp, vector<pair<int, char> >& cigar_vec)
{
	int score = 0;

	if (sp.rLen == 0 || sp.gLen == 0)
	{
		if (sp.rLen > 0) cigar_vec.push_back(make_pair(sp.rLen, 'I')); // insertion
		else if (sp.gLen > 0) cigar_vec.push_back(make_pair(sp.gLen, 'D')); // deletion
	}
	else
	{
		string frag1, frag2;
		frag1.resize(sp.rLen); strncpy((char*)frag1.c_str(), seq + sp.rPos, sp.rLen);
		frag2.resize(sp.gLen); strncpy((char*)frag2.c_str(), RefSequence + sp.gPos, sp.gLen);

		if (sp.rLen == sp.gLen && (sp.rLen - (score = CalFragPairIdenticalBases(sp.rLen, (char*)frag1.c_str(), (char*)frag2.c_str())) <= (sp.rLen < 25 ? 5 : (int)(sp.rLen*.2))))
		{
			cigar_vec.push_back(make_pair(sp.rLen, 'M'));
		}
		else
		{
			GenerateNormalPairAlignment(sp.rLen, frag1, sp.gLen, frag2);
			score = AddNewCigarElements(frag1, frag2, cigar_vec);
		}
		//if (bDebugMode) printf("NormalPair:\n%s #read[%d-%d]=%d\n%s #chr[%ld-%ld]=%d\nScore=%d\n\n", frag1.c_str(), sp.rPos, sp.rPos + sp.rLen - 1, sp.rLen, frag2.c_str(), sp.gPos, sp.gPos + sp.gLen - 1, sp.gLen, score);
	}
	return score;
}

int ProcessHeadSequencePair(char* seq, SeedPair_t& sp, vector<pair<int, char> >& cigar_vec)
{
	int score, thr;
	string frag1, frag2;

	frag1.resize(sp.rLen); strncpy((char*)frag1.c_str(), seq + sp.rPos, sp.rLen);
	frag2.resize(sp.gLen); strncpy((char*)frag2.c_str(), RefSequence + sp.gPos, sp.gLen); 

	//printf("frag1=%s\nfrag2=%s\n", frag1.c_str(), frag2.c_str());
	if (!bPacBioData && sp.rLen == sp.gLen && (sp.rLen - (score = CalFragPairIdenticalBases(sp.rLen, (char*)frag1.c_str(), (char*)frag2.c_str()))) <= (sp.rLen < 25 ? 5 : (int)(sp.rLen*0.2)))
	{
		cigar_vec.push_back(make_pair(sp.rLen, 'M'));
		//if (bDebugMode) printf("Head1:\n%s #read[%d-%d]=%d\n%s #chr[%ld-%ld]=%d\nScore=%d\n\n", frag1.c_str(), sp.rPos, sp.rPos + sp.rLen - 1, sp.rLen, frag2.c_str(), sp.gPos, sp.gPos + sp.gLen - 1, sp.gLen, score);
	}
	else if (!bPacBioData && sp.rLen > 100)
	{
		score = 0;
		cigar_vec.push_back(make_pair(sp.rLen, 'S'));
	}
	else
	{
		GenerateNormalPairAlignment(sp.rLen, frag1, sp.gLen, frag2);
		if (CalFragPairIdenticalBases((int)frag1.length(), (char*)frag1.c_str(), (char*)frag2.c_str()) <= ((int)frag1.length()*0.5))
		{
			cigar_vec.push_back(make_pair(sp.rLen, 'S'));
			score = 0;
		}
		else
		{
			//Case1: -X..X vs XX..X (leading gaps in the read block)
			int p = 0; while (frag1[p] == '-') p++;
			if (p > 0) // shrink the genome block
			{
				frag1.erase(0, p); frag2.erase(0, p);
				sp.gPos += p; sp.gLen -= p;
			}
			//Case2: XX..X vs -X..X (leading gaps in the genome block)
			p = 0; while (frag2[p] == '-') p++;
			if (p > 0) // shrink the read block
			{
				frag1.erase(0, p); frag2.erase(0, p);
				sp.rPos += p; sp.rLen -= p; cigar_vec.push_back(make_pair(p, 'S'));
			}
			score = AddNewCigarElements(frag1, frag2, cigar_vec);
			if (bDebugMode) printf("Head2:\n%s #read[%d-%d]=%d\n%s #chr[%ld-%ld]=%d\nScore=%d\n\n", frag1.c_str(), sp.rPos, sp.rPos + sp.rLen - 1, sp.rLen, frag2.c_str(), sp.gPos, sp.gPos + sp.gLen - 1, sp.gLen, score);
		}
	}
	return score;
}

int ProcessTailSequencePair(char* seq, SeedPair_t& sp, vector<pair<int, char> >& cigar_vec)
{
	int score;
	string frag1, frag2;

	frag1.resize(sp.rLen); strncpy((char*)frag1.c_str(), seq + sp.rPos, sp.rLen);
	frag2.resize(sp.gLen); strncpy((char*)frag2.c_str(), RefSequence + sp.gPos, sp.gLen);

	if (!bPacBioData && sp.rLen == sp.gLen && (sp.rLen - (score = CalFragPairIdenticalBases(sp.rLen, (char*)frag1.c_str(), (char*)frag2.c_str()))) <= (sp.rLen < 25 ? 5 : (int)(sp.rLen*0.2)))
	{
		cigar_vec.push_back(make_pair(sp.rLen, 'M'));
		//if (bDebugMode) printf("Tail1:\n%s #read[%d-%d]=%d\n%s #chr[%ld-%ld]=%d\nScore=%d\n\n", frag1.c_str(), sp.rPos, sp.rPos + sp.rLen - 1, sp.rLen, frag2.c_str(), sp.gPos, sp.gPos + sp.gLen - 1, sp.gLen, score);
	}
	else if (!bPacBioData && sp.rLen > 100)
	{
		score = 0;
		cigar_vec.push_back(make_pair(sp.rLen, 'S'));
	}
	else
	{

		GenerateNormalPairAlignment(sp.rLen, frag1, sp.gLen, frag2);
		if (CalFragPairIdenticalBases((int)frag1.length(), (char*)frag1.c_str(), (char*)frag2.c_str()) <= ((int)frag1.length()*0.5))
		{
			cigar_vec.push_back(make_pair(sp.rLen, 'S'));
			score = 0;
		}
		else
		{
			int p, c;
			//Case1: X..X- vs X..XX (tailing gaps in the read block)
			p = (int)frag1.length() - 1, c = 0; while (frag1[p--] == '-') c++;
			if (c > 0) // shrink the genome block
			{
				frag1.resize((int)frag1.length() - c); frag2.resize((int)frag2.length() - c);
				sp.gLen -= c;
				//if (bDebugMode) printf("find %d tailing gaps in the read block\n", c);
			}
			//Case2: X..XX vs X..X- (tailing gaps in the genome block)
			p = (int)frag2.length() - 1, c = 0; while (frag2[p--] == '-') c++;
			if (c > 0) // shrink the read block
			{
				frag1.resize((int)frag1.length() - c); frag2.resize((int)frag2.length() - c);
				sp.rLen -= c;
				//if (bDebugMode) printf("find %d tailing gaps in the genome block\n", c);
			}
			score = AddNewCigarElements(frag1, frag2, cigar_vec); if (c > 0) cigar_vec.push_back(make_pair(c, 'S'));
			//if (bDebugMode) printf("Tail2:\n%s #read[%d-%d]=%d\n%s #chr[%ld-%ld]=%d\nScore=%d\n\n", frag1.c_str(), sp.rPos, sp.rPos + sp.rLen - 1, sp.rLen, frag2.c_str(), sp.gPos, sp.gPos + sp.gLen - 1, sp.gLen, score);
		}
	}
	return score;
}
