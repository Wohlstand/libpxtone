#include <stdio.h>
#include <stdint.h>
#include <memory>
#include "pxtnService.h"
#include "pxtnError.h"
#include "../files/list.h"

// I/O..
static bool _pxtn_r(void* user, void* p_dst, int32_t size, int32_t num)
{
	int i = fread(p_dst, size, num, (FILE*)user);
	if ( i < num ) {
		return false;
	}
	return true;
}

static bool _pxtn_w(void* user,const void* p_dst, int32_t size, int32_t num)
{
	int i = fwrite(p_dst, size, num, (FILE*)user);
	if ( i < num ) {
		return false;
	}
	return true;
}

static bool _pxtn_s(void* user, int32_t mode, int32_t size)
{
	if (fseek((FILE*)user, size, mode) < 0) {
		return false;
	}
	return true;
}

static bool _pxtn_p(void* user, int32_t* p_pos)
{
	int i = ftell((FILE*)user);
	if( i < 0 ) {
		return false;
	}
	*p_pos = i;
	return true;
}

static bool compare_two_files(const char* file1, const char* file2)
{
	uint8_t byte1, byte2;
	long long bytes = 0;
	bool equal = true;

	FILE *f_src = fopen(file1, "rb");
	if(!f_src)
	{
		fprintf(stderr, "ERROR: Can't open file1 for read: %s\n", file1);
		fflush(stderr);
		return false;
	}

	FILE *f_dst = fopen(file2, "rb");
	if(!f_dst)
	{
		fclose(f_src);
		fprintf(stderr, "ERROR: Can't open file2 for read: %s\n", file2);
		fflush(stderr);
		return false;
	}

	do
	{
		if(fread(&byte1, 1, 1, f_src) < 1)
		{
			printf("ERROR: Quickly reached end of file 1\n");
			fflush(stderr);
			equal = false;
			break;
		}

		if(fread(&byte2, 1, 1, f_dst) < 1)
		{
			printf("ERROR: Quickly reached end of file 2\n");
			fflush(stderr);
			equal = false;
			break;
		}

		if(byte1 != byte2)
		{
			printf("ERROR: Files has different content (position %lld)\n", (long long)bytes);
			fflush(stderr);
			equal = false;
			break;
		}

		if(feof(f_src) != feof(f_dst))
		{
			printf("ERROR: Files has different size\n");
			equal = false;
			fflush(stderr);
			break;
		}

		bytes++;
	} while(feof(f_src) && feof(f_dst));

	fclose(f_src); fclose(f_dst);

	return equal;
}


int main(int argc, char **argv)
{
	std::shared_ptr<pxtnService> pxtn;

	if(argc < 3)
	{
		printf("Syntax: %s [\"source path\"] [\"temp directory\"]\n\n", argv[0]);
		return 1;
	}
#ifdef px_BIG_ENDIAN
	printf("-- BIG ENDIAN\n");
#else
	printf("-- LITTLE ENDIAN\n");
#endif

	const char* src = argv[1];
	const char* dst = argv[2];
	char src_path[1024];
	char dst_path[1024];
	pxtnERR ret;
	int failed_tests = 0;

	for(const char* const* f = s_pxtone_files; *f != NULL; ++f)
	{
		uint16_t exe_version = 996;
		uint8_t exe_version_src[2];
		printf("Testing file %s ... ", *f);
		fflush(stdout);

		sprintf(src_path, "%s/%s", src, *f);
		sprintf(dst_path, "%s/%s", dst, *f);
		FILE *f_src = fopen(src_path, "rb");
		if(!f_src)
		{
			fprintf(stderr, "ERROR: Can't open file for read: %s\n", src_path);
			fflush(stderr);
			failed_tests++;
			continue;
		}

		// Get original EXE version from the file
		fseek(f_src, 16, SEEK_SET);
		fread(exe_version_src, 1, 2, f_src);
		fseek(f_src, 0, SEEK_SET);
		exe_version = (uint16_t)((exe_version_src[0] & 0x00FF) | ((exe_version_src[1] << 8) & 0xFF00));

		printf("[exe version: %u {0x%02X, 0x%02X}] ", exe_version, exe_version_src[0], exe_version_src[1]);
		fflush(stdout);

		FILE *f_dst = fopen(dst_path, "wb");
		if(!f_dst)
		{
			fclose(f_src); fclose(f_dst);
			fprintf(stderr, "ERROR: Can't open file for write: %s\n", dst_path);
			fflush(stderr);
			failed_tests++;
			continue;
		}

		pxtn.reset(new pxtnService(_pxtn_r, _pxtn_w, _pxtn_s, _pxtn_p));

		ret = pxtn->init();
		if(ret != pxtnOK )
		{
			pxtn.reset();
			fclose(f_src); fclose(f_dst);
			fprintf(stderr, "ERROR: Failed to initialize the library: %s\n", pxtnError_get_string(ret));
			fflush(stderr);
			failed_tests++;
			continue;
		}

		if(!pxtn->set_destination_quality(2, 44100))
		{
			pxtn.reset();
			fclose(f_src); fclose(f_dst);
			fprintf(stderr, "ERROR: Failed to set the destination quality");
			fflush(stderr);
			failed_tests++;
			continue;
		}

		// LOAD MUSIC FILE.
		ret = pxtn->read(f_src);
		if(ret != pxtnOK)
		{
			pxtn.reset();
			fclose(f_src); fclose(f_dst);
			fprintf(stderr, "ERROR: Failed to set the source stream: %s\n", pxtnError_get_string(ret));
			fflush(stderr);
			failed_tests++;
			continue;
		}

		ret = pxtn->tones_ready();
		if(ret != pxtnOK)
		{
			pxtn.reset();
			fclose(f_src); fclose(f_dst);
			fprintf(stderr, "ERROR: Failed to initialize tones: %s\n", pxtnError_get_string(ret));
			fflush(stderr);
			failed_tests++;
			continue;
		}

		pxtn->moo_get_total_sample();

		pxtnVOMITPREPARATION prep;
		memset(&prep, 0, sizeof(pxtnVOMITPREPARATION));
		prep.flags |= pxtnVOMITPREPFLAG_loop | pxtnVOMITPREPFLAG_unit_mute;
		prep.start_pos_float = 0;
		prep.master_volume   = 1.0f;

		if(!pxtn->moo_preparation(&prep))
		{
			fclose(f_src); fclose(f_dst);
			pxtn->evels->Release();
			pxtn.reset();
			fprintf(stderr, "ERROR: Failed to initialize the moo\n");
			fflush(stderr);
			failed_tests++;
			continue;
		}

		// Writing final result
		ret = pxtn->write(f_dst, false, exe_version);
		if(ret != pxtnOK)
		{
			fclose(f_src); fclose(f_dst);
			pxtn->evels->Release();
			pxtn.reset();
			fprintf(stderr, "ERROR: Failed to write the destination stream: %s\n", pxtnError_get_string(ret));
			fflush(stderr);
			failed_tests++;
			continue;
		}

		// Closing
		fclose(f_src); fclose(f_dst);
		pxtn->evels->Release();
		pxtn.reset();

		bool equal = compare_two_files(src_path, dst_path);
		if(!equal)
			failed_tests++;

		remove(dst_path);

		if(equal)
		{
			printf("OK!\n");
			fflush(stdout);
		}
	}

	if(failed_tests ==0)
		printf("\nAll tests passed!\n\n");
	else
		printf("\nSome tests had failed: %d!\n\n", failed_tests);
	fflush(stdout);

	return failed_tests;
}
