extern "C" {
#include <libavcodec\avcodec.h>
}

bool libInit = false;

void init_av_codec() {
	if (!libInit) {
		/* register all the codecs */
		avcodec_register_all();
		libInit = true;
	}
}