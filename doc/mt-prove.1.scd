mt-prove(1)

# NAME
mt-prove - Prove properties of messages in a Merkle Tree

# SYNOPSIS
*mt-prove* [options] <path/to/tree>

# DESCRIPTION
*mt-prove* can create proofs about the messages included in a specified
Merkle Tree. In particular, it can prove knowledge of an included message
(i.e., that the prover knows of a message which is included), or prove
that a specific message is included itself.

In addition, it offers an easy mechanism to export an encoded Merkle Root
from its Tree. If no proof or root are requested, *mt-prove* prints the
encoded Root to *stdout*.

# OPTIONS
*-h, --help*
	Print a short help message and exit.

*-I, --inclusion*=_NUM_
	Prove that the message at leaf-index _NUM_ is included in
	the root.  *This will include the full message in the proof
	verbatim!* Mutually-exclusive with *-K*.

*-K, --knowledge*=_NUM_
	Prove knowledge that the message at leaf-index _NUM_ is
	included in the root.  This will only include the hash of the
	chosen message rather than the message itself. *Note*: this is
	only sufficient to convince a verifier that you know that *a*
	message is included, not that *a specific* message is included.
	Mutually-exclusive with *-I*.

*-b, --bytesize*=_NUM_
	Override the bytesize set in the tree and use hashes of _NUM_
	bytes instead.	_NUM_ must be an integer in [1, 64].

	*Note: proofs will only verify for a root given the bytesize matches.*

*-p, --proof*=_PATH_
	Write the textual encoding of the proof to _PATH_. If _PATH_
	is the special value *-*, *mt-prove* will print the encoding
	to *stdout*. If *-K* or *-I* is specified and *-p* is not,
	*mt-prove* behaves as if *-p-* had been passed. Unless _PATH_
	is *-*, it must differ from *-r*'s argument (if any).  Takes no
	effect if neither *-K* nor *-I* are specified.

*-r, --root*=_PATH_
	Write the textual encoding of the root to _PATH_. If _PATH_
	is the special value *-*, *mt-prove* will print the encoding
	to *stdout*.  Unless _PATH_ is *-*, it must differ from *-p*'s
	argument (if any).

# EXAMPLES
*mt-prove -K3 ./822f608a.mt*
	Creates a Merkle Knowledge Proof for the fourth leaf of the tree
	stored in the file named `822f608a.mt` in the current directory.

# BUGS
Report bugs for mt to https://github.com/HalosGhost/mt/issues

# SEE ALSO
*mt-rationale*(7), *mt*(1), *mt-create*(1), *mt-stat*(1)
