
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <locale.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>
#include "msg_mmp.h"
#include "taxipak.h"
#include "Object.h"
#include "Vehicle_public.h"
#include "path.h"
#include "sprintf.h"
#include "mad_error.h"
#include "dispatch.h"
#include "language.h"
#include "kdt.h"
#include "veh_driv.h"
#include "df_maint.h"
#include "timers.h"
#include "writer.h"
#include "ex_key.h"
#include "except.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "msg.h"
#include "msg_mmp.h"
#include "Call_public.h"
#include "Call_private.h"
#include "Zone_public.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
time_t mads_time;
char *mdt_char_mapping;
char *mdt_map;
char *DISPATCH_catd;
char *scratch_ptr;
extern MSG_QUEUE_HDR outb;
void cleanup() {

}


main (int argc, char **argv )
{

  struct veh_driv vehicle;

  if (argc < 4){
    printf("Usage: mmp_shell <MID> <LINK> <CMD>\n");
    exit(-1);
  }
  mdt_init(0);
  mk_outb_text("");
  outb.delvry_prio = argv[2][0];
  //  add_outb_text("%SZ200096%SZ23000F");
  add_outb_text(argv[3]);
  send_msg_mmp(argv[1], TEXT_DISPLAY, &vehicle);  
  printf("Done...\n");
}
