DB_DIR=/usr/taxi/mads.dbs
ISQL=/usr/informix/bin/isql
    

echo "--Adding/modifying R4.9 Columns--"


echo -n "Update zone  table?(Y/N)"
read ANS
if [ $ANS = "Y" ]
then $ISQL mads - << \ISQL_EOF
alter table zone add (zn_inq_backup1 smallint);
alter table zone add (zn_inq_backup2 smallint);
alter table zone add (zn_inq_backup3 smallint);
alter table zone add (zn_inq_backup4 smallint);
alter table zone add (zn_inq_backup5 smallint);
alter table zone add (zn_inq_backup6 smallint);
alter table zone add (zn_inq_backup7 smallint);
alter table zone add (zn_inq_backup8 smallint);
alter table zone add (zn_inq_backup9 smallint);
alter table zone add (zn_inq_backup10 smallint);
ISQL_EOF
fi

echo -n "Alter table zone 2?(Y/N)"
read ANS
if [ $ANS = "Y" ]
then $ISQL mads - << \ISQL_EOF
alter table zone add (zn_poly_checksum integer);
ISQL_EOF
fi

echo -n "Alter table zone 3?(Y/N)"
read ANS
if [ $ANS = "Y" ]
then $ISQL mads - << \ISQL_EOF
alter table zone drop zn_poly_checksum;
alter table zone add (zn_poly_checksum char(4));
ISQL_EOF
fi

echo -n "Update zone values?(Y/N)"
read ANS
if [ $ANS = "Y" ]
then $ISQL mads - << \ISQL_EOF
update zone set zn_inq_backup1=0,zn_inq_backup2=0,zn_inq_backup3=0,zn_inq_backup4=0,zn_inq_backup5=0,zn_inq_backup6=0,zn_inq_backup7=0,zn_inq_backup8=0,zn_inq_backup9=0,zn_inq_backup10=0,zn_poly_checksum=0;
ISQL_EOF
fi

echo -n "Update zone poly table?(Y/N)"
read ANS
if [ $ANS = "Y" ]
then $ISQL mads - << \ISQL_EOF
alter table zonepoly drop znpoly_chksm;
ISQL_EOF
fi

echo -n "Update zone poly table 2?(Y/N)"
read ANS
if [ $ANS = "Y" ]
then $ISQL mads - << \ISQL_EOF
alter table zonepoly add (znpoly_latitude float);
alter table zonepoly add (znpoly_longitude float);
ISQL_EOF
fi

echo -n "Update zone poly table 3?(Y/N)"
read ANS
if [ $ANS = "Y" ]
then $ISQL mads - << \ISQL_EOF
alter table zonepoly drop znpoly_latitude;
alter table zonepoly drop znpoly_longitude;
alter table zonepoly add (znpoly_clat char(12));
alter table zonepoly add (znpoly_clon char(12));
ISQL_EOF
fi
 
echo -n "Update fleet  table?(Y/N)"
read ANS
if [ $ANS = "Y" ]
then $ISQL mads - << \ISQL_EOF
alter table fleet add (fl_stand_intvl smallint);
alter table fleet add (fl_stand_life smallint);
update fleet set fl_stand_intvl=20,fl_stand_life=120;
ISQL_EOF
fi

