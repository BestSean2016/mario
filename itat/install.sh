#!/bin/sh

LIBPATH=/usr/lib
PYPATH=/opt/python2.7.13

cp *.so.* $LIBPATH
ln -sf $LIBPATH/libitat.so.1.0.0 $LIBPATH/libitat.so.1
ln -sf $LIBPATH/libitat.so.1.0.0 $LIBPATH/libitat.so
ln -sf $LIBPATH/libitat.so.1.0.0 $PYPATH/lib/python2.7/site-packages/libitat.so
ln -sf $LIBPATH/libitat.so.1.0.0 $PYPATH/lib/python2.7/site-packages/libitat.so.1

ln -sf $LIBPATH/libigraph.so.0.0.0 $LIBPATH/libigraph.so.0
ln -sf $LIBPATH/libigraph.so.0.0.0 $LIBPATH/libigraph.so
ln -sf $LIBPATH/libboost_python.so.1.63.0 $LIBPATH/libboost_python.so.1
ln -sf $LIBPATH/libboost_python.so.1.63.0 $LIBPATH/libboost_python.so

cp -f bill_message.py $PYPATH/lib/python2.7/site-packages/
