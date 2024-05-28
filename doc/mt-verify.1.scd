mt-create(1)

# NAME
mt-verify - Verify a Merkle Proof

# SYNOPSIS
*mt-verify* <--root=RT> <--from-str=STR> <sibling [sibling…]>

# DESCRIPTION
*mt-verify* verifies a Merkle Proof for a Merkle Root built using
*mt-create(1)*.

While the root and message can be specified in any order, siblings are
position-relative to one another (the first specified sibling is the lowest
sibling in the proof, etc.). Siblings are specified as either being to 
the `left` or the `right`. The direction of a sibling declares whether or not
the sibling branch is to the left or right of the message ancestor's along the
path to the root.

To verify a Merkle Proof, the hash of the message is taken; then, in passed-
order left-to-right, the hash is iteratively concatenated with the sibling
(ordered as-specified by the sibling) and hashed. If the path was valid, the
final hash will match the root.

# OPTIONS

*-h, --help*
	Print a short help message and exit.

*-R, --root=HASH*
	Consider HASH to be the Merkle Root. Must be provided.

*-s, --from-str=STR*
	Treat STR as the leaf to verify. Must be provided.

*-l, --left=HASH*
	Add HASH as a left sibling (will be ordered *before* the message-ancestor).

*-r, --right=HASH*
	Add HASH as a right sibling (will be ordered *after* the message-ancestor).

# EXAMPLES

*mt-verify -b8 -R 5ccb81f5e3243d69 -s 'MTs are cool!' -l 49dc9017001960e0*
	Validates that 'MTs are cool!' is the second leaf in the 64-bit Merkle Root
	5ccb81f5e3243d69.

# BUGS
Report bugs for mt to https://github.com/HalosGhost/mt/issues
