typedef enum {
  SHORT_TYPE,
  INT_TYPE,
  LONG_TYPE,
  DOUBLE_TYPE,
  CHAR_TYPE,
  VCHAR_TYPE,
  FLOAT_TYPE
} FIELD_TYPE;

typedef struct _TABLE_DEF_
{
  char  field_name[64];
  FIELD_TYPE type;
  int   length;
  int   offset;
} TABLE_DEF;

void build_update( char *, char *, TABLE_DEF [], int, char * );
