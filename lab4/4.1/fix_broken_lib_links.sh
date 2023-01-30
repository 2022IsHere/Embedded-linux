#!/bin/bash

printf "\nFixing broken library links in the cross-build environment.\n\n"

cd /var/lib/schroot/chroots/rpizero-bullseye-armhf/usr/lib/arm-linux-gnueabihf


sudo ln -Tfs  libresolv.so.2 ./libresolv.so
sudo ln -Tfs  libm.so.6 ./libm.so
sudo ln -Tfs  libpthread.so.0 ./libpthread.so
sudo ln -Tfs  libthread_db.so.1 ./libthread_db.so
sudo ln -Tfs  libnss_compat.so.2 ./libnss_compat.so
sudo ln -Tfs  libtirpc.so.3.0.0 ./libtirpc.so
sudo ln -Tfs  libnss_files.so.2 ./libnss_files.so
sudo ln -Tfs  libanl.so.1 ./libanl.so
sudo ln -Tfs  libdl.so.2 ./libdl.so
sudo ln -Tfs  libutil.so.1 ./libutil.so
sudo ln -Tfs  libBrokenLocale.so.1 ./libBrokenLocale.so
sudo ln -Tfs  librt.so.1 ./librt.so
sudo ln -Tfs  libnss_dns.so.2 ./libnss_dns.so
sudo ln -Tfs  libnss_hesiod.so.2 ./libnss_hesiod.so
sudo ln -Tfs  libexpat.so.1.6.12 ./libexpat.so
sudo ln -Tfs  libcrypt.so.1 ./libcrypt.so
sudo ln -Tfs  libz.so.1.2.11 ./libz.so

printf "\nDone.\n"
