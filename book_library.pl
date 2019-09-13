#!/usr/bin/perl
#
use strict;
use warnings;

my %lib;
open my $fh, "book_library.txt" or die "Cannot open book_library.txt\n";
while ( my $line = readline $fh ) {
	my ( $book, $reader ) = split ' ', $line;

	$lib{$book} = {} unless ( defined $lib{$book} );
	$lib{$book}->{$reader} = 0 unless ( defined $lib{$book}->{$reader} );
	$lib{$book}->{$reader}++;
}
close $fh;

sub reads {
	my $bookref = shift;
	my $reads   = 0;
	foreach my $reader ( keys %{$bookref} ) {
		$reads += $bookref->{$reader};
	}
	return $reads;
}
printf "Количество аренд:\n";

my @bbooks = sort { reads( $lib{$b} ) <=> reads( $lib{$a} ) } keys %lib;

for ( my $i = 0; $i < 5; $i++ ) {
	printf $bbooks[$i] . ' ' . reads( $lib{ $bbooks[$i] } ) . "\n";
}

printf "\nУникальных читателей:\n";
my @breaders
	= sort { scalar( keys %{ $lib{$b} } ) <=> scalar( keys %{ $lib{$a} } ) }
	keys %lib;

for ( my $i = 0; $i < 5; $i++ ) {
	printf $breaders[$i] . ' '
		. scalar( keys %{ $lib{ $breaders[$i] } } ) . "\n";
}
