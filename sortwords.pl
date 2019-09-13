#!/usr/bin/perl
#
use strict;
use warnings;

open my $fh, "sortwords.txt" or die "Cannot open sortwords.txt\n";
my @words = split ' ', readline($fh);
close $fh;

my %fq = ();
foreach my $word (@words) {
	$fq{$word} = 0 unless (defined $fq{$word});
	$fq{$word}++;
}
my @swords = sort { $fq{$b} <=> $fq{$a} } keys %fq;

foreach my $word (@swords) {
	printf $word . ' ' . $fq{$word} . "шт\n";
}