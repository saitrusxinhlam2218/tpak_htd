#!/usr/local/bin/perl
#
# @(#) simcheck
#
# Author:      Hans Ottosson
# Date:        2004-06-21
# Copyright:   (c) Digital Dispatch Scandinavia AB, 2004
######################################################################################################

# Get list of DDS vehicles
&GetListOfDDSvehicles;

open(OUT, ">> /taxi-scan/work/fin_scratch/simcheck.log") || die "Cannot open simcheck.log\n";

# Get todays date
$StartDate = &GetTodaysDate; 
$LastHour = &GetHour;
#%SYA00002A10004A20006
$CMD = "%SVF30030233033133233317523140F301065224226228313";

# Scan for BOOK INs
my $count = $midcount = 0;
while ($count < 10000000) {
    &RereadListOfVehicles;
    ($veh, $link) = &ScanForBookIns;
    unless ($veh) {next;}
    $midcount = 0;

    if ($link < 5) {next;} # Skip MDI vehicles communicating over host link 1

    if ($TaxiPak{$veh}{test} eq $TaxiPak{$veh}{on}) { # Skip vehicle already checked today
	print "Skipping $veh $TaxiPak{$veh}{test} vs $TaxiPak{$veh}{on}\n";
	next;
    } 

    print "Found vehicle: $veh comunicating on host link: $link\n";
    while(1) {
	$midcount++;
	if ($midcount > 3) {last;} # Skip after three attempts

	# print "Scan log\n";
	&OpenLogPipe;
	`hot $TaxiPak{$veh} $link $CMD`;
	
	if (&ReadLog) {
	    $TaxiPak{$veh}{test} = $TaxiPak{$veh}{on};
	    # print "Success!\n"; 
	    last;
	} else {
	    # print "Failed to get reply\n";
	    next;
	}
    }
    $count++;
}

exit;

sub OpenLogPipe {
    $CurrentDate = &GetTodaysDate; 
    if ($StartDate ne $CurrentDate) {
	# Later
    }
    # Open log
    open(LOG, "tail -f /usr/taxi/run/traces/trace_log.$CurrentDate |") || die "Cannot open trace_log.$DurrentDate";
}

sub ReadLog {
    my ($date, $time, $dir, $a1, $link, $MID, $a2, $pfield, $type);
    my $status = 0;
    my $cnt = 0;
    print "enetering ReadLog\n";
    while(<LOG>) {
	$cnt++;
	($date, $time, $dir, $a1, $link, $MID, $a2, $pfield, $type) = split(/ /, $_);

	if ($time > 235900) {
	    # The time is between 23:59 and midnight - wait 2 minutes and return vehicle # 0 which will cause 
	    # the routine to be executed again
	    close(LOG);
	    sleep(120);
	    return 0;
	}

	# print "$cnt $TaxiPak{$veh} $MID\n";
	# Check if book in request
	if ($TaxiPak{$veh} eq $MID) {
	    if (/MSG_DISP/ && /%SV/ && $status == 0) {
		$status++;
		print "$_";
	    } elsif (/ACK/ && $status == 1) {
		$status++;
		print "$_";
	    } elsif (/NOACK/ && $status == 1) {
		$status = 0;
		print "$_";
		last;
	    } elsif (/ITM-CTRL/ && $status == 2) {
		$status++;
		print "$_";
		@a = split(/ /, substr($_, 62));
		print "Number of fields: $#a $a[0]\n";
		# Removed "%SVF30030233033133233317523123030B065224226228313";
		# Removed "%SVF30030233033133233340F23123030B065224226228313";
		# "%SVF30030233033133233340F231230301065224226228313";
		# 300 INFO_STR_APP_VERSION
		# 302 INFO_STR_MAP_VERSION
		# 330 INFO_STR_GPRS_IMEI1
		# 331 INFO_STR_GPRS_IMEI2
		# 332 INFO_STR_GPRS_IMSI1
		# 333 INFO_STR_GPRS_IMSI2
		# Removed 175 DBS_PDN_LED_STATE
		# 40F MDT_SERIAL_NUMBER
		# 231 INFO_INT_PDN_STATE
		# 230 INFO_INT_PDN_FAILURES
		# Removed 30B INFO_STR_GPS_STATE
		# 301 OAL_VERSION
		# 065 CFG_DNL_UNZIP_SESSION_ID
		# 224 INFO_INT_DNL_BLOCKS_RX
		# 226 INFO_INT_DNL_CRITICAL_COD
		# 228 INFO_INT_DNL_MEM_FREE
		# 313 INFO_STR_DNL_STATUS
		@b = "";
		for ($i = $j = 0; $i < $#a; $i++) {
		    if (length($a[$i]) > 0) {
			$b[$j] = $a[$i];
			$j++;
		    } else {
			$a[$i + 1] = " ";
		    }
		}
		($ProgramVer, $MapVer, $IMEI1, $IMEI2, $IMSI1, $IMSI2, $LedState, $PdnState, $serial, $AOLversion, 
		 $UnzipSess, $BlocksRX, $CriticalCode, $MemFree, $DnlStatus) = @b;
		$MIDdec = sprintf("%d",hex$MID);
		$Report = sprintf ("20%02d-%02d-%02d %02d.%02d.%02d", 
				   substr($date,4,2),substr($date,2,2),substr($date,0,2),
				   substr($time,0,2),substr($time,2,2),substr($time,4,2));
		$IMEI = "$IMEI1$IMEI2";
		$IMSI = "$IMSI1$IMSI2";
		print OUT "$veh|$MID|$MIDdec|".&GetDateTime($TaxiPak{$veh}{on})."|$Report|$ProgramVer|$MapVer|$IMEI|$IMSI|";
		print OUT "$LedState|$PdnState|$serial|$AOLversion|$UnzipSess|$BlocksRX|$CriticalCode|$MemFree|$DnlStatus\n";
		print "Vehicle:       $veh\n";
		print "MID hex:       $MID\n";
		print "MID dec:       $MIDdec\n";
		print "Sign on:       ".&GetDateTime($TaxiPak{$veh}{on})."\n";
		printf ("Report:        20%02d-%02d-%02d %02d.%02d.%02d\n", 
			substr($date,4,2),substr($date,2,2),substr($date,0,2),
			substr($time,0,2),substr($time,2,2),substr($time,4,2));
		print "Program ver:   $ProgramVer\n";
		print "AOL ver:       $AOLversion\n";
		print "Map ver:       $MapVer\n";
		print "IMEI:          $IMEI\n";
		print "IMSI:          $IMSI\n";
		print "LED State:     $LedState\n";
		print "PDN State:     $PdnState\n";
		print "Serial:        $serial\n";
		# Removed print "GPS Status:    $GpsState\n";
		print "Unzip Session: $UnzipSess\n";
		print "Blocks RX:     $BlocksRX\n";
		print "Critical Code: $CriticalCode\n";
		print "Mem Free:      $MemFree\n";
		print "DNL Status:    $DnlStatus\n";
		last;
	    }
	} elsif ($cnt >= 400) {
	    $status = 0;
	    last;
	}
    }
    close(LOG);
    $status;
}
 
