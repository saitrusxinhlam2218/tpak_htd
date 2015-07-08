/* this include file defines teh length of temporary strings. These should be big (much bigger than beeded) to avoid
   bus error becourse of a broken range, This can sometimes happen in sprintf and strcpy calls. By making it far bigger
   than normaly needed changes are big that dispatch crashes of this reason will be avoided. */

#define TMP_STR_LEN 4096
