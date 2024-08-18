mt-rationale(7)

# NAME
mt-rationale - What? Why did you do this?!

# SYNOPSIS
To learn! This library and CLI toolset exposes an incredibly sharp
cryptographic primitive in a very easily accessible way. Some care has
been taken to make it easier to hold; but “easily accessible” and
“easier to hold” do not imply “hard to hold wrong.” At this
point, it seems clear that hard-to-hold-wrong should be a pre-requisite
for any new tool to be considered anything more than a *toy*.

# DESCRIPTION
Thankfully, this is a toy! In some sense, it's an exercise in playing
with some of the trappings of modern (or semi-dated) cryptographic
technologies, though not engaging fully (e.g., the encoding of the
core data-types is done ad-hoc rather than via ASN.1 as is far more
standard/typical/expected). On the other, it's a bit of an art project,
imagining something of an alternate reality where we developed some of
our conventions and standards, but the field had barely made progress
on useful, high-level primitives, and so the tooling available exposes
something very anachronistic. And yet playing with a primitive that has
seen a lot of recent use, evolution, and maturation.

Regardless of how you view this tool¹, it is a *toy*, and should be
used only for education (or alternate reality role-playing), not relied
upon for security.

¹: Equivalently, as a mechanism for making hash-commitments with
well-defined anonymity sets, and blinding for what was committed---in
this case, the root is the commitment, and the path is an opening; or,
as a Few-Time hash signature scheme where all messages signable with a
given public key must be known up-front---the root then is the public key,
and a path is the signature (enabling signatures to prove either inclusion
of a specific message or knowledge of some message that was committed to).

_To be more blunt:_ This code and design have *not* undergone rigorous
security review, and I am *not* a formally trained cryptographer. Caveat
artifex.

# QUALITY OF LIFE
In an effort both to explore and make this contraption slightly
less terrifying, several steps have been taken to make use of it,
theoretically, less insecure.  They are documented below:

1. By default, the number of leaves for a generated tree is set to the
next power of two greater than the number of committed messages. All
these additional messages are random data, each 64 bytes long. From
the perspective of a commitment scheme, this means at most half of
the messages in the tree are meaningful. And, where N is the number of
committed messages, the anonymity set for each message is strictly ≥2N,
and therefore you have strictly ≥64N bytes of randomness present in
the tree.

2. Additionally, every included message's sibling at the leaves of the
tree is one of these uniformly random messages. From the perspective of a
signature scheme, this ensures that a message sign*able* by many different
keys is not revealed to have been included by any other keys than those
who publicly revealed signatures. This is a very subtle security property,
but can be intuitively understood as every message's signature including
64-bytes of additional randomness. Even if a specific message is signable
by many different keys, and those keys provided many signatures of other
included messages, no signature ever leaks that a non-revealed message
has been included in any other tree.

3. All preimages in the tree are prefixed with a domain-separating height
tag. While resisting second-preimage attacks should be accomplishable
by just domain-separating the leaf hashes from the internal hashes,
prefixing every preimage with its node's height in the tree places a
hard-limit on the tree-height which is public upon revelation of any
proof/path anyway.  As the tree height can still be up to 255, this
feels a totally acceptable limit.

4. Sibling hashes are lexicographically ordered before hashing. This
means proofs/paths only need to specify the sibling hashes, and need
not worry about left/right ordering as the tree was originally rendered.

5. All proofs include the root. From either perspective¹, this is a
bit wasteful.  Most use-cases will likely expect the root to be revealed
in-advance. However, this makes all proofs statically verifiable, at
the expense of a few bytes.

# POTENTIAL FOOTGUNS
1. The byte-width of the tree is freely settable thanks to Monocypher's
Blake2b implementation exposing all widths directly. This lets you do
obviously useless and silly things like have a 1-byte merkle root, where
the PEM header radically dwarfs the size of the encoded proof. So long
as the plaintext messages are retained (which is the default in attached
proofs), the byte-width can even be overridden when generating paths,
or deriving roots. *Note*: paths/proofs will only verify if the root
was derived at the same byte width.

2. In an attempt to enable exploration and whimsy, the tools enable you
to disable any of the defaults and specify the tree topology exactly
to-specification. This hopefully nets anyone playing with it a great
deal of freedom and ideally enables some level of intuition to develop,
but is also a bit like making sure the sharpest knife in the kitchen
is always left on the countertop. It is trivial to construct numerous,
terribly insecure schemes (even if all this code met the highest standard
of quality).

# BUGS
Report bugs for mt to https://github.com/HalosGhost/mt/issues

# SEE ALSO
*mt*(1)
