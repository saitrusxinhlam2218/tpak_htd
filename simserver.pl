#!/usr/bin/perl -w
#
# @(#) simserver.pl
#
# Author:    Hans Ottosson
# Date:      2004
# Copyright: (c) Hans Ottosson IT-Konsult i Kolarängen, 2004
######################################################################################################
use IO::Socket;
use Net::hostent;              # for OO version of gethostbyaddr 
my  ($PORT)          = 9000;                  
my  ($StringSetting) = 1;
my  ($veh, $action, $parameters, $Parameters);
my  ($MID, $MIDdec, $on, $off, $term_type, $CMD);
$veh = $action = $parameters = "";

################################
# Starting Server
################################

print "Starting: Simserver.pl\n" ;
#&PhoneNumber;
&SetUpSocket;

print "[Server $0 accepting clients]\n"; 
while ($client = $server->accept()) {
    $client->autoflush(1);

    print $client "Welcome to $0; type help for command list.\n";
    printf "[Connect from %s]\n", $client->peerhost;

    while ( <$client>) {

	next unless /\S/;       # blank line

	if        (/quit|exit/i)            { close $client;                           print "Connection closed\n"; last; }
	else {

	    ($veh, $action, $parameters) = split(/\|/, $_);      # Extract vehicle #, action and parameters 
	    if (length($parameters)) { chomp $parameters; }      # Remove NL from parameters

	    if    (/VehicleStatus/i)        { printf $client &VehicleStatus($veh);     print $client "quit\n";            }
	    elsif (/VehicleLastStatus/i)    { printf $client &VehicleLastStatus($veh); print $client "quit\n";            }
	    elsif (/GPRSmodem/i)            { printf $client &GPRSmodem($veh);         print $client "quit\n";            }
	    elsif (/DownloadStatus/i)       { printf $client &DownloadStatus($veh);    print $client "quit\n";            }
	    elsif (/GPSparameters/i)        { printf $client &GPSparameters($veh);     print $client "quit\n";            }
	    elsif (/RadioChannels/i)        { printf $client &RadioChannels($veh);     print $client "quit\n";            }
	    elsif (/DownloadChannels/i)     { printf $client &DownloadChannels($veh, $parameters);  
					      print  $client "quit\n";                                                    }
	    elsif (/VehicleTrace/i)         {                &VehicleTrace($veh);      print $client "quit\n";            }
	    elsif (/GPRStest/i)             { printf $client &GPRStest($veh);          print $client "quit\n";            }
	    elsif (/ALLparams/i)            {               &Allparams($veh);          print $client "quit\n";            }
	    elsif (/Counters/i)             { printf $client &Counters($veh);          print $client "quit\n";            }
	    elsif (/Command/i)              { printf $client &Command($veh, $parameters); 
					      print $client "quit\n";                                                     }
	    else {
		print $client "Unknown query type!\n";
		print "Unknown query type: $action!\n";
		print $client "quit\n"; 
	    }
	}
    }
    
    print "Closing: client\n";
    close $client;
}

###########################################
# Set up socket
###########################################
sub SetUpSocket {
    print "Entering: SetUpSocket\n"; 
    $server = IO::Socket::INET->new( Proto     => 'tcp',
				     LocalPort => $PORT,
				     Listen    => SOMAXCONN,
				     Reuse     => 1); die "can't setup server" unless $server;
    print "Exiting:  SetUpSocket\n"; 
}

sub InitVehicle {
    print "Entering: InitVehicle\n"; 
    $MID = $MIDdec = $on = $off = $term_type = "";
    print "Exiting:  InitVehicle\n"; 
}
    
sub VehicleStatus {
    my ($veh) = $_[0];
    my $Reply;
    $StringSetting = 1;

    print "Entering: VehicleStatus\n";
    &InitVehicle;
    unless (&FindVehicleInTaxiPak($veh)) { return "Vehicle $veh does not exist!\n"; }
    $Reply = &Perform;
    print "Exiting:  VehicleStatus - Reply: $Reply\n";
    return $Reply;
}

sub GPRSmodem {
    my ($veh) = $_[0];
    my $Reply;
    $StringSetting = 2;

    print "Entering: GPRSmodem\n";
    &InitVehicle;
    unless (&FindVehicleInTaxiPak($veh)) { 
	return "Vehicle $veh does not exist in TaxiPak!\n"; 
    }
    $Reply = &Perform;
    print "Exiting:  GPRSmodem - Reply: $Reply\n";
    return $Reply;
}

