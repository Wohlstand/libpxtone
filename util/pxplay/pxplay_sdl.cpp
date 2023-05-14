#include <stdio.h>
#include <SDL2/SDL.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include "pxtnService.h"
#include "pxtnError.h"

/* variable declarations */
static Uint32 is_playing = 0; /* remaining length of the sample we have to play */

struct MidPlayInstance
{
	pxtnService *p;
};

static void stop_playing(int)
{
	is_playing = 0;
}

/*
 audio callback function
 here you have to copy the data of your audio buffer into the
 requesting audio buffer (stream)
 you should only copy as much as the requested length (len)
*/
void my_audio_callback(void *midi_player, Uint8 *stream, int len)
{
	MidPlayInstance *mp = (MidPlayInstance*)midi_player;
	if(!mp->p->Moo(stream, len))
		is_playing = 0;
}


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


int main(int argc, char *argv[])
{
	SDL_AudioSpec spec, spec_obtained; /* the specs of our piece of music */
	MidPlayInstance mp;
	pxtnERR ret;

	signal(SIGINT, &stop_playing);
	signal(SIGTERM, &stop_playing);

	if(argc <= 1)
	{
		printf("%s filename.mid\n", argv[0]);
		return 0;
	}

	FILE *f_src = fopen(argv[1], "rb");
	if(!f_src)
	{
		printf("File open error!\n");
		return 1;
	}

	mp.p = new pxtnService(_pxtn_r, _pxtn_w, _pxtn_s, _pxtn_p);

	ret = mp.p->init();
	if(ret != pxtnOK )
	{
		fclose(f_src);
		fprintf(stderr, "ERROR: Failed to initialize the library: %s\n", pxtnError_get_string(ret));
		fflush(stderr);
		delete mp.p;
		return 1;
	}

	/* Initialize SDL.*/
	if(SDL_Init(SDL_INIT_AUDIO) < 0)
		return 1;

	SDL_memset(&spec, 0, sizeof(SDL_AudioSpec));
	spec.freq = 44100;
	spec.format = AUDIO_S16SYS;
	spec.channels = 2;
	spec.samples = 1024;

	/* set the callback function */
	spec.callback = my_audio_callback;
	/* set ADLMIDI's descriptor as userdata to use it for sound generation */
	spec.userdata = &mp;

	/* Open the audio device */
	if (SDL_OpenAudio(&spec, &spec_obtained) < 0)
	{
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		return 1;
	}

	if(!mp.p->set_destination_quality(spec_obtained.channels, spec_obtained.freq))
	{
		delete mp.p;
		fclose(f_src);
		fprintf(stderr, "ERROR: Failed to set the destination quality");
		fflush(stderr);
		return 1;
	}

	ret = mp.p->read(f_src);
	if(ret != pxtnOK)
	{
		fclose(f_src);
		fprintf(stderr, "ERROR: Failed to set the source stream: %s\n", pxtnError_get_string(ret));
		fflush(stderr);
		delete mp.p;
		return 1;
	}

	ret = mp.p->tones_ready();
	if(ret != pxtnOK)
	{
		fclose(f_src);
		fprintf(stderr, "ERROR: Failed to initialize tones: %s\n", pxtnError_get_string(ret));
		fflush(stderr);
		delete mp.p;
		return 1;
	}

	mp.p->moo_get_total_sample();

	pxtnVOMITPREPARATION prep;
	SDL_memset(&prep, 0, sizeof(pxtnVOMITPREPARATION));
	prep.flags |= pxtnVOMITPREPFLAG_loop | pxtnVOMITPREPFLAG_unit_mute;
	prep.start_pos_float = 0;
	prep.master_volume   = 1.0f;

	if(!mp.p->moo_preparation(&prep))
	{
		mp.p->evels->Release();
		fclose(f_src);
		fprintf(stderr, "ERROR: Failed to initialize the moo\n");
		fflush(stderr);
		delete mp.p;
		return 1;
	}

	is_playing = 1;
	/* Start playing */
	SDL_PauseAudio(0);

	printf("Playing... Hit Ctrl+C to quit!\n");

	/* wait until we're don't playing */
	while(is_playing)
	{
		SDL_Delay(100);
	}

	/* shut everything down */
	SDL_CloseAudio();

	mp.p->evels->Release();
	delete mp.p;
	fclose(f_src);

	return 0 ;
}
