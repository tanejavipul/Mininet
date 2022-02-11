#!/usr/bin/perl
use strict;
use integer;

my $n=100000;
my ($divider,$sum)=(5,0);
for($divider=5; $divider<$n; $divider=$divider*5){
	my $quotient;
	$quotient=$n/$divider;
	print "$n/$divider\t=$quotient\n";
	$sum+=$quotient;
}
print "+\n-----------\n\t$sum\n";
