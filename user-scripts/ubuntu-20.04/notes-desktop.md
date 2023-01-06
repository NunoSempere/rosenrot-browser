For some reason, the Ubuntu environment assigns a program to the rose desktop tab if it is called from within /usr/bin/rose, even if from the Whatsapp desktop file.

The solution is to make a syslink from rose to whatsapp:

```
$ ln -s /usr/bin/rose /usr/bin/whatsapp
```

and then call whatsapp from:

```
/usr/bin/whatsapp https://web.whatsapp.com/
```
