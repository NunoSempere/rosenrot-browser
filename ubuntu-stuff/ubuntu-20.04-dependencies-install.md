sudo apt install libwebkit2gtk-4.0-dev
sudo apt install clang
# sudo apt instal sudo apt install gstreamer1.0-plugins-good gstreamer1.0-libav

git clone https://github.com/jun7/wyebadblock
cd wyebadblock
make
sudo make install 

user="loki"
cp config.def.h config.h
sed "s/fenze/$user/g" config.h

./install.sh

chmod +x ./ubuntu-stuff/rose.desktop
cp ./ubuntu-stuff/rose.desktop /usr/share/applications