sub RadioChannels {
    my ($veh) = $_[0];
    my $Reply;
    $StringSetting = 7;

    print "Entering: RadioChannels\n";
    &InitVehicle;
    unless (&FindVehicleInTaxiPak($veh)) { 
	return "Vehicle $veh does not exist in TaxiPak!\n"; 
    }
    $Reply = &Perform;
    print "Exiting:  RadioChannels - Reply: $Reply\n";
    return $Reply;
}

sub Counters {
    my ($veh) = $_[0];
    my $Reply;
    $StringSetting = 10;

    print "Entering: Counters\n";
    &InitVehicle;
    unless (&FindVehicleInTaxiPak($veh)) { 
	return "Vehicle $veh does not exist in TaxiPak!\n"; 
    }
    $Reply = &Perform;
    print "Exiting:  Counters - Reply: $Reply\n";
    return $Reply;
}

sub Perform {
    my ($Reply, $now); 

    print "Entering: Perform\n";

    if (&System($term_type) eq "DDS vehicle") {
	print "Perform:   Found DDS vehicle\n";

	if ($on > $off)     { # Vehicle is signed on
	                      print "Perform:   Vehicle is signed on\n";
			      &ReadLog;
			      &SendCMD;
			      $Reply = &CheckLog;
			  } 

	elsif ($off >= $on) { # Vehicle is signed off
	                      print "Perform:   Vehicle is signed off\n";
			      $now = &now;
			      $Reply = "OK\|2\|$veh\|$MID\|$MIDdec\|$on\|$off\|Signed off\|$now\|$term_type\n"; 
			  } 
	else {                # Vehicle has never been signed off
	                      print "Perform:   Vehicle has never been signed on\n";
			      $now = &now;
			      $Reply = "OK\|2\|$veh\|$MID\|$MIDdec\|$on\|$off\|Never signed on\|$now\|$term_type\n"; 
			  }
    } else {
	print "Perform:   Found MDI vehicle\n";
	$now = &now;
	if ($on > $off) {
	    print "Perform:   Vehicle is signed on\n";
	    $Reply = "OK\|2\|$veh\|$MID\|$MIDdec\|$on\|$off\|Signed On\|$now\|$term_type\n"; 
	} elsif ($off >= $on) {
	    print "Perform:   Vehicle is signed off\n";
	    $Reply = "OK\|2\|$veh\|$MID\|$MIDdec\|$on\|$off\|Signed Off\|$now\|$term_type\n"; 
	} else {
	    print "Perform:   Vehicle has never been signed on\n";
	    $Reply = "OK\|2\|$veh\|$MID\|$MIDdec\|$on\|$off\|Never signed on\|$now\|$term_type\n"; 
	}
    }
    print "Exiting:  Perform - Reply: $Reply\n";
    return $Reply;
}

sub  now {
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
    return sprintf("%04d-%02d-%02d %02d.%02d.%02d", $year +1900, $mon +1, $mday, $hour, $min, $sec);
}

