#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif /* _MSC_VER */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sstream>

#define FOUR_MB (4 * 1024 * 1024)
#define SANITY_LONGEST_WRITE_NUM (16 * 1024 * 1024)

#define HEADER_VAR_IDENTIFIER "PSFLSL_CONFIG_BUILTIN_HEXSTRING"
#define DOUBLEQUOTE "\""

static int hex_encode_char(const char RawChar, char *oHexChar1, char *oHexChar2);
static int hex_encode_buf(
	const char *DataBuf, int LenData,
	char *ioHexDataBuf, int HexDataSize, int *oLenHexData);
static int header_gen(
	const char *HexSrcBuf, int LenHexSrc,
	char *ioHdrSrcBuf, int HdrSrcSize, int *oLenHdrSrc);
static int file_read_small(
	const char *FName,
	char *ioDataBuf, int DataSize, int *oLenData);
static int file_write(
	const char *FNameDst,
	const char *HdrSrcBuf, int LenHdrSrc);
static int file_copy_if_different(const char *FNameSrc, const char *FNameDst);
int main(int argc, char **argv);

int hex_encode_char(const char RawChar, char *oHexChar1, char *oHexChar2)
{
	const char Chars[] = "0123456789ABCDEF";
	int Fst = (RawChar >> 0) & 0x0F;
	int Snd = (RawChar >> 4) & 0x0F;
	*oHexChar1 = Chars[Fst];
	*oHexChar2 = Chars[Snd];
	return 0;
}

int hex_encode_buf(
	const char *DataBuf, int LenData,
	char *ioHexDataBuf, int HexDataSize, int *oLenHexData)
{
	int r = 0;

	if (2 * LenData + 1 /*zero*/ > HexDataSize)
		{ r = 1; goto clean; }

	for (int i = 0; i < LenData; i++)
		if (!!(r = hex_encode_char(
			DataBuf[i],
			&ioHexDataBuf[2*i+0],
			&ioHexDataBuf[2*i+1])))
		{ r = 1; goto clean; }

	memset(&ioHexDataBuf[2*LenData], '\0', 1);

	if (oLenHexData)
		*oLenHexData = LenData * 2;

clean:

	return r;
}

int header_gen(
	const char *HexSrcBuf, int LenHexSrc,
	char *ioHdrSrcBuf, int HdrSrcSize, int *oLenHdrSrc)
{
	int r = 0;

	std::stringstream ss;
	std::string header;

	ss << "/* generated by config_header_gen.cpp - do not edit */" << std::endl;
	ss << "#define " HEADER_VAR_IDENTIFIER << " "
		<< DOUBLEQUOTE << std::string(HexSrcBuf, LenHexSrc) << DOUBLEQUOTE
		<< std::endl;

	header = ss.str();

	if (header.size() + 1 /*zero*/ > HdrSrcSize)
		{ r = 1; goto clean; }

	memmove(ioHdrSrcBuf, header.data(), header.size());
	memset(ioHdrSrcBuf + header.size(), '\0', 1);

	if (oLenHdrSrc)
		*oLenHdrSrc = header.size();

clean:

	return r;
}

int file_read_small(
	const char *FName,
	char *ioDataBuf, int DataSize, int *oLenData)
{
	int r = 0;

	FILE *f = NULL;
	int MaxReadWanted = DataSize - 1;
	int Offset = 0;
	size_t Cnt = 0;
	
	if (!(f = fopen(FName, "rb")))
		{ r = 1; goto clean; }

	while (0 != (Cnt = fread(&ioDataBuf[Offset], 1, MaxReadWanted - Offset, f))) {
		Offset += Cnt;
		if (Offset > MaxReadWanted)
			break;
	}

	if (ferror(f) || !feof(f))
		{ r = 1; goto clean; }

	memset(ioDataBuf + Offset, '\0', 1);

	if (oLenData)
		*oLenData = Offset;

clean:
	if (f)
		fclose(f);

	return r;
}

int file_write(
	const char *FNameDst,
	const char *HdrSrcBuf, int LenHdrSrc)
{
	int r = 0;

	FILE *f = NULL;
	int Offset = 0;
	size_t Cnt = 0;

	if (!(f = fopen(FNameDst, "wb")))
		{ r = 1; goto clean; }

	while (0 != (Cnt = fwrite(&HdrSrcBuf[Offset], 1, LenHdrSrc - Offset, f))) {
		Offset += Cnt;
		if (Offset > LenHdrSrc)
			break;
		if (Offset > SANITY_LONGEST_WRITE_NUM)
			{ r = 1; goto clean; }
	}

	if (ferror(f))
		{ r = 1; goto clean; }

clean:
	if (f)
		fclose(f);

	return r;
}

int file_copy_if_different(const char *FNameSrc, const char *FNameDst)
{
	int r = 0;

	int HaveSrc = false;
	int HaveDst = false;

	char *FileSrcBuf = NULL;
	int LenFileSrc = 0;
	char *FileDstBuf = NULL;
	int LenFileDst = 0;

	char *HexSrcBuf = NULL;
	int LenHexSrc = 0;

	char *HdrSrcBuf = NULL;
	int LenHdrSrc = 0;

	int Equal = 0;
	int NeedWrite = 0;

	if (! (FileSrcBuf = (char *) malloc(FOUR_MB)))
		{ r = 1; goto clean; }

	if (! (FileDstBuf = (char *) malloc(FOUR_MB)))
		{ r = 1; goto clean; }

	if (! (r = file_read_small(FNameSrc, FileSrcBuf, FOUR_MB, &LenFileSrc)))
		HaveSrc = true;

	if (! (r = file_read_small(FNameDst, FileDstBuf, FOUR_MB, &LenFileDst)))
		HaveDst = true;

	if (HaveSrc) {
		if (! (HexSrcBuf = (char *) malloc(2 * FOUR_MB)))
			{ r = 1; goto clean; }
		if (! (HdrSrcBuf = (char *) malloc(3 * FOUR_MB)))
			{ r = 1; goto clean; }

		if (!!(r = hex_encode_buf(FileSrcBuf, LenFileSrc, HexSrcBuf, 2 * FOUR_MB, &LenHexSrc)))
			{ r = 1; goto clean; }

		if (!!(r = header_gen(
			HexSrcBuf, LenHexSrc,
			HdrSrcBuf, 3 * FOUR_MB, &LenHdrSrc)))
			{ r = 1; goto clean; }
	}

	if (HaveDst) {
		/* dummy */
	}

	/* if we had both - can check for equality */
	if (HaveSrc && HaveDst)
		Equal = LenHdrSrc == LenFileDst && !memcmp(HdrSrcBuf, FileDstBuf, LenHdrSrc);

	/* write if have at least src - but no need if have both and are equal */
	NeedWrite = HaveSrc && !Equal;

	if (NeedWrite)
		if (!!(r = file_write(FNameDst, HdrSrcBuf, LenHdrSrc)))
			goto clean;

clean:
	if (HdrSrcBuf)
		free(HdrSrcBuf);
	if (HexSrcBuf)
		free(HexSrcBuf);
	if (FileDstBuf)
		free(FileDstBuf);
	if (FileSrcBuf)
		free(FileSrcBuf);

	return r;
}

int main(int argc, char **argv)
{
	int r = 0;

	if (argc < 3)
		{ r = 1; goto clean; }

	if (!!(r = file_copy_if_different(argv[1], argv[2])))
		goto clean;

clean:

	return !!r ? EXIT_FAILURE : EXIT_SUCCESS;
}
