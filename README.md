# algorithm-theory-project

## Description
- Explain contents of repo

## Getting Started

Installing GCC on Linux:

```python
$ sudo apt update
$ sudo apt install build-essential
```

Verify that the GCC compiler is installed correctly:
```python
$ gcc --version
```

Compiling The Program:
```python
$ make sha512
```

Running the Application:
```python
$ ./sha512 ./filename
```

Testing:
- tbc

## Overview of SHA512
The SHA512 algorithm  is part of a collection of hash functions, standardized by NIST as part of the Secure Hash Standard in FIPS 180-4 [1]. 

Hashing functions operate by taking in some form of data, which is referred to as a hash digest and generate an output to a fixed length. The SHA512 algorithm operates on a 1024-bit message block with a 512-bit intermediate hash value [2].

Due to this constraint in order to facilitate the message hashing, the input must be first "Padded" to reach size that is a multiple of 1024-bits.

The algorithm works by processing each block of 1024-bits from the original message, using the result from the previous block to randomise the values.

After every block of 1024-bits goes through the message processing phase, we get the final 512-bit Hash value of our original message.

SHA512 is used in a wide range of applications [3], such as:

- Verifying the integrity of messages and files is done by comparing the hash digest of the sent and received message, to help determine if any changes have been made to the original message.

- Signature generation and verification in enabled by recalculating the hash digest over a given message. Digital signatures can be used to authenticate the identity of the source message, through the use of security keys specific to individual users. 

- Password verification is achieved by storing the hash digest of each password in a database. To authenticate a user, they first enter their password which is then turned into a hash digest. This hashed input is then compared to the hash digest of the users password on the database for verification. 

The SHA-2 family of hash function are implemented in some widely used security applications and protocols, including TLS and SSL, PGP, SSH, S/MIME, and IPsec.

## Why cant we reverse the SHA512 algorithm to retrieve the original message from a hash digest? 

During a hash function there are an exponential number of one-way operations that take place as the original message is processed in blocks of 1024-bits with a 512-bit intermediate hash value. The results from one set of operations are then used in later operations.

The SHA-2 family of cryptographic hash functions are built using the Merkle–Damgård structure. This is a method of building collision-resistant cryptographic hash functions from collision-resistant one-way compression functions [4].

<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/e/ed/Merkle-Damgard_hash_big.svg/400px-Merkle-Damgard_hash_big.svg.png"
     alt="Merkle–Damgård hash construction"
     style="float: left; margin-right: 10px;" />

A hash algorithm is designed to ensure that each bit of the hash digest is dependent upon every bit contained within the input. This means the output hash digest can not have individual bits calculated separately.

Changing one single bit in the input is designed to result in a large deviation caused by the algorithm and will significantly alter the final hash value. This effect is known as Avalanching [5] and is intended to prevent patterns and relationships forming between inputs and outputs.

Hashing algorithms contain non-linear operations. This makes them resistant to linear algebra techniques of reverse engineering the input from a given output.

These characteristics make sha512 technically impossible to reverse. The only practical way of finding a matching hash is to use brute force and keep guessing random inputs until you find a hash value that matches the hash digest from the original message [6].

### Can you design an algorithm, that given enough time, will find input messages that give each of the possible 512-bit strings?

- add section about brute forcing

### How difficult is it to find a hash digest beginning with at least twelve zeros?

- add section about crypto currency


| Decimal | Hex | Binary |
|---------|-----|--------|
|       0 |   0 |   0000 |
|       1 |   1 |   0001 |
|       2 |   2 |   0010 |
|       3 |   3 |   0011 |
|       4 |   4 |   0100 |
|       5 |   5 |   0101 |
|       6 |   6 |   0110 |
|       7 |   7 |   0111 |
|       8 |   8 |   1000 |
|       9 |   9 |   1001 |
|      10 |   A |   1010 |
|      11 |   B |   1011 |
|      12 |   C |   1100 |
|      13 |   D |   1101 |
|      14 |   E |   1110 |
|      15 |   F |   1111 |

|Decimal | Hex |   Binary |
|--------|-----|----------|
|    241 |  F1 | 11110001 |
|    170 |  AA | 10101010 |

## References

[1] FIPS PUB 180-4 - https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf

[2] Encryption vs Hashing - https://medium.com/@caseygibson_42696/encryption-vs-hashing-290d12e36ed4

[3] Cryptographic hash function - https://en.wikipedia.org/wiki/Cryptographic_hash_function#Applications

[4] Merkle–Damgård construction - https://en.wikipedia.org/wiki/Merkle%E2%80%93Damg%C3%A5rd_construction

[5] Avalanche Effect in Cryptography - https://www.geeksforgeeks.org/avalanche-effect-in-cryptography/

[6] Why cant we reverse hashes? - https://crypto.stackexchange.com/questions/45377/why-cant-we-reverse-hashes