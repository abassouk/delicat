delicat
=======

CLI tool that allows concatenation and de-concatenation of streams.

Rationale
---------

* tar cannot handle streams (e.g. cannot have foo | tar -c)
* cat cannot delimit streams

I wanted to distribute the results of sfdisk and multiple ntfsclone invocations to multiple destinations using udpcast,
but there was no tool that could multiplex or concatenate their results without using local storage first. So I wrote
a small tool that could assist with that.

Usage
-----

The command
   (<cmd1> | delicat; <cmd2> | delicat) | ssh user@host "delicat -r | <cmd3>; delicat -r | <cmd4>"

Is equicalent to the following:
   <cmd1> | ssh user@host <cmd3>; <cmd2> | ssh user@host <cmd4>

with the difference that only 1 ssh connection will be made. 

Options
-------

If you have lots of small files passing through delicat, you may want to use the -n (--no-pad) option, which
will cause a slightly smaller output size due to disabling padding (which is 4k by default), but which will cause
higher CPU usage. You will need to pass this option to both sides of the delicat invocation.

License
-------

See the file COPYING - the source code is released under the GPLv2.
