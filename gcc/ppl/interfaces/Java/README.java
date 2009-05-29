The Java interface of the Parma Polyhedra Library has been tested
with both Sun's JDK 6 (http://java.sun.com/javase/downloads/)
and the open-source OpenJDK 6 (http://openjdk.java.net/).

The Java interface provides the `parma_polyhedra_library' package:
after installing the Parma Polyhedra Library in <PPL prefix>,
the ppl_java.jar JAR file and the JNI shared object will be installed
in directory <PPL prefix>/<libdir>/ppl.

The Java compilation command for `My_Test.java' should then be
something like:

$ javac -classpath .:<PPL prefix>/<libdir>/ppl/ppl_java.jar My_Test.java

This will produce class file My_Test.class, which can then be executed
by issuing a command like:

$ java -classpath .:<PPL prefix>/<libdir>/ppl/ppl_java.jar My_Test

Note that the source code in My_Test.java should take care of loading
the PPL interface library, by calling `System.load' and passing the
full path of the dynamic shared object. For instance, on a Linux machine
and assuming <PPL prefix>=/usr/local/, the call will be something like:

  System.load("/usr/local/lib/ppl/libppl_java.so");

