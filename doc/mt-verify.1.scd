mt-create(1)

# NAME
mt-verify - Verify a Merkle Proof

# SYNOPSIS
*mt-verify* [options] </path/to/proof>

# DESCRIPTION
*mt-verify* verifies a Merkle Proof for a given Merkle Root built using
*mt-create*(1) and *mt-prove*(1).

Both detached (proofs of knowledge) and attached (proofs of inclusion)
are supported. All proofs must include the associated root, enabling
proofs to be verified even if a root is not explicitly passed via
*-r*/*-R*. Because, for some cases, this should be relied upon only
as a sanity and integrity check, if a Root is not explicitly provided,
a warning will be printed.

# OPTIONS

*-h, --help*
	Print a short help message and exit.

*-r, --root-hex*=_HEX_
	Read the Merkle Root from the hexadecimal-encoded string, _HEX_.

*-R, --root-pem*=_PATH_
	Read the Merkle Root from the textual encoding at _PATH_.

*-o, --out*=_PATH_
	If the proof demonstrates inclusion, and verifies correctly,
	write the plaintext message to _PATH_. If _PATH_ is the special
	value *-*, *mt-verify* will print the message to *stdout*.

# EXAMPLES

*mt-verify -r 252a6e84845461b7 -s 'MTs are cool!' -p b4c015a40380d377*
	Verifies that 'MTs are cool!' is one of two leaves in the 64-bit
	Merkle Root 252a6e84845461b7.

# BUGS
Report bugs for mt to https://github.com/HalosGhost/mt/issues

# SEE ALSO
*mt-rationale*(7), *mt*(1), *mt-create*(1), *mt-prove*(1)
