DB_DIR=/usr/taxi/mads.dbs
ISQL=/usr/informix/bin/isql
 
$ISQL mads - << \ISQL_EOF
update systables set dirpath="zone" where tabname="zone";
update systables set dirpath="fleet" where tabname="fleet";
update systables set dirpath="zonepoly" where tabname="zonepoly";
ISQL_EOF
