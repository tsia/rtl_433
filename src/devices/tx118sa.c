/* TX118SA-4
 *
 * Tested devices:
 * TX118SA-4
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "decoder.h"

static int tx118sa_callback(r_device *decoder, bitbuffer_t *bitbuffer) {
	bitbuffer_invert(bitbuffer);

	unsigned bits = 0;
	
	uint8_t *b;

	uint32_t device_id = 0;
	uint8_t channel = 0;

	int channels_map[4];
	int channels_on = 0;

	if(bitbuffer->num_rows >= 2){
		bits = bitbuffer->bits_per_row[1];
	}else{
		bits = bitbuffer->bits_per_row[0];
	}
	// fprintf(stdout, "-- number of rows: %d\n", bitbuffer->num_rows);
	// fprintf(stdout, "-- number of bits: %d\n", bits);
	if(bits > 0){
		if(bitbuffer->num_rows >= 2){
			b = bitbuffer->bb[1];
		}else{
			b = bitbuffer->bb[0];
		}
		if(bits != 25){
			//invalid length
			// fprintf(stdout, "invalid length: %d\n", bits);
			return 0;
		}
		// fprintf(stdout, "byte1: %d\n", b[0]);
		// fprintf(stdout, "byte2: %d\n", b[1]);
		// fprintf(stdout, "byte3: %d\n", b[2]);
		if(bits == 24){
			b[2] = b[2] >> 1;
			b[2] += b[1] & 1;
			b[1] = b[1] >> 1;
			b[1] += b[0] & 1;
			b[0] = b[0] >> 1;
		}
		device_id = (b[0] << 12) + (b[1] << 4) + (b[2] >> 4);
		channel = b[2] & 0x0f;
		channels_on = 0;
		for (int j = 0; j < 4; j++) {
			int v = 1 << j;
			if(b[2] & v){
				if(j == 3){ // 1
					channels_map[channels_on] = 2;
				}else if(j == 2){ // 2
					channels_map[channels_on] = 1;
				}else{
					channels_map[channels_on] = 4 - j;
				}
				channels_on++;
			}
		}
	}

	// bitbuffer_print(bitbuffer);

	data_t *data;
	data = data_make("model",         "Model",              DATA_STRING, "TX118SA-4",
					 "id",            "Device ID",          DATA_INT, device_id,
					 "channels",      "Channel",            DATA_ARRAY, data_array(channels_on, DATA_INT, channels_map),
                      NULL);

    decoder_output_data(decoder, data);

	return 1;
}

static char *output_fields[] = {
    "time",
	"type"
    "id",
    "channels",
    NULL
};

r_device tx118sa = {
	.name			= "TX118SA-4",
	.modulation		= OOK_PULSE_PWM,
	.short_width	= 276, //300,
	.long_width		= 852, //916,
	.reset_limit	= 8684, //9464,
	.gap_limit		= 872,
	.sync_width 	= 0, // No sync bit used
	.tolerance  	= 232, // us
	.decode_fn		= &tx118sa_callback,
	.disabled		= 0,
	.fields			= output_fields,
};
