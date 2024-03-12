# Key dependencies
sudo apt install git vim gcc make
sudo apt install libwebkit2gtk-4.1-dev

# Optional adblock
git clone https://github.com/jun7/wyebadblock
cd wyebadblock
sudo apt install gstreamer1.0-plugins-good gstreamer1.0-libav
make
sudo make install 
cd ..
mkdir -p ~/.config/wyebadblock
cd ~/.config/wyebadblock
wget https://easylist.to/easylist/easylist.txt
cd -

cd ../..
make build # or just make
sudo make install 

cd  - 
# Ubuntu desktop icon
chmod +x rose.desktop
sudo cp rose.desktop /usr/share/applications
