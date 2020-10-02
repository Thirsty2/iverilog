To build this Dockerfile with the latest code in master, from
its location within the alpine312 subdirectory:
cd alpine312
docker build . -t iverilog

To build using your own fork and branch, pass your fork's URL and branch as --build-arg params like this:
docker build --build-arg IVERILOG_REPO_URL=https://github.com/Thirsty2/iverilog.git --build-arg IVERILOG_BRANCH=add-dockerfile --build-arg TEST_BRANCH=add-dockerfile . -t iverilog

If you prefer, you can tag it with the release number, or latest:
docker build . -t iverilog:latest

The line in the Dockerfile that sets the default branch to build is: 
ARG IVERILOG_BRANCH=master

To build this Dockerfile with the code in the v10_3 tagged release, use 
--build-arg to override IVERILOG_BRANCH to pull the v10_3 tag with:
docker build --build-arg IVERILOG_REPO_URL=https://github.com/Thirsty2/iverilog.git --build-arg IVERILOG_BRANCH=v10_3 --build-arg TEST_BRANCH=add-dockerfile . -t iverilog:10.3

The default target container of this dockerfile has no entrypoint, so you 
can run a shell (sh or ash) in interactive mode with a command like this:
docker run -it -v $(pwd):/home/ic iverilog sh

The -v (volume) command above maps the current directory on your system to
the /home/ic directory inside the container.

Try running the above command from the test directory:
cd test
docker run -it -v $(pwd):/home/ic iverilog sh

Then, from the shell prompt in the container, try running make:

~ $ ls
Makefile    hello.v     hello_tb.v  readme.txt
~ $ make
iverilog -o hello_tb.vvp hello_tb.v
vvp hello_tb.vvp
VCD info: dumpfile hello_tb.vcd opened for output.
Hello World test complete
~ $ ls
Makefile      hello.v       hello_tb.v    hello_tb.vcd  hello_tb.vvp  readme.txt
~ $ make clean
rm hello_tb.vvp hello_tb.vcd
~ $ ls
Makefile    hello.v     hello_tb.v  readme.txt
~ $

It is also easy to build a docker container from this dockerfile with
an entrypoint that runs make, with an optional command passed 
as an argument at the end of the command line.  

docker build --target iverilog-make . -t iverilog-make

Again, to build your own fork and branch:
docker build --build-arg IVERILOG_REPO_URL=https://github.com/Thirsty2/iverilog.git --build-arg IVERILOG_BRANCH=add-dockerfile --build-arg TEST_BRANCH=add-dockerfile --target iverilog-make . -t iverilog-make

Now, when you run this container, it automatically invokes make in the current directory:
Try this in the test directory:

~/iverilog/dockerfiles/test$ ls
Makefile  hello.v  hello_tb.v  readme.txt
~/iverilog/dockerfiles/test$ docker run -v $(pwd):/home/ic iverilog-make
iverilog -o hello_tb.vvp hello_tb.v
vvp hello_tb.vvp
VCD info: dumpfile hello_tb.vcd opened for output.
Hello World test complete
~/iverilog/dockerfiles/test$ ls
Makefile  hello.v  hello_tb.v  hello_tb.vcd  hello_tb.vvp  readme.txt

If you have gtkwave installed, use it to view the .vcd file
~/iverilog/dockerfiles/test$ gtkwave hello_tb.vcd

Run the container again, passing the clean command to the make entrypoint:

~/iverilog/dockerfiles/test$ docker run -v $(pwd):/home/ic iverilog-make clean
rm hello_tb.vvp hello_tb.vcd
~/iverilog/dockerfiles/test$ ls
Makefile  hello.v  hello_tb.v  readme.txt

