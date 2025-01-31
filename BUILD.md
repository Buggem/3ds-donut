# Building from source

Building requires nothing but Docker (the Docker Desktop-bundled CLI is also applicible). All dependancies are included in the container once built.

If you can replace any files in `utils/bin` with executables you compiled yourself for piece of mind. The Docker container grabs the deps from there (it also grabs the devkitARM container from the web). They are there for purely my own convenience


```
docker build -t devkitarm_utils .
docker run -v "$PWD":"$PWD" --rm -ti devkitarm_utils bash -c "$PWD/.github/ci.sh \"$PWD\""
```