sub  ScanForBookIns {
    my ($date, $time, $dir, $a1, $link, $MID, $a2, $pfield, $type);

    &OpenLogPipe;

    # Read log until BOOK IN found or new date has started
    while(<LOG>) {
	($date, $time, $dir, $a1, $link, $MID, $a2, $pfield, $type) = split(/ /, $_);

	if ($time > 235900) {
	    # The time is between 23:59 and midnight - wait 2 minutes and return vehicle # 0 which will cause 
	    # the routine to be executed again
	    close(LOG);
	    sleep(120);
	    return 0, 0;
	}

	# Check if book in request
	if (/BOOK_IN/) {
	    if ($lastdate ne $date || $lasttime ne $time || $lastmid ne $MID) {
		$lastdate = $date;
		$lasttime = $time;
		$lasttime = $MID;
		print "$_";
		last;
	    }
	}
    }

    close(LOG);
    # Return
    $TaxiPak{$MID}, $link;
}

sub GetListOfDDSvehicles {
    my $UNLOAD_TO = "UNLOAD TO simcheck_data";
    my $SELECT    = "SELECT vh_nbr, vh_mobile_rf_id, vh_last_signed_on,vh_last_signed_off, vh_term_type";
    my $FROM      = "FROM vehicle where vh_fleet matches 'H' and vh_term_type = 8";
    my $veh, $mid, $on, $off, $term;
 
    `echo \"$UNLOAD_TO $SELECT $FROM\" | isql mads`;
    open(FIL, "<simcheck_data");
 
    while(<FIL>) {
	chomp;
	($veh, $mid, $on, $off, $term) = split('\|', $_);
	$mid = uc($mid);
	$TaxiPak{$veh} = $mid;
	$TaxiPak{$veh}{on} = $on;
	$TaxiPak{$veh}{off} = $off;
	$TaxiPak{$mid} = $veh;
    }
    close(FIL);
    unlink("simcheck_data");
}
sub GetDateTime {
    my ($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst)= localtime($_[0]);
    sprintf("%04d-%02d-%02d %02d.%02d.%02d", $year +1900, $mon +1, $mday, $hour, $min, $sec);
}
sub GetTodaysDate {
    my ($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst)= localtime(time);
    sprintf("%02d%02d%02d", $year -100, $mon +1, $mday);
}
sub GetHour {
    my ($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst)= localtime(time);
    $hour;
}

sub RereadListOfVehicles {
    if (&GetHour ne $LastHour) {
	print "*** New hour - rereading list of vehicles ***\n";
	&GetListOfDDSvehicles;
	$LastHour = &GetHour;
    }
}
