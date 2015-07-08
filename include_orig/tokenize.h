 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  tokenize.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:50:05
 * @(#)  Last delta: 12/2/94 at 18:14:55
 * @(#)  SCCS File: /taxi/sccs/s.tokenize.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:tokenize.h	10.1.2.1"
#define MAX_FUNC_KEYS 64

#define FUNC_KEY0 0410

#define MAX_KEY_CODE_LEN 20
#ifndef ESC
#define ESC 033
#endif
#define DELAY_FOR_FUNCTION_KEYS 550
#define TERM_DIR "/usr/taxi/term"
#define MAX_LINE_LENGTH 300

#define getsh() (wgetsh(stdscr))
#define getsh_or_wait(ms) (wgetsh_or_wait(stdscr, (ms)))
#define getkey() (wgetkey(stdscr))

struct caps {
    char *capname;
    int  code;
};

#ifndef TOKENIZE_SRC
extern struct caps Caps[];
#else
struct caps  Caps[] = {
/* These use the terminfo names */
{"kcud1", 0402}, /* key down */
{"kcuu1", 0403}, /* key up */
{"kcuf1", 0405}, /* key right */
{"kcub1", 0404}, /* key left */
/* Function keys 1 to 32 */
{"kf0", 0410},
{"kf1", 0411},
{"kf2", 0412},
{"kf3", 0413},
{"kf4", 0414},
{"kf5", 0415},
{"kf6", 0416},
{"kf7", 0417},
{"kf8", 0420},
{"kf9", 0421},
{"kf10", 0422},
{"kf11", 0423},
{"kf12", 0424},
{"kf13", 0425},
{"kf14", 0426},
{"kf15", 0427},
{"kf16", 0430},
{"kf17", 0431},
{"kf18", 0432},
{"kf19", 0433},
{"kf20", 0434},
{"kf21", 0435},
{"kf22", 0436},
{"kf23", 0437},
{"kf24", 0440},
{"kf25", 0441},
{"kf26", 0442},
{"kf27", 0443},
{"kf28", 0444},
{"kf29", 0445},
{"kf30", 0446},
{"kf31", 0447},
{"kf32", 0450},
{"kf33", 0451},
{"kf34", 0457},
{"kf35", 0455},

/* English key names */
{"down", KEY_DOWN},
{"up", KEY_UP},
{"right", KEY_RIGHT},
{"left", KEY_LEFT},
/* Function keys 1 to 32 */
{"toggle", FUNC_KEY0 + 1}, /* kf1 */
{"alt_ad", FUNC_KEY0 + 2}, /* kf2 */
{"mult", FUNC_KEY0 + 3}, /* kf3 */
{"update", FUNC_KEY0 + 4}, /* kf4 */
{"send", FUNC_KEY0 + 5}, /* kf5 */
{"msg_send", FUNC_KEY0 + 6}, /* kf6 */
{"detail", FUNC_KEY0 + 7}, /* kf7 */
{"search", FUNC_KEY0 + 8}, /* kf8 */
{"hist", FUNC_KEY0 + 9}, /* kf9 */
{"next_pg", FUNC_KEY0 + 11}, /* kf11 */
{"refresh", FUNC_KEY0 + 12}, /* kf12 */
{"command", FUNC_KEY0 + 13}, /* kf13 */
{"man_zone", FUNC_KEY0 + 14}, /* kf14 */
{"dispatch", FUNC_KEY0 + 15}, /* kf15 */
{"approve", FUNC_KEY0 + 16}, /* kf16 */
{"call_back", FUNC_KEY0 + 17}, /* kf17 */
{"call_disp", FUNC_KEY0 + 19}, /* kf19 */
{"cancel", FUNC_KEY0 + 20}, /* kf20 */
{"send_dup", FUNC_KEY0 + 21}, /* kf21 */
{"msg_read", FUNC_KEY0 + 22}, /* kf22 */
{"prt_scr", FUNC_KEY0 + 23}, /* kf23 */
{"help", FUNC_KEY0 + 24}, /* kf24 */
{"prev_scr", FUNC_KEY0 + 26}, /* kf26 */
{"mdt_emul", FUNC_KEY0 + 28}, /* kf28 */
{"resend", FUNC_KEY0 + 29}, /* kf29 */
{"man_veh", FUNC_KEY0 + 30}, /* kf30 */
{"hold", FUNC_KEY0 + 31}, /* kf31 */
{"disapprove", FUNC_KEY0 + 32}, /* kf32 */
{"flat_rate", FUNC_KEY0 + 33}, /* kf33 */
{"cl_msg", FUNC_KEY0 + 37},
{"manual_rate", 0457},
{"pf1", 0344},
{"send_hold", 0345},
{"send_switch", 0346},
{"pf4", 0347},
{"send_wakeup", FUNC_KEY0 + 40},
{"emerg_bcst", FUNC_KEY0 + 41},
{"make_call", FUNC_KEY0 + 42},

/* Alternate prev and next page and search (for having
   more than one key to do prev and next page) */
{"prev_pg2", FUNC_KEY0 + 10}, /* kf10 */
{"next_pg2", FUNC_KEY0 + 11}, /* kf11 */
{"search2", FUNC_KEY0 + 8}, /* kf8 */

/* Non function key values */
{"is", -1}, /* Initialiation screen for terminal */
{"ts", -1}, /* Start writing to status line */
{"fs", -1}, /* End writing to status line */

{"", 0}       /* Null at end necessary */
};
#endif


struct function_keys {
    char s[MAX_KEY_CODE_LEN + 1]; /* the actual keycodes */
    int len; /* number of chars in s */
    int code; /* Its identifier */
    char name[MAX_KEY_CODE_LEN + 1]; /* terminfo name, followed by null */
};

#ifndef TOKENIZE_SRC
extern struct function_keys Fk[MAX_FUNC_KEYS];
extern char *Init_terminal;
extern char *Status_line_start;
extern char *Status_line_end;
#else
struct function_keys Fk[MAX_FUNC_KEYS];
char *Init_terminal = NULL;
char *Status_line_start = NULL;
char *Status_line_end = NULL;
#endif

/* The actual number of function keys on this terminal */
#ifndef TOKENIZE_SRC
extern int N_func_keys;
#else
int N_func_keys = 0;
#endif

/* The max number of characters a func key produces */
#ifndef TOKENIZE_SRC
extern int Max_func_key_len;
#else
int Max_func_key_len = 0;
#endif