sub CheckLog {
    my ($Stime, $time, $now, $Reply);
    my ($count, $MaxCount, $vh_tele);

    print "Entering: CheckLog - MID: $MID\n";

    $Stime = 0;
    $count = 0;
    $MaxCount = 400;

    while(<LOG>) {
	$count++;
	$time = time;
	if  ($count >= $MaxCount || ($Stime > 0 && $Stime +18 < $time)) {
	    $Reply = "Time out - no answer within 18 seconds\n";
	    print "Count $count\n";
	    last;
	} elsif ($_ =~ / $MID / ) {
	    print "CheckLog: Found record from MID: $MID\n$_";
	    if ($_ =~ / %SV/) {
		print "CheckLog: Found %SV record from MID: $MID\n";
		print "$_\n";
		$Stime = time;
	    } elsif ($StringSetting eq 11) {
		print "CheckLog: Found Free text record to MID: $MID\n";
		if (($_ =~ / ACK / || $_ =~ / NOACK /) && $parameters =~ "%SV") {
		    print "CheckLog: Found ACK/NOACK from Free text record to MID: $MID\n";
		} else {
		    print "CheckLog: Found Free text record to MID: $MID\n";
		    return "Reply: $_\n";
		}
	    } elsif ($_ =~ /%SY24000/ && $StringSetting eq 9) {
		print "CheckLog: Found set Data Channels record to MID: $MID\n";
		return "Preferred Data Channel Setting downloaded to vehicle $veh\n";
	    } elsif ($_ =~ / %SZ780001/ ) {
		print "CheckLog: Found trigger GPRS test record to MID: $MID\n";
		return "GPRS test triggered\n";
	    } elsif ($_ =~ / ITM-CTRL_REGS /) {
		print "CheckLog: Found reply from MID: $MID\n";
		print "$_\n";
		if ($StringSetting eq 1) {
		    ($Ldate, $Ltime, $Ldir, $La1, $Llink, $LMID, $La2, $Lpfield, $Ltype, $LtxmType) = split(/ /, $_);
		    
		    ($Lversion, $LMapVersion, $LMapVersion2, $LModemDLL, $Limei1, $Limei2, $Limsi1, 
		     $Limsi2, $LGPRSled, $LPDN, $LPDNfailure, $LGPSstatus, $LGPSlat, $LGPSlong, $LVehNbr, $LSerial, $Rest) = 
			 split(/ /, substr($_, 62));
		    
		    if (length($Limei1) eq 0) {
			# IMEI is missing
			($Lversion, $LMapVersion, $LMapVersion2, $LModemDLL, $LModemDLL2, $LModemDLL3, $Limei1, $Limei2, $Limsi1, 
			 $Limsi2, $LGPRSled, $LPDN, $LPDNfailure, $LGPSstatus, $LGPSlat, $LGPSlong, $LVehNbr, $LSerial, $Rest) = 
			     split(/ /, substr($_, 62));
			if (length($Limsi1) eq 0) {
			    # IMEI is missing
			    ($Lversion, $LMapVersion, $LMapVersion2, $LModemDLL, $LModemDLL2, $LModemDLL3, $LModemDLL4, 
			     $LModemDLL5, $Limei1, $Limei2, $Limsi1, $Limsi2, $LGPRSled, $LPDN, $LPDNfailure, 
			     $LGPSstatus, $LGPSlat, $LGPSlong, $LVehNbr, $LSerial, $Rest) = split(/ /, substr($_, 62));
			}
		    } elsif (length($Limsi1) eq 0) {
			($Lversion, $LMapVersion, $LMapVersion2, $LModemDLL, $LModemDLL2, 
			 $LModemDLL5, $Limei1, $Limei2, $Limsi1, $Limsi2, $LGPRSled, $LPDN, $LPDNfailure, 
			 $LGPSstatus, $LGPSlat, $LGPSlong, $LVehNbr, $LSerial, $Rest) = split(/ /, substr($_, 62));
		    }
		    $Limei = "$Limei1$Limei2";
		    $Limsi = "$Limsi1$Limsi2";
		    if (length($Limei1) < 10 ) {
			$Limei = "Missing";
		    } 
		    if (length($Limsi1) < 10) {
			$Limsi = "Missing";
		    }
		    
		    if ($LGPRSled eq 0) { 
			$LGPRSled = "PDN module not connected";
		    } elsif ($LGPRSled eq "1") { 
			$LGPRSled = "Red - Disconnected"; 
		    } elsif ($LGPRSled eq 2) { 
			$LGPRSled = "Yellow - Modem connected"; 
		    } elsif ($LGPRSled eq 3) { 
			$LGPRSled = "Green - Data connecte"; 
		    } else { 
			$LGPRSled = "Undefined"; 
		    }
		    if ($LPDN eq 3) { 
			$LPDN = "Modem connected";
		    } elsif ($LPDN eq 4) {
			$LPDN = "Voice call pending";
		    } elsif ($LPDN eq 5) {
			$LPDN = "Answering voice call";
		    } elsif ($LPDN eq 6) {
			$LPDN = "Voice call active";
		    } elsif ($LPDN eq 7) {
			$LPDN = "Hang up voice call";
		    } elsif ($LPDN eq 2) {
			$LPDN = "Dialing modem";
		    } elsif ($LPDN eq 1) {
			$LPDN = "Initializing modem";
		    } elsif ($LPDN eq 0) {
			$LPDN = "Modem disconnected";
		    }
		    print "CheckLog: answering\n";
		    $now = &now;
		    $vh_tele = &PhoneNumber($Limsi);
		    print "IMSI: $Limsi Phone: $vh_tele\n";
		    $Reply = sprintf("OK\|1\|$veh\|$MID\|$MIDdec\|$on\|$off\|Signed on\|$now\|$term_type\|$Lversion\|$LMapVersion\|$LModemDLL\|$Limei\|$Limsi\|$LGPRSled\|$LPDN\|$LPDNfailure\|$LGPSstatus\|$LGPSlat\|$LGPSlong\|$LVehNbr\|$LSerial\|$vh_tele\n");
		    last;
		} elsif ($StringSetting eq 2) {
		    ($Ldate, $Ltime, $Ldir, $La1, $Llink, $LMID, $La2, $Lpfield, $Ltype, $LtxmType) = split(/ /, $_);
		    ($LPDNport, $Lip, $Lport, $Ldial, $Linit, $Lcband, $Lcgdcont, $Lcgqmin, $Lcgqreq, $Lcsq, $Lcrtt, $Lcgsn, 
		     $Lcimi, $Ls96, $Lcnum, $Rest) = split(/ /, substr($_, 62));
		    
		    $now = &now;
		    $Reply = sprintf("OK\|1\|$veh\|$MID\|$MIDdec\|$on\|$off\|Signed on\|$now\|$term_type\|$LPDNport\|$Lip\|$Lport\|$Ldial\|$Linit\|$Lcband\|$Lcgdcont\|$Lcgqmin\|$Lcgqreq\|$Lcsq\|$Lcrtt\|$Lcgsn\|$Lcimi\|$Ls96\|$Lcnum\n");
		    last;
		} elsif ($StringSetting eq 3) {
		    ($Ldate, $Ltime, $Ldir, $La1, $Llink, $LMID, $La2, $Lpfield, $Ltype, $LtxmType) = split(/ /, $_);
		    ($Lsession, $LfileLen, $LblockSize, $LnumBlocks, $LblocksRX, $Lcriticals, $Lcode, $LtotMem, $LmemFree, $LtotFs,
		     $LfsFree, $Ldecomp, $LdnlStatus, $LmapVer, @Rest) = split(/ /, substr($_, 62));
		    if (length($LdnlStatus) == 0) {
			# No download status - the terminal is brand new - fix the map version
			$LmapVer = $Rest[0];
		    }
		    printf("\n%-14s %-20d %-14s %-43s\n", 
			    "Vehicle:", $veh, "Sign On:", &Date($on));
		    printf("%-14s %-20s %-14s %-30s\n", 
			    "MID Hex:", uc($MID), "Sign Off:", ($off eq 0 ? "" : &Date($off)));
		    printf("%-14s %-20s %-14s %-43s\n", 
			    "MID dec:", hex($MID), "Report:", &DateFormat($Ldate, $Ltime));
		    print "\n";
		    printf("%-14s %-20s %-14s %-43s %-14s %-20s\n", 
			    "DNL Session:", $Lsession, "Num blocks:", $LnumBlocks, "Tot.memory:", $LtotMem);
		    printf("%-14s %-20s %-14s %-43s %-14s %-20s\n", 
			    "File lenght:", $LfileLen, "Blocks RX:", $LblocksRX, "Memory free:", $LmemFree);
		    printf("%-14s %-20s %-14s %-43s %-14s %-20s\n", 
			    "Block size:", $LblockSize, "Remaining:", $LnumBlocks-$LblocksRX, "Tot.filesys:", $LtotFs);
		    printf("%-14s %-20s %-14s %-43.2f %-14s %-20s\n", 
			    "", "", "Remaining %:", $LnumBlocks ? ($LnumBlocks-$LblocksRX)/$LnumBlocks*100 : 0, 
			    "Filesys used:", $LtotFs - $LfsFree);
		    printf("%-14s %-20s %-14s %-43s %-14s %-20s\n", 
			    "", "", "", "", "Filesys free:", $LfsFree);
		    printf("%-14s %-20s %-14s %-43s %-14s %-20.2f\n", 
			    "Criticals:", $Lcriticals, "Decomp.sess:", $Ldecomp, "FS free %:", $LtotFs ? $LfsFree/$LtotFs*100 : 0);
		    printf("%-14s %-20s %-14s %-43s %-14s %-20s\n", 
			    "Critical code:", $Lcode, "Map version:", $LmapVer, "DNL Status", $LdnlStatus);
		} elsif ($StringSetting eq 7) {
		    ($Ldate, $Ltime, $Ldir, $La1, $Llink, $LMID, $La2, $Lpfield, $Ltype, $LtxmType) = split(/ /, $_);
		    ($EmergCh, $Vch1, $Ch1, $Ch2, $Ch3, $Ch4, $Ch5, $Ch6, $Ch7, $Ch8, $Ch9, $Ch10, $Vch2, $Rest) = 
			split(/ /, substr($_, 62));
		    $now = &now;
		    $Reply = sprintf("OK\|1\|$veh\|$MID\|$MIDdec\|$on\|$off\|Signed on\|$now\|$term_type\|$EmergCh\|$Vch1\|$Ch1\|$Ch2\|$Ch3\|$Ch4\|$Ch5\|$Ch6\|$Ch7\|$Ch8\|$Ch9\|$Ch10\|$Vch2\n");
		    last;
		} elsif ($StringSetting eq 10) {
		    ($Ldate, $Ltime, $Ldir, $La1, $Llink, $LMID, $La2, $Lpfield, $Ltype, $LtxmType) = split(/ /, $_);
		    ($LprimaryChannel, 
		     $LBackupChannel, 
		     $LVoiceChannel, 
		     $LGPSfailures, 
		     $LGPSreset, 
		     $LPDNrxPkts, 
		     $LPDNtxPkts, 
		     $LPDNdataLoss, 
		     $LPDNfailures, 
		     $LPDNstate, 
		     $LCamStatus, 
		     $LCamError, 
		     $LWatchdogReset, 
		     $LFatalError,
		     $Rest) = split(/ /, substr($_, 62));
		    $now = &now;
		    $Reply = sprintf("OK\|1\|$veh\|$MID\|$MIDdec\|$on\|$off\|Signed on\|$now\|$term_type\|$LprimaryChannel\|$LBackupChannel\|$LVoiceChannel\|$LGPSfailures\|$LGPSreset\|$LPDNrxPkts\|$LPDNtxPkts\|$LPDNdataLoss\|$LPDNfailures\|$LPDNstate\|$LCamStatus\|$LCamError\|$LWatchdogReset\|$LFatalError\n");
		    last;
		}
	    } elsif ($_ =~ / NOACK /) {
		$Reply = "NOACK from vehicle\n";
		last;
	    } elsif  ($count >= $MaxCount) {
		print "Time $Stime > ".time." \n";
		print "Count $count\n";
		$Reply = "No answer in $MaxCount records in log file\n";
		last;
	    } 
	} 
    }
    print "\n";
    close(LOG);
    print "Entering: CheckLog - Reply: $Reply\n";
    return $Reply;
}

