# Refactor

### Made by Nami Reghbati

### HOW TO USE

```NAME
refactor - refactor strings in files and folders

SYNOPSIS
refactor [OPTION]... [-i|-r|-d] [SOURCE] [FIND] [REPLACEMENT]
refactor [OPTION]... [-i]|[-r]|[-d] [SOURCE] [FIND] [REPLACEMENT]
refactor [OPTION]... [-h]

DESCRIPTION
Find and replace all matching substrings in 'SOURCE' where 'FIND' is the substring to search for
and 'REPLACEMENT' being the string to replace it with.

OPTIONS
Default usage without any options will result in a search of the substringin the file provided as
input.

	EXAMPLE (1):

		$ refactor test.txt original changed

		Translates to : Find and replace all occurrences of the word 'original' in the file 'test
		.txt' with the word 'changed'.

	EXAMPLE (2):

		$ refactor -ird /home/$USER/Test/ ERROR SUCCESS

		Translates to : Find and replace all occurrences of the word 'ERROR' with the word 'SUCCE
		SS' in every file that '/home/$USER/Test/' contains.

	-h, --help
		Show help for the command's usage and exit.

	-i, --case-insensitive
		Ignore case of characters in the given 'FIND' string when matching.

	-r, --recursive
		Search recursively all subfolders and replace all occurrences in files that match 'FIND'
		with 'REPLACEMENT'. Option '-d' MUST be declared as well!

	-d, --directory

		Parse all files in 'SOURCE' directory and replace all occurrences that match 'FIND' with
		'REPLACEMENT'.
		If the directory provided contains subdirectories, '-d' will IGNORE them UNLESS '-r' is
		declared as well.

RETURN VALUE
Upon successful return, the program will return 0.
If an error has occurred during the refactoring process, the program will return a unique ERROR
code alongside an error message passed in stderr.

	See the EXIT STATUS section to view all possible ERROR codes.

EXIT STATUS
0:	Program executed successfully.
1:	An internal error has occurred with the c libraries used.
2:	Invalid usage of command, command options are most likely not used properly.
3:	File or folder not found.
4:	File or folder empty.
5:	Source is neither a file or folder.
6:	Substring [FIND] did not match anything.

CONFORMING TO
C99, C11, C17, POSIX.1-2001, POSIX.1-2008

AUTHOR
Written by Nami Reghbati.

COPYRIGHT
Copyright © 2020 Free Software Foundation, Inc. License  GPLv3+:  GNU  GPL  version  3  or  later
<https://gnu.org/licenses/gpl.html>.
This is free software: you are free to change and redistribute it.  There is NO WARRANTY, to the
extent permitted by law.
```