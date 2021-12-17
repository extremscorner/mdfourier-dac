#include <stdint.h>
#include <stdlib.h>
#include <ogc/audio.h>

#if HZ < 48000
#include "mdfourier_dac_32000_pcm.h"
static uintptr_t start = (uintptr_t)mdfourier_dac_32000_pcm;
static uintptr_t end = (uintptr_t)mdfourier_dac_32000_pcm_end;
#else
#include "mdfourier_dac_48000_pcm.h"
static uintptr_t start = (uintptr_t)mdfourier_dac_48000_pcm;
static uintptr_t end = (uintptr_t)mdfourier_dac_48000_pcm_end;
#endif

static void callback(void)
{
	if (start <= end) {
		ptrdiff_t length = end - start;

		if (length > 0xFFFE0)
			length = 0xFFFE0;

		AUDIO_InitDMA(start, length);
		start += length;
	}
}

int main(int argc, char **argv)
{
	AUDIO_Init();
	#if HZ < 48000
	AUDIO_SetDSPSampleRate(AI_SAMPLERATE_32KHZ);
	#else
	AUDIO_SetDSPSampleRate(AI_SAMPLERATE_48KHZ);
	#endif
	AUDIO_RegisterDMACallback(callback);
	callback();
	AUDIO_StartDMA();
	while (AUDIO_GetDMALength());
	while (AUDIO_GetDMABytesLeft());
	AUDIO_StopDMA();
	return EXIT_SUCCESS;
}