sub PhoneNumber {
    my ($imsi, $phone);

    $imsi = $_[0] ;

    print "Entering: PhoneNumber\n";
    $SQLphoneQuery = "SELECT vh_imsi, vh_tele FROM imsi WHERE vh_fleet MATCHES 'H' AND vh_imsi MATCHES '$imsi'";
    print "$SQLphoneQuery\n";
    open(PHONE, "echo \"$SQLphoneQuery\" | isql mads |");
    while(<PHONE>) {

	if ($_ =~ $imsi) {
	    ($imsi, $phone) = split(" ", $_); 
	    close (PHONE);
	    return $phone;
	}
    }
    close (PHONE);
    return "IMSI not in TaxiPak database";

}
		 
		 
sub SendCMD {
    my $stand;

    print "Entering: SendCMD\n";
 
    if ($StringSetting eq "11") {
	print "SendCMD:  Sending free text string!\n";
	$CMD = $Parameters;
	print "SendCMD: $Parameters\n";
	&Send;
    } elsif ($StringSetting eq "10") {
	# Request Counters
	#  21D = INFO_INT_PRIMARY_CHAN    
	#  21E = INFO_INT_BACKUP_CHAN     
	#  21F = INFO_INT_VOICE_CHAN      
	#  22C = INFO_INT_GPS_FAILURES    
	#  22B = INFO_INT_GPS_RESET
	#  22E = INFO_INT_PDN_RX_PACKETS  
	#  22D = INFO_INT_PDN_TX_PACKETS  
	#  22F = INFO_INT_PDN_DATA_LOSS   
	#  230 = INFO_INT_PDN_FAILURES    
	#  231 = INFO_INT_PDN_STATE      
	#  236 = INFO_INT_CAM_STATUS     
	#  237 = INFO_INT_CAM_ERROR      
	#  238 = INFO_INT_WATCHDOG_RESET                         
	#  30F = INFO_STR_FATAL_ERROR   
	print "SendCMD:  Requesting Counters\n";
	$CMD = "%SVE21D21E21F22C22B22E22D22F23023123623723830F";
	&Send;
    } elsif ($StringSetting eq "9") {
	print "Download of channels\n";
	$CMD = $Parameters;
	print "SendCMD: $Parameters\n";
	&Send;
    } elsif ($StringSetting eq "7") {
	# Request Roaming Table - Data channels
	# 024 = Data channel 1
	# 025 = Data channel 2
	# 026 = Data channel 3
	# 027 = Data channel 4
	# 028 = Data channel 5
	# 029 = Data channel 6
	# 02A = Data channel 7
	# 02B = Data channel 8
	# 02C = Data channel 9
	# 02D = Data channel 10
	# 034 = Voice channel 
	# 035 = Voice channel 
	# 03C = Emergency channel
	print "Requesting Roaming Table - Data channels - Config Parameters\n";
	print "EmergCh Vch1 Ch1 Ch2 Ch3 Ch4 Ch5 Ch6 Ch7 Ch8 Ch9 Ch10 VCh2\n";
	$CMD = "%SVD03C03402402502602702802902A02B02C02D035";
	&Send;
    } elsif ($StringSetting eq "6") {
	# Request DBS GPS Parameters
	# 11C = GPS Active Status
	# 11D = GPS Report
	# 11E = GPS Report Factor
	# 11F = GPS Embed Factor
	# 120 = GPS Report Interval Vacant
	# 121 = GPS Report Interval Hired
	# 122 = GPS Report Interval Emerg 
	# 123 = GPS Report Distance Vacant
	# 124 = GPS Report Distance Hired
	# 125 = GPS Report Distance Emerg
	# 126 = GPS Confidence Interval
	# 127 = GPS GPS GMT Offset
	# 14F = GPS Cold Start Interleave
	# 16F = GPS Report GPS Failures
	print "Requesting DBS GPS Parameters\n";
	print "Active Report RptFactor IntVacant IntHired IntEmerg DistVacant DistHired DistEmerg ConfidInt GMToffset CldStrtIntrlv Failures\n";
	$CMD = "%SVE11C11D11E11F12012112212312412512612714F16F";
	&Send;
    } elsif ($StringSetting eq "5") {
	
	# Download Stands coordinates table
	# %SS clause - 1 stand per clause 
	print "Download Stands Table\n";
	
	$nbrStands = 10;
	my $endclause = ".";
	&DefineStands;
	
	# Clear all previous stands
	$CMD = "%.SR.";
	# &Send;
	#sleep 15;
	
	$cnt = 0;
	# BUILD COMMAND STRING
	
	foreach $stand (sort keys %zone) {
	    $cnt++;
	    if ($cnt > $nbrStands) {
		$cnt = 1;
		$CMD = &StartOfString;
		$CMD = sprintf("%s%s%1X", $CMD, $CMD1, $nbrStands);
		print "$CMD\n";
		$CMD1 = "";
		$CMD = $CMD.".\n";
		print "$CMD\n";
	    }
	    $CMD1 = $CMD1.sprintf("%03d+%s+%s%03d", 
				  $stand,
				  $lat{$stand},
				  $long{$stand},
				  $zone{$stand}
				  );
	}
	$CMD = &StartOfString;
	$CMD = sprintf("%s%1X", $CMD1, $nbrStands);
	$CMD1 = "";
	$CMD = $CMD.".\n";
	print "$CMD\n";
	print "$cnt\n";
	#&Send;
    } elsif ($StringSetting eq "4") {
	# Request GPRS Test
	# 178 = PDN Param Upload
	print "Requesting PDN Param Upload\n";
	$CMD = "%SZ780001";
	&Send;
    } elsif ($StringSetting eq "3") {
	# Request Download Status
	# 220 = Session
	# 221 = File len
	# 222 = Block size
	# 223 = Num blocks
	# 224 = Blocks RX
	# 225 = Criticals
	# 226 = Code
	# 227 = Tot mem
	# 228 = Mem free
	# 229 = Tot fs
	# 22A = Fs free
	# 065 = Decompress session at next power up
	# 302 = Map Version
	print "Requesting Download Status\n";
	print "Session File_len Block_size Num_blocks Blocks_RX Criticals Code Tot_mem Mem_free Tot_fs Fs_free DecompSess MapVer\n";
	$CMD = "%SVE22022122222322422522622722822922A065313302";
	&Send;
    } elsif ($StringSetting eq "2") {
	# Request IP/Port/AT-Strings
	# 400 = PDN port
	# 402 = IP
	# 401 = Port
	# 405 = Dial string
	# 406 = Init command
	# 407 = AT+CBAND=4
	# 408 = AT+CGDCONT
	# 409 = AT+CGQMIN
	# 40A = AT+CGQREQ
	# 40B = AT+CSQ
	# 413 = AT+CRTT
	# 414 = AT+CGSN
	# 415 = AT+CIMI
	# 416 = ATS96=1
	# 417 = AT+CNUM?
	print "Requesting PDN-port/IP/Port/AT-Strings\n";
	print "PDN_port IP Port Dial Init CBAND CGDCONT CGQMIN CGQREQ CSQ CRTT CGSN CIMI S96 CNUM\n";
	$CMD = "%SVF40040240140540640740840940A40B413414415416417";
	&Send;
    } else {
	# 300 = Program version
	# 302 = Map version
	# 309 = Modem version
	# 330 = IMEI1
	# 331 = IMEI2
	# 330 = IMSI1
	# 331 = IMSI2
	# 175 = LED State
	# 231 = PDN State
	# 230 = PDN Failures
	# 30B = GPS Status
	# 30C = GPS LAT
	# 30D = GPS LONG
	# 40F = MDT Serial nbr
	# 40C = Vehicle nbr
	print "Requesting Program version/Map version/Modem ver/IMEI/IMSI LEDstate PDNstate PDNfailures GPSstatus GPSlat GPSlong VehicleNbr Serial\n";
	print "Program Map IMEI1 IMEI2 IMSI1 IMSI2 Download\n";
	$CMD = "%SVF30030230933033133233317523123030B30C30D40C40F";
	&Send;
    }
}
    
