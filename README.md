# Befungell

![logo](assets/logo.png?raw=true)

Welcome to the homepage for Befungell, an implementation of concurrent Befunge, designed for [SIGBOVIK 2017](http://sigbovik.org/2017/). You can find a link to the original paper here (shortly), or you can clone the code and play with it yourself. 

## Making the Project

Run `make` to generate a binary called `befunge`. While you can interface with this binary manually, you may want to use the python loader program which makes it easier to load a program from an existing file. To use it, run `python loader.py progs/<prog.bef>`.

## The Befungell Specification

Befungell by and large acts just like Befunge 93 (The specification can be found [here](https://esolangs.org/wiki/Befunge#Instructions)). However, it introduces three new language constructs for parallelism and concurrency.

The first operator, `=` is used to initiate thread splitting. When the cursor enters this block, it pauses and spawns two new threads on either side of the equal sign facing away from the spawner with stacks copied from their parent. Once each thread terminates, their top value will be popped off the stack and pushed onto the stack of the controlling thread. Then, the original thread will continue execution.

The second two operators are designed for concurrent operations. The operator `{` enters block mode and effectively decrements a semaphore (initialized to 1). Likewise the operator `}` leaves block mode and effectively increases a semaphore. Thus, there can be used to wrap critical sections of code so that they can be executed atomically.

## Examples

Several example befunge programs are available in `/progs/`. You can also find examples of non-parallel befunge applications throught the internet.

## Contributing

...why would you want to contribute to this? Well if you do, just issue a PR. I'll probably accept it. 

## Credits

Follow me on twitter @zwad3 or on github /zwade

