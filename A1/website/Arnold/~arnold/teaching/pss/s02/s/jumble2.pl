#!/usr/bin/perl
use strict;
my ($w, %wl);
open (F, "<words");
while($w=<F>){
	chomp $w;
	push @{$wl{join("", sort(split //,lc $w))}}, $w;
}
close F;

my ($key, $max)=("",0);
foreach $key (keys %wl){
	$max = scalar @{$wl{$key}} if(scalar @{$wl{$key}}>$max);
}
foreach $key (keys %wl){
	print "$key  @{$wl{$key}} \n" if($max == scalar @{$wl{$key}});
}
