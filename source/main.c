#include <stdint.h>
#include <stdlib.h>
#include <ogc/audio.h>
#include <ogc/color.h>
#include <ogc/system.h>
#include <ogc/video.h>

static GXRModeObj rmode = {
	.viTVMode  = VI_TVMODE_NTSC_INT,
	.fbWidth   = 704,
	.xfbHeight = 486,
	.viXOrigin = (VI_MAX_WIDTH_NTSC  - 704) / 2,
	.viYOrigin = (VI_MAX_HEIGHT_NTSC - 486) / 2,
	.viWidth   = 704,
	.viHeight  = 486,
	.xfbMode   = VI_XFBMODE_PSF
};

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
	void *xfb = SYS_AllocateFramebuffer(&rmode);

	VIDEO_Init();
	VIDEO_Configure(&rmode);
	VIDEO_ClearFrameBuffer(&rmode, xfb, COLOR_WHITE);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(false);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	VIDEO_WaitVSync();

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
