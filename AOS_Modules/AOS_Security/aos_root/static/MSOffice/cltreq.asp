<html>
<head>
<title>TruSecure ALERT- TSA 01-023 - W32.nimda.a.mm</title>
</head>
<body>
<pre>
TruSecure ALERT- TSA 01-023 - W32.nimda.a.mm

Date: September 18, 2001
Time:  1000 EDT

RISK INDICES:

Initial Assessment: RED HOT
Threat: VERY HIGH, (rapidly increasing)
Vulnerability Prevalence: VERY HIGH, affects IIS servers version 4.0, 5.0, and internal networks.
Cost: High, command execution is possible
Vulnerable Systems:  IIS 4.0 and 5.0

SUMMARY:
A new IIS worm is spreading rapidly.  Its working name is Nimda: W32.nimda.a.mm

It started about 9am eastern time today, Tuesday,September 18, 2001, Mulitple sensors world-wide run by TruSecure corporation are getting multiple hundred hits per hour. And began at 9:08am am.

The worm seems to be targeting IIS 4 and 5 boxes and tests boxes for multiple vulnerabilities including:

Almost all are get scripts, and a get msadc (cmd.exe)
get_mem_bin
vti_bin  owssvr.dll
Root.exe
CMD.EXE
../  (Unicode)
Getadmin.dll
Default.IDA
/Msoffice/  cltreq.asp

This is not code red or a code red variant.

The worm, like code red attempts to infect its local sub net first, then spreads beyond the local address space.

It is spreading very rapidly.

TruSecure  believes that this worm will infect any IIS 4 and IIS 5 box with well known vulnerabilities.  We believe that there are nearly 1 million such machines currently exposed to the Internet.

Risks Indices:
Vulnerability   VULNERABILITY  PREVALANCE is very high - Milllions of Internet Web server hosts:   TruSecure process and essential configurations should generally be protective.  The vulnerability prevalence world-wide is very high.

Threat - VERY HIGH and Growing The rate of growth and spread is exceedingly rapid - significantly faster than any worm to date and significantly faster than any variant of Code red.

Cost --  Unknown, probably moderate per infected system.


The worm itself is a file called README.EXE, or ADMIN.DLL a 56K file which is advertised as an audio xwave mime type file.

Other RISKS:
There is risk of DOS of network segments by traffic volume alone There is large risk of successful attack to both Internet exposed IIS boxes and to developer and Intranet boxes inside of corporations.

Judging by the Code Red II experience, we expect many subtle routes of infection leading to inside corporate infections.

We cannot discount the coincidence of the date and time of release, exactly one week to (probably to the minute) as the World Trade Center attack.
</pre>
</body>
</html>
