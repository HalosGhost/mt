mt(1)

# NAME
mt - A simple Merkle Tree multitool.

# SYNOPSIS
*mt* [subcommand] [arguments]

# DESCRIPTION
*mt* is, effectively, a simple dispatcher that passes all arguments on
to the specified subcommand. See the relevant page for each subcommand's
usage.

If no subcommand is specified, *mt* lists all known
subcommands. Equivalently, you may specify the special subcommand *help*
to get the same list.

# GENERAL USAGE

```
$ mt create -b32 -f/path/to/message [...] -o new.mt
```

```
$ mt stat new.mt
```

```
$ mt prove -r new.root -p new.proof -K0 new.mt
```

```
$ mt verify -r new.root new.proof
```

# BUGS
Report bugs for mt to https://github.com/HalosGhost/mt/issues
