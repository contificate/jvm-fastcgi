JAVA_HOME=/lib/jvm/default/
javac Server.java
gcc stub.c -o stub -I$JAVA_HOME/include -I$JAVA_HOME/include/linux -L$JAVA_HOME/lib/server -ljvm -lfcgi
LD_PRELOAD=$JAVA_HOME/lib/server/libjvm.so lighttpd -D -f lighttpd.conf
