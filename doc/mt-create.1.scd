mt-create(1)

# NAME
mt-create - Create a Merkle Tree

# SYNOPSIS
*mt-create* [options]

# DESCRIPTION
*mt-create* builds a binary Merkle Tree from a set of messages.

The messages can be interspersed with (randomly-rolled) decoys, and a required
number of leaves can be specified to guarantee a given anonymity set size.
Messages are specified in leaf-order, left-to-right!

*mt-create* guarantees that the leaf-count is a power-of-two so the tree will
always be balanced (guaranteeing all valid Merkle Paths are of equal length).

# OPTIONS

*-h, --help*
	Print a short help message and exit.

*-b, --bytesize=NUM*
	Use hashes of NUM bytes long. (maximum and default value of 64).

*-l, --leaf-count=NUM*
	Require NUM leaves in the resulting tree. If fewer than NUM messages are
	provided, *mt-create* will randomly-roll additional messages to meet the
	specification. NUM must be >1, and will be rounded-up to the nearest power-
	of-to.

*-s, --from-str=STR*
	Include STR as a leaf in the tree.

*-f, --from-file=PATH*
	Include file at PATH as a leaf in the tree.

*-d, --decoy*
	Randomly-roll a message.

# EXAMPLES
*mt-create -b16 -d -s foo -d -d -s 'bar baz' -d*
	Creates a 128-bit Merkle Tree with 8 leaves; the second leaf is 'foo' and
	the fifth is 'bar baz'.

# BUGS
Report bugs for mt to https://github.com/HalosGhost/mt/issues
