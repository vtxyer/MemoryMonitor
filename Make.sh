#!/bin/bash
sudo make  xen && sudo make install-xen && cp xen/include/public/xen.h /usr/include/xen/xen.h
#sudo make -j24 xen && make tools && sudo make -j24 install-xen && sudo make install-tools
