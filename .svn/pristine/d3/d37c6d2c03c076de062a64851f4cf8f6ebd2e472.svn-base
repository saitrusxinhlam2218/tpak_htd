 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  menu.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:47:49
 * @(#)  Last delta: 12/2/94 at 18:18:25
 * @(#)  SCCS File: /taxi/sccs/s.menu.h
 *                                                                           *
 *****************************************************************************/
#define	MENU_MAX 56		/* max number of records in menu file */
struct menus {
	short mf_nbr;		/*Sequential number assigned at installation 
				 time and used for permission cross-reference.*/
   	short mf_command; 	/* A command string assigned to each menu item.
				  Used by the user with the <CMD> function key 
				  to invoke menu items. */
	char  mf_last_menu_item;/*  "Y"/"N".  Whether this menu item is the 
				  last one on this menu or not. */
   	char mf_type; 		/* A menu item may be a "M"enu, or an 
				 "E"xternal executable, or an "I"nternal 
				  executable.  External executable implies
				  that a UNIX executable string is provided in 
				  mf_unix_string.  Internal executable implies 
				  a function provided by MADS user interface. */
   	char mf_desc[34]; 	/*The actual character description that will be
                      		  displayed on the menu. */
   	char mf_unix_string[41];/*  If this menu item is an External executable 
				  then this field defines the UNIX command 
				  sequence to be executed. */
   char mf_prev_scr_flag;	/* "Y"/"N". Whether this menu choice should be
				  considered a previous screen for the previous
				  screen command funnction key */
}; 
