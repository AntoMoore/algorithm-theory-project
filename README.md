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
$ ./sha512 ./input.txt
```

Testing the Results:
```python
$ make test
```

Cleaning the Test Environment:
```python
$ make clean
```

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
     style="float: center; margin-center: 10px;" />   
     
A hash algorithm is designed to ensure that each bit of the hash digest is dependent upon every bit contained within the input. This means the output hash digest can not have individual bits calculated separately.

Changing one single bit in the input is designed to result in a large deviation caused by the algorithm and will significantly alter the final hash value. This effect is known as Avalanching [5] and is intended to prevent patterns and relationships forming between inputs and outputs.

Hashing algorithms contain non-linear operations. This makes them resistant to linear algebra techniques of reverse engineering the input from a given output.

These characteristics make SHA512 technically impossible to reverse. The only practical way of finding a matching hash is to use brute force and keep guessing random inputs until you find a hash value that matches the hash digest from the original message [6].

## Can you design an algorithm, that given enough time, will find input messages that give each of the possible 512-bit strings?

Through careful design, hash functions are engineered in such a way that the most effiecient way to carry out a preimage attack is by brute force methods.

A preimage attack on a hash function refers to finding messages that have specific hash values [7]. Given the amount of pre-specified outputs, it is compotationally impractical to find all of the inputs with matching hash digests [8]. 

     "An expected property of a cryptographic hash function such that given a randomly chosen message digestmessage_digest, it is computationally infeasible to find a preimage of the message_digest."

     Preimage Resistance - NIST SP 800-106

As an expermient giving ourselves every possiable advantage. We will attempt to calculate the estimated time it would take to calculate every possible SHA512 hash digest.

Referencing benchmarks from the world's fastest 8-GPU system in 2016 [9] that was using hashcat [10] to calculate hash digests, we will be able to achive approx 8624.7 MH/s.

### Total amount of possiable SHA512 hash digests:
     512-bit = 2 ^ 512   
     total = 1.34 x 10 ^ 154    

### Divide by the bechmark speed to calculate time:
     speed = 8624.7 MH/s   
     time = total / speed   
     time = 1.55 x 10 ^ 144 (seconds)   

### Converted Time Metrics:

| Time               |  Metric   |
|--------------------|-----------|
| 2.59 x 10 ^ 142    |  Minutes  |
| 4.31 x 10 ^ 140    |  Hours    |
| 1.79 x 10 ^ 139    |  Days     |
| 4.92 x 10 ^ 136    |  Years    |

### To put this into context:
- The life expentency of our sun is approx: 5 x 10 ^ 9 years [11].
- A supermassive black hole will evaporate in approx: 2 x 10 ^ 100 years [12].

So while it is technically possiable to design an algorithim that can use a brute force method to calculate all of the possible 512-bit strings, it will probably take longer than the lifetime of the universe for it to be completed using present day technology.

## How difficult is it to find a hash digest beginning with at least twelve zeros?

In cryptocurrency mining, a target hash is a numeric value that a hashed block header must be less than or equal to in order for a new block to be awarded to a miner. Block headers identify individual blocks in a blockchain.

The target hash is used in determining the difficulty of the input and can be adjusted in order to ensure that blocks are processed efficiently. For example, target hashes are used in cryptocurrencies that use a proof-of-work (PoW) system to set the current mining difficulty (including Bitcoin).

Cryptocurrency difficulty is a parameter that bitcoin and other cryptocurrencies use to keep the average time between blocks steady as the network's hash power changes. Cryptocurrency difficulty is important since a high difficulty can help secure the blockchain network against malicious attacks.

     Difficulty = MAX_TARGET / current_target

### The MAX_TARGET will be a SHA512 digest with 12 leading zeros:

     Hexadecimal:
     0x000000000000fffffffffffffffffffffffffffffffffffffffffffffffffff
     fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff

     Decimal:
     4.76 x 10 ^ 139

### The current_target will be the maximum value of a 512-bit number.

     Hexadecimal:
     0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
     fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff

     Decimal:
     1.34 x 10 ^ 154

### Calculate the Probability

     Probability = (4.76 x 10 ^ 139) / (1.34 x 10 ^ 154)
     Probability = 3.55 x 10 ^ -15

## References

[1] FIPS PUB 180-4 - https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf

[2] Encryption vs Hashing - https://medium.com/@caseygibson_42696/encryption-vs-hashing-290d12e36ed4

[3] Cryptographic hash function - https://en.wikipedia.org/wiki/Cryptographic_hash_function#Applications

[4] Merkle–Damgård construction - https://en.wikipedia.org/wiki/Merkle%E2%80%93Damg%C3%A5rd_construction

[5] Avalanche Effect in Cryptography - https://www.geeksforgeeks.org/avalanche-effect-in-cryptography/

[6] Why cant we reverse hashes? - https://crypto.stackexchange.com/questions/45377/why-cant-we-reverse-hashes

[7] Cryptographic Hash-Function Basics - https://web.cs.ucdavis.edu/~rogaway/papers/relates.pdf

[8] NIST Special Publication 800-106 - https://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-106.pdf

[9] 8x Nvidia GTX 1080 Hashcat Benchmarks - https://gist.github.com/epixoip/a83d38f412b4737e99bbef804a270c40

[10] hashcat - https://hashcat.net/hashcat/

[11] The Sun - https://pwg.gsfc.nasa.gov/istp/outreach/workshop/thompson/facts.html#:~:text=Existing%20for%20about%204%20and,into%20a%20small%20white%20dwarf.

[12] Particle emission rates from a black hole - https://journals.aps.org/prd/abstract/10.1103/PhysRevD.13.198