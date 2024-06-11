mt-create(1)

# NAME
mt-create - Create a Merkle Tree

# SYNOPSIS
*mt-create* [options]

# DESCRIPTION
*mt-create* builds a binary Merkle Tree from a set of messages.

The messages can be interspersed with (randomly-rolled) decoys, or a
minimum number of leaves can be specified to guarantee a given anonymity
set size.

*mt-create* guarantees that the leaf-count is a power-of-two so the
tree will always be balanced (guaranteeing all valid Merkle Paths are
of equal length).

If *-x* is specified, *mt-create* will honor the order of decoys and
messages as-passed. It will only add decoys to the end to get to a
power-of-two leaf-count. Otherwise, D decoys will be rolled at-random,
where D = next_pow2(max(message_count×2, leaf_count)), and each message
will be guaranteed to be next to a decoy. For the cost of a few extra
bytes, several edge-cases of accidental information leakage are wholly
mitigated.

*mt-create* sorts both child nodes by their hashes before calculating
each parent node's hash; this should have no security reduction, and
simplifies the API and proof size for *mt-verify*. Additionally, before
each parent node's hash is calculated, the preimage has a byte appended
representing the number of descendants the parent node has (`0x00`
for leaf nodes). This limits the allowable depth of the tree to 255
(maximum of 2^255 leaves), but offers second-preimage resistance.

# OPTIONS

*-h, --help*
	Print a short help message and exit.

*-b, --bytesize*=_NUM_
	Use hashes of _NUM_ bytes long. (maximum and default value of 64).

*-l, --leaf-count*=_NUM_
	Require _NUM_ leaves in the resulting tree. If fewer than _NUM_
	messages are provided, *mt-create* will randomly-roll additional
	messages to meet the specification. _NUM_ must be >1, and will
	be rounded-up to the nearest power-of-two.

*-s, --from-str*=_STR_
	Include _STR_ as a leaf in the tree.

*-f, --from-file*=_PATH_
	Include file at _PATH_ as a leaf in the tree.

*-d, --decoy*
	Randomly-roll a message.

*-x, --no-harden*
	Without this option, *mt-create* automatically ensures all
	included messages have their sibling be a high-entropy decoy. This
	avoids a lot of odd edge cases for information leakage at the
	cost of a few more bytes on-disk, and typically one additional
	element per-proof.

	Instead, if *-x* is specified, *mt-create* will allow you to
	specify all leaves and their order directly (though it will still
	roll additional decoys to guarantee a power-of-two leaf-count).

*-o, --out*=_PATH_
	Write the textual encoding of the tree at _PATH_. If _PATH_ is
	the special value *-*, *mt-create* will print the encoding to
	*stdout*. If not specified, it will write to _<fingerprint>.mt_

# EXAMPLES
*mt-create -b16 -d -s foo -d -d -s 'bar baz' -d*
	Creates a 128-bit Merkle Tree with 8 leaves; the second leaf
	is 'foo' and the fifth is 'bar baz'. Leaves 1, 3, 4, and 6 are
	explicitly randomly-rolled decoys; leaves 7 and 8 are implicitly
	randomly-rolled decoys.

# BUGS
Report bugs for mt to https://github.com/HalosGhost/mt/issues