sub Send {
    print "SendCMD to $MID 5 $CMD\n";
    print "SendCMD to $MID 6 $CMD\n";
    `hot $MID 5 $CMD`;
    `hot $MID 6 $CMD`;
}

sub ReadLog {
    # print "Opening read log\n";
    my $logfile = "/usr/taxi/run/traces/trace_log.".&ShortDate;
    open(LOG, "tail -f $logfile |") || die "Cannot open trace log\n";
}

sub ShortDate {
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
    sprintf("%02d%02d%02d", $year -100, $mon +1, $mday);
}

sub FindVehicleInTaxiPak {
    # Find the vehicle in TaxiPak
    print "Entering: FindVehicleInTaxiPak - Vehicle: $_[0]\n";
    open (FH, "echo \"select vh_mobile_rf_id, vh_last_signed_on, vh_last_signed_off, vh_term_type from vehicle where vh_nbr = $_[0]\"|/usr/informix/bin/isql mads 2>&1|") || die;
    print "Scan FH\n";
    while(<FH>) {
	chomp;
	if (substr($_, 0,2) eq "00") {
	    ($MID, $on, $off, $term_type) = split(" ", $_);
	    $MID = uc($MID);				
	    print "Found MID $MID\n";
	    $MIDdec = sprintf("%d", hex($MID));
	    return 1;
	}
    }
    close(FH);
    # Vehicle number $_[0] does not exist!
    0;
}

