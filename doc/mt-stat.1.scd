mt-stat(1)

# NAME
mt-stat - Inspect properties of a Merkle Tree

# SYNOPSIS
*mt-stat* [options] <path/to/mtree>

# DESCRIPTION
*mt-stat* allows inspection of a Merkle Tree, including the leafset.

# OPTIONS
*-h, --help*
	Print a short help message and exit.

*-f, --format*=_FMT_
	Print the leaf information according to _FMT_. _FMT_ can be
	exactly one of *none*, *list*, or *tree*; printing either
	no information, a parseable list, or a simple visualization
	respectively. If not specified, *list* is assumed.

# EXAMPLES
*mt-stat -cf list ./example.mt*
	Verifies the integrity of *example.mt* in the current directory, and lists
	its leaves.

# BUGS
Report bugs for mt to https://github.com/HalosGhost/mt/issues
