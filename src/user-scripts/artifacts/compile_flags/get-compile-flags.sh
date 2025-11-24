DEPS='webkit2gtk-4.0'
INCS=`pkg-config --cflags ${DEPS}`
LIBS=`pkg-config --libs ${DEPS}`
echo $INCS
echo $LIBS
