# Key dependencies
sudo apt install libwebkitgtk-6.0-dev
sudo apt install libgtk-4-dev

# Optional adblock
git clone https://github.com/jun7/wyebadblock
cd wyebadblock
sudo apt install gstreamer1.0-plugins-good gstreamer1.0-libav
WEBKITVER=4.1 make
sudo WEBKITVER=4.1 make install

cd ..
mkdir -p ~/.config/wyebadblock
cd ~/.config/wyebadblock
wget https://easylist.to/easylist/easylist.txt
cd -

cd ../..
make build
sudo make install 

cd  - 
# Debian desktop icon
chmod +x rosenrot.desktop
sudo cp rosenrot.desktop /usr/share/applications
mkdir -p /opt/rosenrot
sudo cp rosenrot-desktop-icon.png /opt/rosenrot/

# Optionally, set a shortcut (within GNOME)
# https://askubuntu.com/questions/597395/how-to-set-custom-keyboard-shortcuts-from-terminal/1007035#1007035
name="rose"
binding="<CTRL><SHIFT>W"
action="/usr/bin/rosenrot"

media_keys=org.gnome.settings-daemon.plugins.media-keys
custom_kbd=org.gnome.settings-daemon.plugins.media-keys.custom-keybinding
kbd_path=/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/$name/
new_bindings=`gsettings get $media_keys custom-keybindings | sed -e"s>'\]>','$kbd_path']>"| sed -e"s>@as \[\]>['$kbd_path']>"`
gsettings set $media_keys custom-keybindings "$new_bindings"
gsettings set $custom_kbd:$kbd_path name "$name"
gsettings set $custom_kbd:$kbd_path binding "$binding"
gsettings set $custom_kbd:$kbd_path command "$action"
