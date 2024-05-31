mt-create(1)

# NAME
mt-verify - Verify a Merkle Proof

# SYNOPSIS
*mt-verify* <--root=RT> <message> <sibling [sibling…]>

# DESCRIPTION
*mt-verify* verifies a Merkle Proof for a Merkle Root built using
*mt-create(1)*.

While the root and message can be specified in any order, siblings are
position-relative to one another (the first specified sibling is the lowest
sibling in the proof, etc.). When calculating the parent node's hash, the
message's ancestor and sibling are sorted by their hashes first; this avoids
the caller having to specify an ordering manually.

To verify a Merkle Proof, the hash of the message is taken; then, in passed-
order left-to-right, the hash is iteratively concatenated with the sibling
and hashed. If the path was valid, the final hash will match the root.

# OPTIONS

*-h, --help*
	Print a short help message and exit.

*-R, --root=HASH*
	Consider HASH to be the Merkle Root. Must be provided.

*-s, --from-str=STR*
	Treat STR as the message to verify. Exactly one message must be provided.

*-f, --from-file=PATH*
	Treat file at PATH as the message to verify. Exactly one message must be provided.

*-p, --proof=HASH*
	Add HASH as a proof segment (sibling), ordered next to the message-
	ancestor lexicographically.

# EXAMPLES

*mt-verify -R 252a6e84845461b7 -s 'MTs are cool!' -p b4c015a40380d377*
	Verifies that 'MTs are cool!' is one of two leaves in the 64-bit Merkle Root
	252a6e84845461b7.

# BUGS
Report bugs for mt to https://github.com/HalosGhost/mt/issues
