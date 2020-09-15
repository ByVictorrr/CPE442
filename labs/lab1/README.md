# Lab 1: Linux and You
## Authors
Victor Delaplaine
## Level 0
level0@management:~$ ls
README
level0@management:~$ cat README 
Congratulations! You figured out how to print.  

flag{level1:RjMY4zR6,4kaFXeBB;%K4#KiKbK6qYmoj}keAGVc}

## Level 1
level1@management:~$ ls
README
level1@management:~$ cat README 
There is a file hidden in this directory.  Can you use ls to find it?
level1@management:~$ ls -la
total 48
drwx------  4 level1 level1 4096 Jun 25  2019 .
drwxr-xr-x 12 root   root   4096 Aug 29  2016 ..
-rw-r--r--  1 level1 level1  220 Aug 29  2016 .bash_logout
-rw-r--r--  1 level1 level1 3771 Aug 29  2016 .bashrc
drwx------  2 level1 level1 4096 Jun 25  2019 .cache
-rw-------  1 level1 level1   28 Jun 25  2019 .lesshst
-rw-r--r--  1 level1 level1  655 Aug 29  2016 .profile
-rw-r--r--  1 level1 root     70 Aug 29  2016 README
-rw-r--r--  1 level1 root     82 Jun 24  2019 .README
drwx------  2 level1 level1 4096 Jun 25  2019 .ssh
-rw-------  1 level1 level1  741 Jun 24  2019 .viminfo
-rw-------  1 level1 level1  168 Jun 25  2019 .Xauthority
level1@management:~$ cat .README 
Congrats! You found the hidden file.

flag{level2:W7RU>asPZP846[P6J6(44PVXAW7R6p}
## Level 2
level2@management:~$ cat this\ file\ has\ tons\ of\ spaces 
flag{level3:t{4Z49Z9RUj4MZ2}bMg32e7u9ek;yv}




