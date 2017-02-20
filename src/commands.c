#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "commands.h"

typedef enum { UnkCmd, GetCmd, SetCmd } CType;
typedef enum { Integer, Decimal, String } OType;
typedef enum { UnkName, Ts, T1, T2, Id } OName;

int32_t read_num(void* fp) {
	return ((int32_t (*)(void))(fp))();
}

uint8_t write_num(void* fp, int32_t val) {
	return ((uint8_t (*)(int32_t))(fp))(val);
}

void read_str(void* fp, char* str) {
	((void (*)(char*))(fp))(str);
	return;
}

uint8_t write_str(void* fp, const char* str) {
	return ((uint8_t (*)(const char*))(fp))(str);
}

struct Command {
	CType ctype;
	OName oname;
	OType otype;
	void* fp;
};

struct CBuffer {
	char cmdstr[MAX_SLEN];
	char res[MAX_SLEN];
	uint8_t cnt;
};

struct Command commands[] = {
	{ .ctype = GetCmd, .otype = Decimal, .oname = Ts, .fp = NULL },
	{ .ctype = SetCmd, .otype = Decimal, .oname = Ts, .fp = NULL },
	{ .ctype = GetCmd, .otype = Decimal, .oname = T1, .fp = NULL },
	{ .ctype = GetCmd, .otype = Decimal, .oname = T2, .fp = NULL },
	{ .ctype = GetCmd, .otype = Integer, .oname = Id, .fp = NULL }
};

void set_command_handler(Cname cname, void* fp) {
	commands[cname].fp = fp;
}

uint8_t is_valid_char(char c) {
	static char cs[] = { ' ', '_', '.', '\r', '\n', '+', '-' };
	uint8_t i;
	if ((c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122))
		return 1;
	for (i = 0; i < sizeof(cs); ++i)
		if (cs[i] == c)
			return 1;
	return 0;
}

CType get_command_type(const char* str) {
	CType rv = UnkCmd;
	if (!strcmp("get", str))
		rv = GetCmd;
	else if (!strcmp("set", str))
		rv = SetCmd;
	return rv;
}

OName get_command_object_name(const char* str) {
	OName rv = UnkName;
	if (!strcmp("ts", str))
		rv = Ts;
	else if (!strcmp("t1", str))
		rv = T1;
	else if (!strcmp("t2", str))
		rv = T2;
	else if (!strcmp("id", str))
		rv = Id;
	return rv;
}

void num2str(int32_t num, char* str, OType ot) {
	if (ot == Integer) {
		sprintf(str, "%ld", num);
		return;
	}

	char integer[MAX_SLEN];
	char fraction[MAX_SLEN/2-1];
	sprintf(integer, "%ld", num / DEC_DIV);
	sprintf(fraction, DEC_FRM, (num > 0) ? num % DEC_DIV : -num % DEC_DIV);
	strcat(integer, ".");
	strcat(integer, fraction);
	sprintf(str, integer);
}

uint8_t str2num(const char* str, int32_t* num, OType ot) {
	int32_t integer, fraction;
	char* ptr1;
	char* ptr2;

	integer = strtol(str, &ptr1, 10);
	if (ptr1 == str || (ptr1[0] != '.' && ptr1[0] != '\0'))
		return -1;
	else if (ptr1[0] == '.' && ot == Integer)
		return -1;

	*num = integer;
	if (ot == Integer) {
		return 0;
	}
	else if (ot == Decimal && ptr1[1] == '\0') {
		*num *= DEC_DIV;
		return 0;
	}

	fraction = strtol(++ptr1, &ptr2, 10);
	if (ptr2 == ptr1 || ptr2[0] != '\0')
		return -1;

	int32_t dplaces = (ptr2 - ptr1) / sizeof(char);
	if (DEC_PLCS < dplaces)
		while (dplaces-- - DEC_PLCS)
			fraction /= 10;
	else if (DEC_PLCS > dplaces)
		while (DEC_PLCS - dplaces++)
			fraction *= 10;

	*num = integer * DEC_DIV + (integer > 0 ? fraction : -fraction);
	return 0;
}

struct Command* find_command(CType ct, OName on) {
	uint8_t i;
	for (i = 0; i < sizeof(commands) / sizeof(struct Command); ++i)
		if (commands[i].ctype == ct && commands[i].oname == on)
			return &commands[i];
	return NULL;
}

void read_command_token(struct CBuffer* cb, char* tok) {
	uint8_t p = 0;
	// dismiss leading spaces if any;
	while ((cb->cmdstr[cb->cnt] == ' ' || cb->cmdstr[cb->cnt] == '\t') && cb->cnt < MAX_SLEN)
		cb->cnt++;
	while (cb->cmdstr[cb->cnt] != ' ' && cb->cmdstr[cb->cnt] != '\t' && cb->cnt < MAX_SLEN)
		tok[p++] = cb->cmdstr[cb->cnt++];
	tok[p] = '\0';
	return;
}

void reset_command_buffer(struct CBuffer* cb) {
	cb->cnt = 0;
}

void parse_command(struct CBuffer* cb) {
	static char tok[MAX_SLEN];
	reset_command_buffer(cb);

	read_command_token(cb, tok);
	CType ct = get_command_type(tok);

	if (ct == UnkCmd) {
		reset_command_buffer(cb);
		sprintf(cb->res, "%s %s", "unknown command", tok);
		return;
	}

	read_command_token(cb, tok);
	OName on = get_command_object_name(tok);
	if (on == UnkName) {
		reset_command_buffer(cb);
		sprintf(cb->res, "%s %s", "unknown object name", tok);
		return;
	}

	read_command_token(cb, tok);
	struct Command* cmd = find_command(ct, on);
	if (cmd == NULL) {
		sprintf(cb->res, "%s", "command not supported");
	}
	else if (ct == GetCmd && tok[0] != '\0') {
		sprintf(cb->res, "%s", "to many arguments");
	}
	else if (ct == GetCmd) {
		num2str(read_num(cmd->fp), cb->res, cmd->otype);
	}
	else if (ct == SetCmd) {
		int32_t num;
		if (str2num(tok, &num, cmd->otype) != 0)
			sprintf(cb->res, "%s", "invalid set value");
		else if (write_num(cmd->fp, num) == 0)
			sprintf(cb->res, "%s", "ok");
		else
			sprintf(cb->res, "%s", "error");
	}

	reset_command_buffer(cb);
}

const char* update_command_buffer(char c) {
	static struct CBuffer cb = { .cmdstr = "", .cnt = 0 };

	if (!is_valid_char(c)) {
		reset_command_buffer(&cb);
		return "invalid character";
	}
	else if (c == '\n' || c == '\r') {
		cb.cmdstr[cb.cnt] = '\0';
		parse_command(&cb);
		return cb.res;
	}
	else if (cb.cnt < MAX_SLEN) {
		cb.cmdstr[cb.cnt++] = c;
		return "";
	}
	else if (cb.cnt == MAX_SLEN) {
		reset_command_buffer(&cb);
		return "command to long";
	}
	else {
		return "";
	}
}