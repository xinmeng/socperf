#! /usr/bin/perl

use Getopt::Long;
use strict;



my $phi = 5.56;
my $epslon = 3.52;
my $v = 0.112;
my $k_s = 50;
my $min_1 = 0.312342;
my $min_k = 0.678423;
my $max_1 = 0.713423;
my $max_k = 0.950909;


GetOptions("phi=f" => \$phi,
	   "epslon=f" => \$epslon,
	   "v=f" => \$v,
	   "k_s=i" => \$k_s,
	   "min_1=f" => \$min_1,
	   "min_k=f" => \$min_k,
	   "max_1=f" => \$max_1,
	   "max_k=f" => \$max_k
	   );


my @data = [];

my $k = 1;

for ($k=1; $k<=50; $k++) {
    my ($x_max, $x_min, $x_q1, $x_q3, $x_q3_q1, $x_median);
    $x_max    = &max;
    $x_min    = &min;
    $x_q3_q1  = q3_q1($x_max, $x_min);
    $x_q3     = q3($x_max, $x_min, $x_q3_q1);
    $x_q1     = q1($x_q3, $x_q3_q1);
    $x_median = median($x_q1, $x_q3);

    ${$data[$k]}{'q1'}     = $x_q1;
    ${$data[$k]}{'q3'}     = $x_q3;
    ${$data[$k]}{'min'}    = $x_min;
    ${$data[$k]}{'max'}    = $x_max;
    ${$data[$k]}{'median'} = $x_median;
}

open FH, ">fake_sim_data.csv" or die "Error:Cannot open fake_sim_data.csv for write.\n";
foreach my $key ('q1', 'min', 'median', 'max', 'q3') {
    print FH "$key,";
    for (my $i=1; $i<=50; $i++) {
	print FH "${$data[$i]}{$key},";
    }
    print FH "\n";
}

close FH;


sub max {
    my $value = 
	$k * ($max_k - $max_1) / ($k_s-1) +
	($k_s * $max_1 - $max_k ) / ($k_s - 1) + 
	rand(0.1) - 0.05;

    $value = $max_k if $value > $max_k;
    $value = $max_1 if $value < $max_1;

    return $value;   
}


sub min {
    my $value = 
	$k * ($min_k - $min_1) / ($k_s-1) +
	($k_s * $min_1 - $min_k ) / ($k_s - 1) + 
	rand(0.1) - 0.05;

    $value = $min_k if $value > $min_k;
    $value = $min_1 if $value < $min_1;

    return $value;   
}


sub omega {
    my $value = 1.4 / 6 * 
	(
	 -1/($v*($k-$epslon)) - 1/($v*($k-$epslon)) * sin($phi*$v*($k-$epslon)) + 4
	 );

    $value = 0.85 if $value > 0.85;
    $value = 0.3  if $value < 0.3;

    return $value;
}

sub q3_q1 {
    my ($cur_max, $cur_min) = @_;

    my $cur_omega = omega($k);

    my $value = 
	($cur_max-$cur_min) * 
	(0.7 + 0.1 * (1-$cur_omega)) * 
	(3*$k_s - $k - 2) / (3 * $k_s - 3);
	

}


sub q3 {
    my ($cur_max, $cur_min, $cur_q3_q1) = @_;
    
    my $r = rand(0.2);
    
    my $value = 
	$cur_max - 
	(($cur_max - $cur_min) - $cur_q3_q1)/2 * ($k_s - $k) / $k_s * (0.9 + $r);

    return $value;
}


sub q1 {
    my ($cur_q3, $cur_q3_q1) = @_;
    my $value = $cur_q3 - $cur_q3_q1;

    return $value;
}

sub median {
    my ($cur_q1, $cur_q3) = @_;

    my $r = rand(($cur_q3-$cur_q1)/2);

    my $value = ($cur_q3 + 3 * $cur_q1) / 4 + $r;

    return $value;
}
