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

## Examples
### Symbol search
```
$ ctfquery -s allproc /boot/kernel/kernel
9259
```

## License
2-clause BSD, for more information please see the [license](LICENSE.md).

## Author
Daniel Lovasko lovasko@freebsd.org

