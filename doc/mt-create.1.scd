mt-create(1)

# NAME
mt-create - Create a Merkle Tree

# SYNOPSIS
*mt-create* [options]

# DESCRIPTION
*mt-create* builds a binary Merkle Tree from a set of messages.

The messages can be interspersed with (randomly-rolled) decoys, or a minimum
number of leaves can be specified to guarantee a given anonymity set size.

*mt-create* guarantees that the leaf-count is a power-of-two so the tree will
always be balanced (guaranteeing all valid Merkle Paths are of equal length).

If there are manually-specified decoys, *mt-create* will place each message and
decoy in-specified-order, left-to-right. Otherwise, D decoys will be rolled at
random, where D = next_pow2(max(message_count, leaf_count)), and each message
will be next to a decoy leaf.

*mt-create* sorts both child nodes by their hashes before calculating each
parent node's hash; this should have no security reduction, and simplifies the
API and proof size for *mt-verify*. Additionally, before each parent node's
hash is calculated, the preimage has a byte appended representing the number of
descendants the parent node has (`0x00` for leaf nodes). This limits the
allowable depth of the tree to 255 (maximum of 2^255 leaves), but offers
second-preimage resistance.

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

*-o, --out=PATH*
	Write the textual encoding of the tree at PATH. If PATH is the special
	value *-*, *mt-create* will print the encoding to *stdout*. If not
	specified, it will write to *<fingerprint>.mt*

# EXAMPLES
*mt-create -b16 -d -s foo -d -d -s 'bar baz' -d*
	Creates a 128-bit Merkle Tree with 8 leaves; the second leaf is 'foo' and
	the fifth is 'bar baz'. Leaves 1, 3, 4, and 6 are explicitly randomly-rolled
	decoys; leaves 7 and 8 are implicitly randomly-rolled decoys.

# BUGS
Report bugs for mt to https://github.com/HalosGhost/mt/issues
