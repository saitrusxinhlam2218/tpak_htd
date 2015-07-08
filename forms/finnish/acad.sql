create table "root".acadtmp 
  (
    acad_fleet char(1),
    acad_account char(18),
    acad_index integer,
    acad_st_nbr integer,
    acad_st_suffix char(1),
    acad_st_pre_dir char(2),
    acad_st_name char(20),
    acad_st_type char(3),
    acad_st_post_dir char(2),
    acad_apartment char(5),
    acad_city_name char(25),
    acad_zip_code char(10),
    acad_phone_nbr char(18),
    acad_phone_ext char(6),
    acad_comment char(30)
  );

create index "root".acad_idx02 on "root".acadtmp (acad_fleet,acad_account,
    acad_index);




