Index: pi_action.c
===================================================================
RCS file: /taxi-scan/taxi_proj/cvs/taxi-scan/src/pi/pi_action.c,v
retrieving revision 1.34
diff -r1.34 pi_action.c
1932a1933
> #ifdef FOO
1943a1945
> #endif
2785c2787
<           counter,		/* Common loop counter.			*/
---
>           counter, exc_of_this_type,		/* Common loop counter.			*/
2844c2846,2879
<     
---
> 
>     num_of_pck_exc = 0;
>     if ( cur_excep_type == 0 ) // get all exceptions
>       {
> 	for ( i = 0; i < NUM_OF_EXCEP_TYPE; i++ )
> 	  {
> 	    exc_of_this_type = get_excepts(except_ptr, i, 0, 0 );
> 	    for (counter=0; counter < exc_of_this_type; counter++ )
> 	      MakeExceptRecord( &picked_excep[num_of_pck_exc+counter], except_ptr[ counter ] );
> 	    num_of_pck_exc += exc_of_this_type;
> 	  }
> 
> 	/******************************/
> 	/* Build return-data-block.   */
> 	/******************************/
> 	
> 	return_data->data_len = 0;
> 	position = return_data->data;
> 	pi_fail_ok = PI_OK;
> 	
> 	return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
> 
> 
> 	if ( pi_fail_ok == PI_OK )
> 	  {
> 	    return_code = ins_to_data_blk( PI_INT_TYPE, &position, (char *) &num_of_pck_exc, &return_data->data_len );
> 	    for ( counter = 0; counter < num_of_pck_exc; counter++ )
> 	      {
> 		return_code = ins_to_data_blk( sizeof(PI_EXCEPT_REC), &position, (char *) &picked_excep[ counter ], &return_data->data_len );
> 	      }
> 	  }
> 	return(return_code);
>       }
> 
5390a5426,5427
>   char      szLat[11];
>   char      szLong[12];
5393a5431
>   int       iMsgLength = 0;
5401c5439,5442
< 
---
>   struct excepts *except_ptr[ NUM_OF_EXCEPTIONS * 10 ];
>   int num_of_pck_exc = 0;	/* number_of_exceptions picked.		*/
>   short counter;
>   
5406,5413c5447,5455
<   memcpy( &iMobileID, (char *)(order_data->data+2), sizeof(int) );
<   memcpy( &iTime, (char *)(order_data->data+22), sizeof(int) );
<   memcpy( &dLat, (char *)(order_data->data+6), sizeof(double) );
<   memcpy( &dLong, (char *)(order_data->data+14), sizeof(double) );
<   cEmerg = order_data->data[0];
<   cCamTrigger = order_data->data[1];
<   cGPSStatus = order_data->data[26];
<   strncpy( sVehNbr, (char *)(order_data->data + 30), 11 );
---
>   memcpy( &iMsgLength, (char *)(order_data->data + 4), sizeof(int) );
>   memcpy( &iMobileID, (char *)(order_data->data+8), sizeof(int) );
>   memcpy( &iTime, (char *)(order_data->data+48), sizeof(int) );
>   memcpy( szLat, (char *)(order_data->data+15), 11 );
>   memcpy( szLong, (char *)(order_data->data+26), 12 );
>   cEmerg = order_data->data[12];
>   cCamTrigger = order_data->data[13];
>   cGPSStatus = order_data->data[14];
>   strncpy( sVehNbr, (char *)(order_data->data + 38), 11 );
5415,5417c5457,5461
<   cYear = order_data->data[27];
<   cMonth = order_data->data[28];
<   cDay = order_data->data[29];
---
>   cYear = order_data->data[52];
>   cMonth = order_data->data[53];
>   cDay = order_data->data[54];
>   dLat = atof(szLat);
>   dLong = atof(szLong);
5437c5481
<       fwrite( (void *)(order_data->data+41), sizeof(char), order_data->data_len , fpEmergPicture );
---
>       fwrite( (void *)(order_data->data+60), sizeof(char), iMsgLength, fpEmergPicture ); //order_data->data_len , fpEmergPicture );
5491a5536,5545
> 
>   num_of_pck_exc = get_excepts(except_ptr, 1, 0, 0);
>   if ( num_of_pck_exc > 0 ) // got some emergencies to look at
>     {
>       for ( counter = 0; counter < num_of_pck_exc; counter++ )
> 	{
> 	  if ( except_ptr[counter]->veh_nbr == veh_rec.nbr )
> 	    return(return_code);
> 	}
>     }
