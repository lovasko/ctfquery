# ctfquery
Query the CTF for specific information.

## Usage
`ctfquery [-s SYM | -t TYPE_ID | -c TYPE_ID | -l LABEL | -v | -h ] <file>` 
 * `-s SYM` search for a symbol named `SYM`
 * `-t TYPE_ID` search for a type identified with `TYPE_ID`
 * `-c TYPE_ID` solve a `typedef` chain with the head identified with `TYPE_ID`
 * `-l LABEL` search for a label with name `LABEL`
 * `-v` print CTF version of the file
 * `-h` print help message

## Build
```
$ ninja
```

## Dependencies
 * libctf

## Example
### ... or the tale of the maximal process ID 
Grab my hand, we are going for a trip! Say we are, for some reason, totally
into finding out about the kernel symbol `pid_max` which denotes the maximal
PID on a system. One way is to look into the source, sure, but what if it is
not available and we are stuck with some random black box crash dump?
`ctfquery` to the rescue!

We will start by gently poking the beast:

```
$ ctfquery -s pid_max /boot/kernel/kernel
776
```

The first bit of knowledge! The symbol inspection (`-s`) told us that the
type ID of the type associated with the symbol is 776. Only if we could
describe a type!

```
$ ctfquery -t 776 /boot/kernel/kernel
    Kind: typedef
  Ref ID: 775
New name: pid_t
```

And we could. A type with ID 775 disguised as the `pid_t` using the `typedef`
sorcery. Sneaky. Let's dive even further!

```
$ ctfquery -t 775 /boot/kernel/kernel
    Kind: typedef
  Ref ID: 17 
New name: __pid_t
```

Oh man! This is a serious `typedef` hell! For how long will we have to follow
these links? Luckily, we can solve typedef chains with the `-c` option for free:

```
$ ctfquery -c 776 /boot/kernel/kernel
pid_t (776) -> __pid_t (775) -> __int32_t (17) -> int (16)
```

Finally, some solid info right there, it all seems to boil down to the type with
ID 16. You guessed correctly, we're gonna inspect the hell out of it!

```
$ ctfquery -t 16 /boot/kernel/kernel
   Kind: int
   Name: int
   Size: 32
 Offset: 0
 Signed: yes
Content: number
```

QED! The symbol `pid_max` is a 32-bit signed integer. Now go solve the
important stuff.

## License
2-clause BSD, for more information please see the [license](LICENSE.md).

## Author
Daniel Lovasko lovasko@freebsd.org