sub System {
    if ($_[0] eq "8") {
	"DDS vehicle";
    } else {
	"MDI vehicle";
    }
}

sub DownloadStatus {
    "DownloadStatus query is currently not available!\n";
}

sub GPSparameters {
    return "GPSparameters query is currently not available!\n";
}

sub VehicleTrace {
    my ($logfile, $count);

    print "Entering: VehicleTrace - Vehicle - $veh: $veh\n";
    unless (&FindVehicleInTaxiPak($veh)) { 
	print "Vehicle $veh does not exist in TaxiPak!\n"; 
	print $client "Vehicle $veh does not exist in TaxiPak!\n"; 
	return "Vehicle $veh does not exist!\n"; 
    }

    close(LOG);
    $logfile = "/usr/taxi/run/traces/trace_log.".&ShortDate;
    open(LOG, "egrep \" $MID \" $logfile |") || die "Cannot open trace log\n";

    # Check trace log
    $count = 0;
    while(<LOG>) {
	$count++;
	if ($_ =~ / $MID / ) {
	    print "$_";
	    print $client "$_";
	}
    }
    close(LOG);
    return;
}

sub GPRStest {
    my ($veh) = $_[0];
    my $Reply;
    
    $StringSetting = 4;

    print "Entering: GPRStest - StringSetting: $StringSetting\n";

    unless (&FindVehicleInTaxiPak($veh)) { 
	print "Vehicle $veh does not exist in TaxiPak!\n"; 
	return "Vehicle $veh does not exist in TaxiPak!\n"; 
    }
    $Reply = &Perform;
    print "$Reply\n";
    if (substr($Reply,0,4) ne "GPRS") {
	$Reply = "Not possible to trigger GPRS test in vehicle $veh\n";
    }
    return $Reply;
}

