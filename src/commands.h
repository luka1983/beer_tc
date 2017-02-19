#include <string.h>

#define MAX_SLEN 64
#define DEC_DIV 1000L
#define DEC_PLCS 3
#define DEC_FRM "%03ld"

// enum memebers correspond to command indexes in commands array
typedef enum {
	GetTs = 0,	// read set temperature
	SetTs		// write set temperature
} Cname;

void set_command_handler(Cname cname, void* fp);
const char* update_command_buffer(char c);