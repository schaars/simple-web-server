#!/usr/bin/perl -w
use strict;
use warnings;

my $core = 0;
sub tsk {
   my $cmd;
   my ($a,$b) = @_;
$cmd = "sudo taskset -cp $core $a";
   print "$cmd\n";
   print `$cmd`;
   $core = ($core + 1)%8;
}

my $process_name = "ws";

   my $res = `ps -A -L -o lwp= -o comm=`;
   my @_lines = split(/\n/, $res);
   for my $l (@_lines) {
      next if($l !~ m/(\d+) ([^\s]+)/);
      next if($2 ne $process_name);
      tsk($1,$2);
   }
