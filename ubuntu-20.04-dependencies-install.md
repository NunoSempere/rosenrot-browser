sudo apt install libwebkit2gtk-4.0-dev
sudo apt install clang

git clone https://github.com/jun7/wyebadblock
cd wyebadblock
make
sudo make install 

user="loki"
cp config.def.h config.h
sed "s/fenze/$user/g" config.h

./install.sh