sub ALLparams {
    my ($veh) = $_[0];
    my $Reply;
    
    $StringSetting = 12;

    print "Entering: GPRStest - StringSetting: $StringSetting\n";

    unless (&FindVehicleInTaxiPak($veh)) { 
	print "Vehicle $veh does not exist in TaxiPak!\n"; 
	return "Vehicle $veh does not exist in TaxiPak!\n"; 
    }
    $Reply = &Perform;
    print "$Reply\n";
    if (substr($Reply,0,4) ne "GPRS") {
	$Reply = "Not possible to trigger GPRS test in vehicle $veh\n";
    }
    return $Reply;
}

sub DownloadChannels {
    my ($veh, $parameters) = @_;
    my $Reply;
    
    $StringSetting = 9;

    print "Entered Download Channels - Parameters: $parameters\n";

    # Check if vehicle exists in TaxiPak
    unless (&FindVehicleInTaxiPak($veh)) { 
	return "Vehicle $veh does not exist!\n"; 
    }

    ($Ch1, $Ch2, $Ch3, $Ch4, $Ch5, $Ch6) = split(/:/, $parameters);
    $Parameters = sprintf("%%SY24%04d%%SY25%04d%%SY26%04d%%SY27%04d%%SY28%04d%%SY29%04d", 
			  $Ch1, $Ch2, $Ch3, $Ch4, $Ch5, $Ch6) ;
    chomp $parameters;
    $Reply = "Preferred Radio Channels downloaded! $parameters\n";
    print "Download string: $Parameters\n";

    $Reply = &Perform;
    print "$Reply\n";

    if (substr($Reply,0,9) ne "Preferred") {
	if (substr($Reply,0,5) eq "OK\|2\|") {
	    $Reply = "Vehicle $veh is signed off!\n";
	} else {
	    $Reply = "Not possible to download Preferred Data Channel Setting in vehicle $veh\n";
	}

    }
    print "Returns Download Channels\n";
    return $Reply;
}

sub Command {
    my ($veh, $Command) = @_;
    
    $StringSetting = 11;

    print "Entering: Command - Vehicle: $veh Command: $Command\n";

    # Check if vehicle exists in TaxiPak
    unless (&FindVehicleInTaxiPak($veh)) { 
	return "Vehicle $veh does not exist in TaxiPak!\n"; 
    }

    $Parameters = $Command;

    print "Parameters: $Parameters\n";
    $Reply = sprintf("OK\|1\|$veh\|$MID\|$MIDdec\|$on\|$off\|Signed on\|$now\|$term_type\|%s", &Perform);
    chomp ($Reply);
    print "Exiting:  Command - Reply: $Reply\n";
    return $Reply;

}

sub VehicleLastStatus {
    open(LOG, "<./simcheck.log");
    my $reply = "";
    while(<LOG>) {
	if (/^$_[0]\|/) {
	    $reply = $_;
	    # print "$action $_"; 
	    # print $client "$_"; 
	}
    }
    close(LOG);
    if ($reply ne "") {
	"OK|$reply"; 
    } else {
	"No rec found\n"; 
    }
}

