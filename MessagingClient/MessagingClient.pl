#!/usr/bin/perl
#MessagingClient.pl
# Usage:
# "perl MessagingClient.pl -s localhost -p 8787 -n 10"
#	-s <Server IP>
#	-p <Server Port>
#	-n <Number of Requests to be send to Server>		


use IO::Socket::INET;
use POSIX qw/strftime/;

$numArgs = $#ARGV + 1;
$index = 0;
$serverIP = "";
$serverPortNumber = "";
$numberOfRequests = "";

while($index < $numArgs ) {
	$firstChar = substr $ARGV[$index],0,1;
	if($firstChar eq "-") {
		$secondChar = substr $ARGV[$index],1,1;
		if($secondChar eq "s")
		{
			if(length($ARGV[$index]) == 2) {
				$index = $index + 1;
				$serverIP = $ARGV[$index];
			}
			else
			{
				$serverIP = substr $ARGV[$index],2,length($ARGV[$index]) - 2;
			}
		}
		if($secondChar eq "p")
		{
			if(length($ARGV[$index]) == 2) {
				$index = $index + 1;
				$serverPortNumber = $ARGV[$index];
			}
			else
			{
				$serverPortNumber = substr $ARGV[$index],2,length($ARGV[$index]) - 2;
			}
		}
		if($secondChar eq "n")
		{
			if(length($ARGV[$index]) == 2) {
				$index = $index + 1;
				$numberOfRequests = $ARGV[$index];
			}
			else
			{
				$numberOfRequests = substr $ARGV[$index],2,length($ARGV[$index]) - 2;
			}
		}
	}
	$index = $index + 1;
}

$logFileName = 'MessagingClient.log';

open(my $fh, '>>', $logFileName) or die "Could not open log file '$logFileName' $!";

PrintToConsole( "Connecting to Server IP = $serverIP Port Number = $serverPortNumber Number of Requests = $numberOfRequests");
PrintToFile ("Connecting to Server IP = $serverIP Port Number = $serverPortNumber Number of Requests = $numberOfRequests");

if(length($serverIP) == 0 ){
	PrintToConsole( "Invalid Server IP. Exiting from Application");
	PrintToFile ("Invalid Server IP. Exiting from Application");
	exit;
}
if(length($serverPortNumber) == 0 ){
	PrintToConsole("Invalid Server Port Number. Exiting from Application");
	PrintToFile ("Invalid Server Port Number. Exiting from Application");
	exit;
}
if(length($numberOfRequests) == 0 ){
	PrintToConsole("Invalid Number of Requests. Exiting from Application");
	PrintToFile ("Invalid Number of Requests. Exiting from Application");
	exit;
}

my ($socket,$client_socket);
$socket = new IO::Socket::INET (
				PeerHost => $serverIP,
				PeerPort => $serverPortNumber,
				Proto => 'tcp'
				) or die "ERROR in Socket Creation : $!\n";

PrintToConsole("TCP Connection Success.");
PrintToFile ("TCP Connection Success.");

$index = 1;
$socketStatus = 0;
while ($index <= $numberOfRequests)
{
	$data = "GETMESSAGE";
	if($socket->send("$data\n") > 0) {
		PrintToConsole( "Request Packet Number: $index  to Server. Data: $data");
		PrintToFile ("Request Packet Number: $index  to Server. Data: $data");
	}
	else
	{
		PrintToConsole( "Failed to Send Packet Number: $index  to Server. Data: $data");
		PrintToFile ("Failed to Send Packet Number: $index  to Server. Data: $data");
		$socketStatus = 1;
		last;
	}
	$data = "";
	$socket->recv($data,1024);
	if(length($data) > 0) {
		PrintToConsole( "Response Packet Number:$index from Server. Data : $data");
		PrintToFile ("Response Packet Number:$index from Server. Data : $data");
	}
	else
	{
		PrintToConsole( "Failed In Receiving Packet Number:$index from Server.");
		PrintToFile ("Failed In Receiving Packet Number:$index from Server.");
		$socketStatus = 1;
		last;
	}
	$index = $index + 1;
	select(undef, undef, undef, 0.01);
}
if($socketStatus == 0)
{
	$data = "BYE";
	if($socket->send("$data\n")) {
		PrintToConsole("Request Packet to Server. Data:$data");
		PrintToFile ("Request Packet to Server. Data:$data");
	}
	else
	{
		PrintToConsole("Failed to Send Packet to Server. Data:$data");
		PrintToFile ("Failed to Send Packet to Server. Data:$data");
	}
}
$socket->close();
PrintToConsole("Exiting From Client Application");
PrintToFile ("Exiting From Client Application");
PrintToConsole("\n..............................................\n");
PrintToFile("\n..............................................\n");
close $fh;

sub PrintToConsole
{
	$datetime = strftime('%d-%b-%Y %H:%M:%S',localtime);
	print "\nPID=$$|$datetime|$_[0]";
}
sub PrintToFile
{
	$datetime = strftime('%d-%b-%Y %H:%M:%S',localtime);
	print $fh "\nPID=$$|$datetime|$_[0]";
}

