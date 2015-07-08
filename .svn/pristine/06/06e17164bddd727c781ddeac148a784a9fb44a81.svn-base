Index: pi_struc.h
===================================================================
RCS file: /taxi-scan/taxi_proj/cvs/taxi-scan/src/pi/pi_struc.h,v
retrieving revision 1.15
diff -r1.15 pi_struc.h
353,369c353,367
<   /* char Head;                                        0     */
<   /* short Len;                     0 + sizeof(char) = 1     */
<   /* char PacNum;                   1 + sizeof(short) = 3    */
<   /* char Fbyte;                    3 + sizeof(char) = 4     */
<   /* char Sbyte;                    4 + sizeof(char) = 5     */
<   char cEmergency;               // 0
<   char cCamTriggerSource;        // 0 + sizeof(char) = 1
<   int  iMobileID;                // 1 + sizeof(char) = 2
<   double szLatitude;             // 2 + sizeof(int) =  6
<   double szLongitude;            // 6 + sizeof(double) = 14
<   int  sTime;                    // 14 + sizeof(double) = 22
<   char cGPSStatus;               // 22 + sizeof(int) = 26
<   char cYear;                    // 23 + sizeof(char) = 27
<   char cMonth;                   // 27 + sizeof(char) = 28
<   char cDay;                     // 28 + sizeof(char) = 29
<   char szVehicleID[11];          // 29 + sizeof(char) = 30
<                 // start of picture 30 + 11 = 41
---
>   int iMessageType;              // 0
>   int iMessageLength;            // 0 + sizeof(int) = 4
>   int  iMobileID;                // 4 + sizeof(int) = 8
>   char cEmerg;               // 8 + sizeof(int) = 12
>   char cCamTrigger;        // 12 + sizeof(char) = 13
>   char cGPSStatus;               // 13 + sizeof(char) = 14
>   char szLat[11];           // 14 + sizeof(char) = 15
>   char szLong[12];          // 15 + 11 = 26
>   char szVehicleID[11];          // 26 + 12 = 38
>   int  iTime;                    // 38 + 11 = 49
>   char cYear;                    // 49 + sizeof(int) = 53
>   char cMonth;                   // 53 + sizeof(char) = 54
>   char cDay;                     // 54 + sizeof(char) = 55
> 
>                 // start of picture 55 + 1 = 56
