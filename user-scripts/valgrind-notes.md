In case you arrive at a segmentation fault when working on rose, you can use valgrind.

To do this, you can compile rose with the `DEBUG` value in `build.sh` set to `-g`

and then:

```
valgrind --track-origins=yes ./rose
```
